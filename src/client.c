#include "wrap.h"

int main(int argc, char* argv[]) {
    int ret = -1, flag = -1;

    char buf[BUFSIZ];

    int cfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "101.200.235.151", &serv_addr.sin_addr.s_addr);
    ret = Connect(cfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    flag = fcntl(STDIN_FILENO, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flag);

    flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    while (1) {
        ret = Read(STDIN_FILENO, buf, sizeof(buf));

        Write(cfd, buf, ret);

        if (!strncmp(buf, "exit", 4)) break;

        ret = Read(cfd, buf, sizeof(buf));

        Write(STDOUT_FILENO, buf, ret);
    }

    Close(cfd);

    return 0;
}
