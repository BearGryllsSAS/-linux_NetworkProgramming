#include "wrap.h"

#define SERV_PORT 9999
#define MAX_SIZE 1024

int main(int argc, char* argv[]) {
    int i = -1, j = -1, n = -1, nready = -1;
    int lfd = -1, cfd = -1;
    int maxfd = -1;
    char buf[BUFSIZ], clit_IP[BUFSIZ], client[BUFSIZ];


    int cfdArr[MAX_SIZE];
    for (i = 0; i < MAX_SIZE; i++) {
        cfdArr[i] = -1;
    }
    int maxi = -1;


    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serv_addr_len = sizeof(serv_addr);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    maxfd = lfd;
    
    int opt = 1;
    Setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(lfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    Listen(lfd, 128);

    fd_set rset, allset;

    FD_ZERO(&allset);
    FD_SET(lfd, &allset);

    while (1) {
        rset = allset;

        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(lfd, &rset)) {
            clit_addr_len = sizeof(clit_addr);

            cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);
            
            printf("received from %s at PORT %d\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr, clit_IP, sizeof(clit_IP)),
                ntohs(clit_addr.sin_port)
            );

            FD_SET(cfd, &allset);

            if (maxfd < cfd) maxfd = cfd;

            
            for (i = 0; i < MAX_SIZE && cfdArr[i] != -1; i++); 
            if (i == MAX_SIZE) perr_exit("too much client!\n");
            cfdArr[i] = cfd;
            if (i > maxi) maxi = i;


            if (--nready == 0) continue;
        }

        for (i = lfd + 1; i <= maxfd; i++) {
            if (FD_ISSET(i, &rset)) {
                if ((n = Read(i, buf, sizeof(buf))) == 0) {
                    printf("-----closed-----\n");

                    Close(i);

                    FD_CLR(i, &allset);
                
                    // 待完善
                    for (j = 0; j < MAX_SIZE && cfdArr[j] == i; j++);
                    cfdArr[j] = -1;
                }
                else {
                    Write(STDOUT_FILENO, buf, n);
                    
                    for (j = 0; j <= maxi; j++) {
                        if (cfdArr[j] == i) continue;
                        
                        memset(client, 0, sizeof(client));
                        sprintf(client, "client[%d]>>> ", i);
                        Write(cfdArr[j], client, strlen(client));

                        Write(cfdArr[j], buf, n);
                    }   
                }
            }
        }

    }

    Close(lfd);

    return 0;
}
