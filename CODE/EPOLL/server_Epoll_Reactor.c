#include "wrap.h"

#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PORT 9999

void recvdata(int fd, int events, void* arg);
void senddata(int fd, int events, void* arg);

struct myevent_s {
    int fd;
    int events;
    void* arg;
    void (*call_back)(int fd, int events, void* arg);

    int status;

    int last_active;

    char buf[BUFLEN];
    int len;
};

int g_efd;

struct myevent_s g_events[MAX_EVENTS];

void eventset(struct myevent_s* ev, int fd, void (*call_back)(int, int, void*), void* arg) {
    ev->fd = fd;
    ev->events = 0;
    ev->arg = arg;
    ev->call_back = call_back;

    ev->status = 0;

    ev->last_active = time(NULL);

    // memset(ev->buf, 0, sizeof(ev->buf));
    // ev->len = 0;

    // bzero(&ev->buf, sizeof(ev->buf));
    // ev->len = 0;

    return;
}

void eventdel(int efd, struct myevent_s* ev) {
    if (ev->status != 1) return;

    ev->status = 0;

    struct epoll_event epv = { {0, { 0 }} };
    epv.data.ptr = NULL;

    int ret = epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
    if (ret == -1) perr_exit("epoll_ctl error\n");

    return;
}

// eventadd(lfd, EPOLLIN, &g_events[MAX_EVENTS]);
void eventadd(int efd, int events, struct myevent_s* ev) {
    int op = -1;

    struct epoll_event epv = { 0, {0} };

    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if (ev->status == 0) {
        op = EPOLL_CTL_ADD;

        ev->status = 1;
    }

    int ret = epoll_ctl(efd, op, ev->fd, &epv);

    if (ret == -1) {
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
    }
    else {
        printf("event add OK [fd=%d], op=%d, events[%d]\n", ev->fd, op, events);
    }

    return;
}

void acceptconn(int lfd, int events, void* arg) {
    int i = -1, ret = -1;

    struct sockaddr_in clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);

    int cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

    do {
        for (i = 0; i < MAX_EVENTS; i++) {
            if (g_events[i].status == 0) break;
        }

        if (i == MAX_EVENTS) perr_exit("client too much\n");


        ret = fcntl(lfd, F_SETFL, O_NONBLOCK);
        if (ret == -1) perr_exit("fcntl error\n");

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(cfd, EPOLLIN, &g_events[i]);

    } while (0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(clit_addr.sin_addr), ntohs(clit_addr.sin_port), g_events[i].last_active, i);

    return;
}

void recvdata(int fd, int events, void* arg) {
    struct myevent_s* ev = (struct myevent_s*)arg;

    int len = Read(fd, ev->buf, sizeof(ev->buf));

    eventdel(g_efd, ev);

    if (len > 0) {
        ev->len = len;
        ev->buf[len] = '\0';

        printf("C[%d]:%s\n", fd, ev->buf);

        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    }
    else if (len == 0) {
        Close(fd);

        printf("[fd=%d] pos[%ld], closed\n", fd, ev - g_events);
    }
    else {
        Close(fd);

        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }

    return;
}

void senddata(int fd, int events, void* arg) {
    struct myevent_s* ev = (struct myevent_s*)arg;

    int len = Write(fd, ev->buf, ev->len);

    eventdel(g_efd, ev);

    if (len > 0) {
        printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);

        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);
    }
    else {
        close(ev->fd);

        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }
}

void initlistensocket(int g_efd, int port) {
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int ret = fcntl(lfd, F_SETFL, O_NONBLOCK);
    if (ret == -1) perr_exit("fcntl error\n");

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(g_efd, EPOLLIN, &g_events[MAX_EVENTS]);

    return;
}

int main(int argc, char* argv[]) {
    int port = SERV_PORT;

    if (argc == 2) port = atoi(argv[1]);

    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd == -1) perr_exit("epoll_create error");

    initlistensocket(g_efd, port);

    struct epoll_event events1[MAX_EVENTS + 1];

    printf("server running : port[%d]\n", port);

    int checkPort = 0, i = 0;

    while (1) {
        
        /*
        long long now = time(NULL);
        for (i = 0; i < 100; i++, checkPort++) {
            if (checkPort == MAX_EVENTS) checkPort = 0;
            if (g_events[checkPort].status != 1) continue;

            long duration = now - g_events[checkPort].last_active;

            if (duration >= 60) {
                close(g_events[checkPort].fd);

                eventdel(g_efd, &g_events[checkPort]);

                printf("[fd=%d] timeout\n", g_events[checkPort].fd);
            }

        }
        */

        
        printf("g_efd = %d, &events1 = %d, MAX_EVENTS = %d\n");

        int nready = epoll_wait(g_efd, events1, MAX_EVENTS + 1, 1000);
        if (nready == -1) perr_exit("epoll_wait error");

        for (i = 0; i < nready; i++) {
            struct myevent_s* ev = (struct myevent_s*)events1[i].data.ptr;

            if ((events1[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
                ev->call_back(ev->fd, events1[i].events, ev->arg);
            }

            if ((events1[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
                ev->call_back(ev->fd, events1[i].events, ev->arg);
            }
        }

    }


    return 0;
}
