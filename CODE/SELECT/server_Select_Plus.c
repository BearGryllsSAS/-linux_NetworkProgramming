#include "wrap.h"

#define SERV_PORT 9999

int main(int argc, char* argv[]) {
    int i = -1, j = -1, n = -1, nready = -1;
    int lfd = -1, cfd = -1;
    int maxfd = -1, maxi = -1;
    int client[FD_SETSIZE];
    char buf[BUFSIZ], clit_IP[INET_ADDRSTRLEN];

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    socklen_t clit_addr_len = sizeof(clit_addr);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    maxfd = lfd;

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    fd_set rset, allset;

    FD_ZERO(&allset);
    FD_SET(lfd, &allset);

    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++) client[i] = -1;

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

            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = cfd;

                    break;
                }
            }

            if (i == FD_SETSIZE) {
                fputs("too many clients\n", STDERR_FILENO);

                exit(1);
            }

            if (i > maxi) maxi = i;

            FD_SET(cfd, &allset);

            if (maxfd < cfd) maxfd = cfd;

            if (--nready == 0) continue;
        }

        for (i = 0; i <= maxi; i++) {
            if (client[i] < 0) continue;

            if (FD_ISSET(client[i], &rset)) {
                if ((n = Read(client[i], buf, sizeof(buf))) == 0) {
                    printf("-----closed-----\n");

                    Close(client[i]);

                    FD_CLR(client[i], &rset);

                    client[i] = -1;
                }
                else {
                     Write(STDOUT_FILENO, buf, n);
                    
                    for (j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    
                    Write(client[i], buf, n);
                }
            }
        }

    }

    Close(lfd);

    return 0;
}
