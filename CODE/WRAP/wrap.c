#include "wrap.h"

void perr_exit(const char* s) {
	perror(s);
	exit(-1);
}

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr) {
	int n;

again:
	if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perr_exit("accept error");
	}
	return n;
}

int Bind(int fd, const struct sockaddr* sa, socklen_t salen) {
	int n;

	if ((n = bind(fd, sa, salen)) < 0)
		perr_exit("bind error");

	return n;
}

int Connect(int fd, const struct sockaddr* sa, socklen_t salen) {
	int n;

	if ((n = connect(fd, sa, salen)) < 0)
		perr_exit("connect error");

	return n;
}

int Listen(int fd, int backlog) {
	int n;

	if ((n = listen(fd, backlog)) < 0)
		perr_exit("listen error");

	return n;
}

int Socket(int family, int type, int protocol) {
	int n;

	if ((n = socket(family, type, protocol)) < 0)
		perr_exit("socket error");

	return n;
}

ssize_t Read(int fd, void* ptr, size_t nbytes) {
	ssize_t n;

again:
	if ((n = read(fd, ptr, nbytes)) < 0) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

ssize_t Write(int fd, const void* ptr, size_t nbytes) {
	ssize_t n;

again:
	if ((n = write(fd, ptr, nbytes)) < 0) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

int Close(int fd) {
	int n;
	if ((n = close(fd)) < 0)
		perr_exit("close error");

	return n;
}

/*参三: 应该读取的字节数*/
ssize_t Readn(int fd, void* vptr, size_t n) {
	size_t  nleft;              //usigned int 剩余未读取的字节数
	ssize_t nread;              //int 实际读到的字节数
	char* ptr;

	ptr = vptr;
	nleft = n;

	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		}
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

ssize_t Writen(int fd, const void* vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char* ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

static ssize_t my_read(int fd, char* ptr) {
	static int read_cnt;
	static char* read_ptr;
	static char read_buf[100];

	if (read_cnt <= 0) {
	again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return -1;
		}
		else if (read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;

	return 1;
}

ssize_t Readline(int fd, void* vptr, size_t maxlen) {
	ssize_t n, rc;
	char    c, * ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ((rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0) {
			*ptr = 0;
			return n - 1;
		}
		else
			return -1;
	}
	*ptr = 0;

	return n;
}

int Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine) (void *), void *arg) {
    int n;
   
    if ((n = pthread_create(thread, attr, start_routine, arg)) < 0) {
        perr_exit("pthread_create error");
    }

    return n;
}

int Pthread_detach(pthread_t thread) {
    int n;

    if ((n = pthread_detach(thread)) < 0) {
        perr_exit("pthread_detach error");
    }

    return n;
}

pid_t Waitpid(pid_t pid, int *status, int options) {
    pid_t n;

    if ((n = waitpid(pid, status, options)) < 0) {
        perr_exit("watipid error");
    }

    return n;
}

pid_t Fork(void) {
    pid_t n;

    if ((n = fork()) < 0) {
        perr_exit("fork error");
    }

    return n;
}

int Sigemptyset(sigset_t *set) {
    int n;

    if ((n = sigemptyset(set)) < 0) {
        perr_exit("sigemptyset error");
    }

    return n;
}

int Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    int n;

    if ((n = sigaction(signum, act, oldact)) < 0) {
        perr_exit("sigaction error");
    }

    return n;
}

int Setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen) {
    int n;

    if ((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0) {
        perr_exit("setsockopt error");
    }

    return n;
}

int Poll(struct pollfd *fds, int nfds, int timeout) {
    int n;

    if ((n = poll(fds, nfds, timeout)) < 0) {
        perr_exit("poll error");
    }

    return n;
}

int Epoll_create(int size) {
    int n;

    if ((n = epoll_create(size)) < 0) {
        perr_exit("epoll_create error");
    }

    return n;
}

int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
    int n;

    if ((n = epoll_ctl(epfd, op, fd, event)) < 0) {
        perr_exit("epoll_ctl error");
    }

    return n;
}

int Epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout) {
    int n;

    if ((n = epoll_wait(epfd, events,maxevents, timeout)) < 0) {
        perr_exit("epoll_wait error");
    }

    return n;
}
