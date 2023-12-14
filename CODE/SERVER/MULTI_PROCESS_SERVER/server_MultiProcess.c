#include "wrap.h"

#define SERV_PORT 9999

void catch_child(int sigNum) {
    while (waitpid(0, NULL, WNOHANG) > 0);

    return;
}

int main(int argc, char* argv[]) {
    int lfd = -1, cfd = -1;
    int ret = -1, i = -1;
    pid_t pid;
    char buf[BUFSIZ], clit_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    socklen_t serv_addr_len = sizeof(serv_addr);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    ret = Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    ret = Listen(lfd, 128);

    while (1) {
        cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

        printf("client has connected! ip is: %s, port is: %d\n",
            inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
            ntohs(clit_addr.sin_port)
        );

        pid = fork();

        if (pid > 0) {
            struct sigaction act;

            act.sa_handler = catch_child;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;

            ret = sigaction(SIGCHLD, &act, NULL);
            if (ret < 0) perr_exit("sigaction error");

            close(cfd);

            continue;
        }
        else if (pid == 0) {
            close(lfd);
            break;
        }
        else {
            perr_exit("fork error");
        }
    }

    if (pid == 0) {
        while (1) {
            ret = Read(cfd, buf, sizeof(buf));
            if (ret == 0) {
                printf("client has closed! ip is: %s, port is: %d\n",
                    inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
                    ntohs(clit_addr.sin_port)
                );

                close(cfd);

                exit(1);
            }

            Write(STDOUT_FILENO, buf, ret);

            for (i = 0; i < ret; i++) {
                buf[i] = toupper(buf[i]);
            }

            Write(cfd, buf, ret);
        }
    }

    return 0;
}
