 /*************************************************************************
 > File Name: client.c
 > Author: ai haibara
 > Created Time: 2023年12月07日 星期四 13时39分12秒
************************************************************************/
#include "wrap.h"

#define SERV_PORT 9999

int main(int argc, char* argv[]) {
    int ret = -1, cfd = -1;
    int count = 20;
    char buf[BUFSIZ];

    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1 ", &serv_addr.sin_addr.s_addr);

    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    ret = Connect(cfd, (struct sockaddr*)&serv_addr, serv_addr_len);

    while (count--) {
        Write(cfd, "haibara ai\n", strlen("haibara ai\n"));

        ret = Read(cfd, buf, sizeof(buf));

        Write(STDOUT_FILENO, buf, ret);

        sleep(1);
    }

    close(cfd);

    return 0;
}
