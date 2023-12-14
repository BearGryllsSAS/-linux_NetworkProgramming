#include "wrap.h"

#define MAX_EVENTS 1024          // 监听上限数
#define BUFLEN 4096              // 通信过程中，收发双方一次能传输的最大字符长度
#define SERV_PORT 9999           // 端口号

// 相关函数声明
void recvdata(int fd, int events, void* arg);           // 接收信息的回调函数
void senddata(int fd, int events, void* arg);           // 发送信息的回调函数

// 描述就绪文件描述符（已经挂上监听红黑树的文件描述符）相关信息的结构体
struct myevent_s {
    int fd;                                              // 要监听的文件描述符                                                                             
    int events;                                          // 对应的监听事件
    void* arg;                                           // 泛型参数
    void (*call_back)(int fd, int events, void* arg);    // 该文件描述符对应的回调函数

    int status;                                          // 是否在监听红黑树上：1-＞监听  2-＞不监听

    long long last_active;                               // 记录每次加入监听红黑树的时间值

    char buf[BUFLEN];                                    // 该文件描述符的读写缓冲区
    int len;                                             // 读写缓冲区的长度
};

int g_efd;                                               // 全局变量，保存epoll_wait返回的监听红黑树的树根

struct myevent_s g_events[MAX_EVENTS + 1];               // 全局的自定义的结构体类型数组，g_events[MAX_EVENTS]存放监听文件描述符 lfd

// 将结构体myevent_s的成员变量初始化
void eventset(struct myevent_s* ev, int fd, void (*call_back)(int, int, void*), void* arg) {
    ev->fd = fd;
    ev->events = 0;
    ev->arg = arg;
    ev->call_back = call_back;

    ev->status = 0;

    ev->last_active = time(NULL);               // 每次调用eventset函数后，都重新计时

    // 一定不要设置这两行，不然读写事件的切换后，缓冲区会被清空，需要回发给客户端的消息的被清空
    // memset(ev->buf, 0, sizeof(ev->buf));    
    // ev->len = 0;

    return;
}

// 从监听红黑树上删除一个文件描述符
void eventdel(int efd, struct myevent_s* ev) {
    if (ev->status == 0) return;                // 不在监听红黑树上

    ev->status = 0;

    // struct epoll_event epv = { {0, { 0 }} };
    // epv.data.ptr = NULL;

    int ret = epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, NULL);
    if (ret == -1) perr_exit("epoll_ctl error\n");

    return;
}

// 添加文件描述符到监听红黑树上
void eventadd(int efd, int events, struct myevent_s* ev) {
    if (ev->status == 1) return;                // 已经在红黑树上
    ev->status = 1;

    struct epoll_event epv;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    int ret = epoll_ctl(efd, EPOLL_CTL_ADD, ev->fd, &epv);
    if (ret == -1) {
        printf("\n----------event  add  failed  fd[%d]  events[%d]----------\n", ev->fd, events);
    }
    else {
        printf("\n----------event  add  OK  fd[%d]  events[%d]----------\n", ev->fd, events);
    }

    return;
}

// lfd的回调函数。当有客户端连接请求，epoll_wait返回，调用该函数，与服务器建立连接
void acceptconn(int lfd, int events, void* arg) {
    int i = -1, ret = -1;

    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);

    int cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

    for (i = 0; i < MAX_EVENTS && g_events[i].status != 0; i++);        // 从全局数组g_events中找一个空闲位置
    if (i == MAX_EVENTS) perr_exit("client too much\n");

    ret = fcntl(cfd, F_SETFL, O_NONBLOCK);                              // 设置cfd为非阻塞
    if (ret == -1) perr_exit("fcntl error\n");

    eventset(&g_events[i], cfd, recvdata, &g_events[i]);                // 初始化cfd对应的g_events[i]成员变量
    eventadd(g_efd, EPOLLIN, &g_events[i]);                   // 将cfd挂上监听红黑树，监听其读事件

    printf("\n----------new connect cfd[%d]  ip[%s]  port[%d]  time[%lld]  pos[%d]----------\n", cfd, inet_ntoa(clit_addr.sin_addr), ntohs(clit_addr.sin_port), g_events[i].last_active, i);

    return;
}

// 读事件对应的回调函数
void recvdata(int cfd, int events, void* arg) {
    struct myevent_s* ev = (struct myevent_s*)arg;

    memset(ev->buf, 0, sizeof(ev->buf));
    ev->len = 0;

    int len = Read(cfd, ev->buf, sizeof(ev->buf));          // 读文件描述符, 数据存入myevent_s成员的buf中

    eventdel(g_efd, ev);                                    // 将该节点从监听红黑树上摘除

    if (len > 0) {
        ev->len = len;
        ev->buf[len] = '\0';                                // 手动添加字符串结束标记

        char str[BUFSIZ];
        sprintf(str, "\n\n>>> recv from cfd[%d]  len: %d  buf: %s", cfd, ev->len, ev->buf);
        Write(STDOUT_FILENO, str, strlen(str) - 1);

        eventset(ev, cfd, senddata, ev);                    // 初始化cfd对应的g_events成员变量
        eventadd(g_efd, EPOLLOUT, ev);            // 将cfd挂上监听红黑树，监听其写事件
    }
    else if (len == 0) {
        Close(cfd);

        printf("\n----------recv cfd[%d]  pos[%ld]  closed----------\n", cfd, ev - g_events);
    }
    else {
        Close(cfd);

        printf("\n---------- recv cfd[%d]  error[%d]: %s----------\n", cfd, errno, strerror(errno));
    }

    return;
}

// 写事件对应的回调函数
void senddata(int cfd, int events, void* arg) {
    struct myevent_s* ev = (struct myevent_s*)arg;

    int len = Write(cfd, ev->buf, sizeof(ev->buf));                         // 直接将数据回写给客户端，未作处理

    eventdel(g_efd, ev);                                            // 将该节点从监听红黑树上摘除

    if (len > 0) {
        char str[BUFSIZ];
        sprintf(str, "\n\n>>> send to cfd[%d]  len: %d  buf: %s", cfd, ev->len, ev->buf);
        Write(STDOUT_FILENO, str, strlen(str) - 1);

        eventset(ev, cfd, recvdata, ev);                            // 初始化cfd对应的g_events成员变量
        eventadd(g_efd, EPOLLIN, ev);                     // 将cfd挂上监听红黑树，监听其读事件
    }
    else {
        Close(ev->fd);

        printf("\n----------send  cfd[%d]  error[%d]: %s----------\n", cfd, errno, strerror(errno));
    }
}

void initlistensocket(int efd, int port) {
    int ret = -1;

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 设置端口复用
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) perr_exit("setsockopt error");

    ret = fcntl(lfd, F_SETFL, O_NONBLOCK);                                              // 将lfd设置为非阻塞
    if (ret == -1) perr_exit("fcntl error\n");

    ret = Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    ret = Listen(lfd, 128);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);              // 初始化lfd对应的g_events[MAX_EVENTS]成员变量
    eventadd(g_efd, EPOLLIN, &g_events[MAX_EVENTS]);                                      // 将lfd挂上监听红黑树，监听读事件
    // eventadd(g_efd, EPOLLIN | EPOLLET, &g_events[MAX_EVENTS]);                        

    printf("\n----------lfd[%d]  is  ready----------\n", lfd);

    return;
}

int main(int argc, char* argv[]) {
    int checkPos = 0, i = 0;
    unsigned short port = SERV_PORT;

    if (argc == 2) port = atoi(argv[1]);                  // 使用用户指定的端口，如未指定，则使用默认端口

    g_efd = epoll_create(MAX_EVENTS + 1);                 // 创建监听红黑树树根，返回给全局变量 g_efd        
    if (g_efd == -1) perr_exit("epoll_create error");

    initlistensocket(g_efd, port);                        // 创建并初始化监听文件描述符 lfd

    struct epoll_event events[MAX_EVENTS + 1];            // 保存已经满足就绪事件的文件描述符的数组，epoll_wait的第二个参数

    for (i = 0; i < MAX_EVENTS; i++) {                    // 初始化全局数组g_events除lfd外的所有成员都为不在监听红黑树的状态
        g_events[i].status = 0;
    }

    printf("\n----------server  running  port[%d]----------\n\n", port);

    while (1) {
        
        // 超时验证，每次测试100个连接（不测lfd）。60s内没和服务器通信，则关闭该客户端
        long long now = time(NULL);                                    // 获取当前时间
        for (i = 0; i < 100; i++, checkPos++) {                        // 一次循环检测100个，用checkPort来控制检测对象
            if (checkPos == MAX_EVENTS) checkPos = 0;
            if (g_events[checkPos].status != 1) continue;              // 如果不在监听红黑树上

            if (now - g_events[checkPos].last_active >= 5) {            // 如果客服端不活跃的时间大于60s
                Write(g_events[checkPos].fd, ">>> timeout <<<\n", strlen(">>> timeout <<<\n"));

                eventdel(g_efd, &g_events[checkPos]);                  // 将该客服端从监听红黑树上删除 --- 一定要先执行这一步，下面再关闭文件描述符
                
                Close(g_events[checkPos].fd);                          // 关闭与该客户端的连接
                
                printf("\n----------fd[%d]  timeout----------t\n", g_events[checkPos].fd);
            }

        }
        

        // 监听红黑树g_efd, 1秒没有事件满足, 返回 0
        int nready = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nready == -1) perr_exit("epoll_wait error");

        for (i = 0; i < nready; i++) {
            // 使用自定义的myevent_s结构体类型指针, 接收联合体data的void * ptr成员
            struct myevent_s* ev = (struct myevent_s*)events[i].data.ptr;

            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {                  // 满足读就绪事件
                ev->call_back(ev->fd, events[i].events, ev->arg);                          // 调用该文件描述符已经设置好的读回调
            }

            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {                // 满足写就绪事件
                ev->call_back(ev->fd, events[i].events, ev->arg);                          // 调用该文件描述符已经设置好的写回调                 
            }
        }

    }

    // 退出前释放所有资源

    return 0;
}
