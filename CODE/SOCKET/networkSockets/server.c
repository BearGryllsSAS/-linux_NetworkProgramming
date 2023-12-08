 /*************************************************************************
 > File Name: server.c
 > Author: ai haibara
 > Created Time: 2023年12月06日 星期三 20时14分01秒
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 9527

void sys_err(const char* str) {
    perror(str);
    exit(1);
}

int main(int argc, char* argv[]) {
    int lfd = 0, cfd = 0;
    int ret = -1, i = -1;
    char buf[BUFSIZ], clit_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) sys_err("socket error");

    bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(lfd, 128);

    cfd = accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);
    if (cfd == -1) sys_err("accept error");

    printf("clit is conneted. ip is %s, port is %d\n", 
        inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
        ntohs(clit_addr.sin_port)
    );

    while (1) {
        ret = read(cfd, buf, sizeof(buf));
        
        write(STDOUT_FILENO, buf, ret);

        for (i = 0; i < ret; i++) buf[i] = toupper(buf[i]);       
        
        write(cfd, buf, ret);
    }

    close(lfd);

    close(cfd);
    
    return 0;
} 
