#ifndef __WRAP_H_
#define __WRAP_H_


#include <stdio.h>
#include <sys/wait.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void perr_exit(const char* s);
int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr);
int Bind(int fd, const struct sockaddr* sa, socklen_t salen);
int Connect(int fd, const struct sockaddr* sa, socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
ssize_t Read(int fd, void* ptr, size_t nbytes);
ssize_t Write(int fd, const void* ptr, size_t nbytes);
int Close(int fd);
ssize_t Readn(int fd, void* vptr, size_t n);
ssize_t Writen(int fd, const void* vptr, size_t n);
static ssize_t my_read(int fd, char* ptr);
ssize_t Readline(int fd, void* vptr, size_t maxlen);
int Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine) (void*), void* arg);
int Pthread_detach(pthread_t thread);
pid_t Waitpid(pid_t pid, int *status, int options);
pid_t Fork(void);
int Sigemptyset(sigset_t *set);
int Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int Setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
int Poll(struct pollfd *fds, int nfds, int timeout);
int Epoll_create(int size);
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int Epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);


#endif
