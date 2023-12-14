#include "wrap.h"

#define SERV_PORT 7777

struct s_Info {         // 定义一个结构体，捆绑地址结构和cfd
    struct sockaddr_in clint_addr;
    int cfd;
};

void* do_Work(int* arg) {

    // 调试
    // printf("hello world---1\n");

    struct s_Info* ts = (struct s_Info*)arg;
    int ret = -1, i = -1;
    char buf[BUFSIZ], clit_IP[BUFSIZ];

    printf("----the client %d is connected! ip is %s, port is %d-----\n",
        ts->cfd,
        inet_ntop(AF_INET, &(ts->clint_addr.sin_addr.s_addr), clit_IP, sizeof(clit_IP)),
        ntohs(ts->clint_addr.sin_port)
    );

    while (1) {

        // 调试
        // printf("hello world---2\n");

        ret = Read(ts->cfd, buf, sizeof(buf));
        if (ret == 0) {
            printf("-----the client %d is closed-----\n", ts->cfd);

            break;
        }
        
        // 测试
        printf("ret = %d\n", ret);

        // 调试
        // printf("hello world---3\n");

        /*
        printf("----the client %d is connected! ip is %s, port is %d-----\n",
            ts->cfd,
            inet_ntop(AF_INET, &(ts->clint_addr.sin_addr.s_addr), clit_IP, sizeof(clit_IP)),
            ntohs(ts->clint_addr.sin_port)
        );
        */

        // 调试
        // printf("hello world---4\n");

        write(STDOUT_FILENO, buf, ret);

        for (i = 0; i < ret; i++) {
            buf[i] = toupper(buf[i]);
        }

        write(ts->cfd, buf, ret);
    }

    close(ts->cfd);

    return (void*)0;
}

int main(int argc, char* argv[]) {
    int lfd = -1, cfd = -1;
    pthread_t tid = -1;

    struct s_Info ts[256];
    int i = 0;

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    printf("-----The server is ready-----\n");

    // int flags = fcntl(lfd, F_GETFL, 0);
    // fcntl(lfd, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        // 测试
        printf("i = %d\n", i);

        clit_addr_len = sizeof(clit_addr);

        cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

        ts[i].clint_addr = clit_addr;
        ts[i].cfd = cfd;

        pthread_create(&tid, NULL, do_Work, (void*)&ts[i]);

        pthread_detach(tid);

        i++;
    }

    return 0;
}
