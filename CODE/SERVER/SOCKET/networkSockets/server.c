 /*************************************************************************
 > File Name: server.c
 > Author: ai haibara
 > Created Time: 2023年12月06日 星期三 20时14分01秒
************************************************************************/
#include <wrap.h>

#define SERV_PORT 9999

int main(int argc, char* argv[]) {
    int lfd = 0, cfd = 0;
    int ret = -1, i = -1;
    char buf[BUFSIZ], clit_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len = sizeof(clit_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    Bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(lfd, 128);

    cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

    printf("clit is conneted. ip is %s, port is %d\n", 
        inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, clit_IP, sizeof(clit_IP)),
        ntohs(clit_addr.sin_port)
    );

    while (1) {
        ret = Read(cfd, buf, sizeof(buf));
        
        Write(STDOUT_FILENO, buf, ret);

        for (i = 0; i < ret; i++) buf[i] = toupper(buf[i]);       
        
        Write(cfd, buf, ret);
    }

    Close(lfd);

    Close(cfd);
    
    return 0;
} 
