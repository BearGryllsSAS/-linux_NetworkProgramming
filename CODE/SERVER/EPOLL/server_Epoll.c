#include "wrap.h"

#define SERV_PORT 9999
#define OPEN_MAX 5000

int main() {
    int nready = -1, ret = -1;
    ssize_t n = -1;
    int lfd = -1, cfd = -1, sockfd = -1;
    int i = -1, j = -1;
    char buf[BUFSIZ], clit_IP[INET_ADDRSTRLEN];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    Setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    int epfd = Epoll_create(OPEN_MAX);

    struct epoll_event tep, ep[OPEN_MAX];

    tep.data.fd = lfd;
    tep.events = EPOLLIN;

    ret = Epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &tep);

    while (1) {
        nready = Epoll_wait(epfd, ep, OPEN_MAX, -1);

        for (i = 0; i < nready; i++) {
            if (ep[i].data.fd == lfd) {
                clit_addr_len = sizeof(clit_addr);

                cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

                tep.data.fd = cfd;
                tep.events = EPOLLIN;

                ret = Epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tep);
                
                printf("received from %s at PORT %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
                ntohs(clit_addr.sin_port));
            }
            else {
                sockfd = ep[i].data.fd;

                n = Read(sockfd, buf, sizeof(buf));

                if (n < 0) {
                    perror("Read n < 0\n");

                    ret = Epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);

                    Close(sockfd);
                }
                else if (n == 0) {
                    ret = Epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);

                    Close(sockfd);

                    printf("client[%d] close connection\n", sockfd);
                }
                else {
                    Write(STDOUT_FILENO, buf, n);

                    for (j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    
                    Write(sockfd, buf, n);
                }
            }
        }
    }

    Close(lfd);

    return 0;
}
