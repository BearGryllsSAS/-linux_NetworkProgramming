#include "wrap.h"

#define SERV_PORT 9999

int main(int argc, char* argv[]) {
    int i = -1, j = -1, n = -1, nready = -1;
    int lfd = -1, cfd = -1;
    int maxfd = -1;
    char buf[BUFSIZ], clit_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    socklen_t clit_addr_len = sizeof(clit_addr);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    maxfd = lfd;

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    fd_set rset, allset;

    FD_ZERO(&allset);
    FD_SET(lfd, &allset);

    while (1) {
        rset = allset;

        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) perr_exit("select error");

        if (FD_ISSET(lfd, &rset)) {
            clit_addr_len = sizeof(clit_addr);

            cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);
            
            printf("received from %s at PORT %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr, clit_IP, sizeof(clit_IP)),
                ntohs(clit_addr.sin_port)
            );

            FD_SET(cfd, &allset);

            if (maxfd < cfd) maxfd = cfd;

            if (--nready == 0) continue;
        }

        for (i = lfd + 1; i <= maxfd; i++) {
            if (FD_ISSET(i, &rset)) {
                if ((n = Read(i, buf, sizeof(buf))) == 0) {
                    printf("-----closed-----\n");

                    Close(i);

                    FD_CLR(i, &rset);
                }
                else {
                    Write(STDOUT_FILENO, buf, n);
                    
                    for (j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);
                    }

                    Write(i, buf, n);
                }
            }
        }

    }

    Close(lfd);

    return 0;
}
