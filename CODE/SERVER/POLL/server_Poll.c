#include "wrap.h"

#define SERV_PORT 9999
#define OPEN_MAX 1024

int main(int argc, char* argv[]) {
    ssize_t n = -1;
    int nready = -1;
    int i = -1, j = -1;
    int lfd = -1, cfd = -1, sockfd = -1;
    char buf[BUFSIZ], clit_IP[INET_ADDRSTRLEN];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    struct pollfd client[OPEN_MAX];
    client[0].fd = lfd;
    client[0].events = POLLIN;

    for (i = 1; i < OPEN_MAX; i++) {
        client[i].fd = -1;
    }

    int maxi = 0;

    while (1) {
        nready = poll(client, maxi + 1, -1);

        if (client[0].revents & EPOLLIN) {
            clit_addr_len = sizeof(clit_addr);

            cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

            for (i = 1; i < OPEN_MAX; i++) {
                if (client[i].fd < 0) break;
            }

            if (i == OPEN_MAX) perr_exit("client to much\n");

            if (i > maxi) maxi = i;

            client[i].fd = cfd;
            client[i].events = POLLIN;

            printf("received from %s at PORT %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
                ntohs(clit_addr.sin_port));

            if (--nready == 0) continue;
        }

        for (i = 1; i < maxi; i++) {
            if ((sockfd = client[i].fd) < 0) continue;

            if ((n = Read(sockfd, buf, sizeof(buf))) < 0) {
                if (errno == ECONNRESET) {
                    printf("client[%d] aborted connection\n", i);

                    Close(sockfd);
                    client[i].fd = -1;
                }
                else {
                    perr_exit("read error");
                }
            }
            else if (n == 0) {
                printf("client[%d] is closed\n", i);

                Close(sockfd);
                client[i].fd = -1;
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

    Close(lfd);

    return 0;
}
