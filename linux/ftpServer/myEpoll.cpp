#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include "myEpoll.h"
#include "FTPProtol.h"

using namespace std;
#define MAX_EPOLL_SIZE 1024
#define MAX_EVENTS 64
#define EPOLL_TIMEOUT 2000
#ifdef __X86__
#define PORT 9527
#else
#define PORT 9527
#endif
MyEpoll* MyEpoll::instance = NULL;
MyEpoll* MyEpoll::getInstance()
{
	if (!instance)
	{
		instance = new MyEpoll();
	}
	return instance;
}

MyEpoll::~MyEpoll()
{
}

int MyEpoll::setNonBlock(int fd)
{
	int flags, ret;
	flags = fcntl(fd, F_GETFL, 0);

	if(flags == -1)
	{
		printf("cntl failed!\n");
		return -1;
	}

	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		printf("cntl failed!\n");
	}
	return 0;
}

void MyEpoll::init()
{
	int len;
	epoll_event event, *events;
	int ret;
	struct sockaddr_in server_address;
	mEpollFd = epoll_create(MAX_EPOLL_SIZE);
	if (mEpollFd == -1)
	{
		printf("create epoll failed");
		return;
	}

	mServerFd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	printf("hello im run with:%d\n", PORT);

	bind(mServerFd, (struct sockaddr *)&server_address, sizeof(server_address));
	setNonBlock(mServerFd);
	listen(mServerFd, 10);

	event.data.fd = mServerFd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, event.data.fd, &event);
}

int MyEpoll::addDataServer(FTPProtol* ftpPtr)
{
	epoll_event event;
	int fd;
	struct sockaddr_in server_address;
	struct sockaddr_in result;
	socklen_t retLen = sizeof(result);
	fd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9527);

	//bind(fd, (struct sockaddr *)&server_address, sizeof(server_address));
	setNonBlock(fd);
	listen(fd, 10);

	int ret = getsockname(fd, (struct sockaddr *)&result, &retLen);
	printf("port:%d\n", result.sin_port);
	printf("ip:%d\n", server_address.sin_addr.s_addr);

	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, event.data.fd, &event);

	mFdMap[fd] = fdInfo();
	mFdMap[fd].type = DATA_SERVER;
	mFdMap[fd].ptr = ftpPtr;
	ftpPtr->addCount();

	ftpPtr->setServerFd(fd);
	return result.sin_port;
}

void MyEpoll::addChannel(int servFd)
{
	epoll_event event;
	while(1)
	{
		struct sockaddr in_addr;
		socklen_t in_len;
		int client_fd;

		in_len = sizeof(in_addr);
		client_fd = accept(servFd, &in_addr, &in_len);
		if (client_fd == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
			else
			{
				printf("error accept");
				break;
			}
		}
		int ret = setNonBlock(client_fd);
		if (ret == -1)
		{
			printf("set non block failed\n");
		}


		event.data.fd = client_fd;
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
		ret = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, client_fd, &event);
		if (ret == -1)
		{
			printf("add client failed!\n");
		}


		mFdMap[client_fd] = fdInfo();
		FTPProtol* tmpPtr;
		if (mServerFd == servFd)
		{
			tmpPtr = new FTPProtol(client_fd);
			mFdMap[client_fd].type = CMD_CLIENT;
			write(client_fd, "220 Welcome.\r\n", strlen("220 Welcome.\r\n"));
			printf("add client:%d\n", client_fd);
		}
		else
		{
			tmpPtr = mFdMap[servFd].ptr;
			mFdMap[client_fd].type = DATA_CLIENT;
			tmpPtr->setDataFd(client_fd);
			printf("add data client:%d\n", client_fd);
		}
		tmpPtr->addCount();
		mFdMap[client_fd].ptr = tmpPtr;
	}
}

void MyEpoll::recvData(int fd)
{
	int done = 0;
	char buffer[1024];
	FTPProtol* tmpPtr = mFdMap[fd].ptr;
	printf("fd:%d type:%d\n\n", fd, mFdMap[fd].type);
	int wholeCount = 0;
	while(1)
	{
		ssize_t count;
		memset(buffer, 0, 1024 * sizeof(char));
		count = read(fd, buffer, sizeof(buffer));
		if (count == -1)
		{
			if (errno != EAGAIN)
			{
				printf("read finish not eagain\n");
			}else
			{
				perror("");
				printf("read finish eagain\n");
			}
			done = 1;
			break;
		}else if(count == 0)
		{
			done = 2;
			printf("count == 0\n");
			break;
		}
		tmpPtr->recvFile(buffer, count);
		wholeCount += count;
	}
	printf("wholeCount is:%d\n", wholeCount);

}

void MyEpoll::recvCmd(int fd)
{
	int done = 0;
	char buffer[1024];
	string response = "";
	FTPProtol* tmpPtr = mFdMap[fd].ptr;
	printf("fd:%d type:%d\n\n", fd, mFdMap[fd].type);
	int wholeCount = 0;
	while(1)
	{
		ssize_t count;
		memset(buffer, 0, 1024 * sizeof(char));
		count = read(fd, buffer, sizeof(buffer));
		if (count == -1)
		{
			if (errno != EAGAIN)
			{
				printf("read finish not eagain\n");
			}else
			{
				perror("");
				printf("read finish eagain\n");
			}
			done = 1;
			break;
		}else if(count == 0)
		{
			done = 2;
			printf("count == 0\n");
			break;
		}
		wholeCount += count;
		response += buffer;
	}
	printf("%s", response.c_str());
	printf("\n\n");
	tmpPtr->processData(response.c_str());
}
void MyEpoll::closeFd(int fd)
{
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	mFdMap.erase(fd);
}
void MyEpoll::run()
{
	int ret;
	epoll_event *events;
	events = new epoll_event[MAX_EVENTS];
	while(1)
	{
		int n;
		n = epoll_wait(mEpollFd, events, MAX_EVENTS, EPOLL_TIMEOUT);
		for (int i = 0; i < n; i++)
		{
			printf("\n\n***********************************************\n");
			printf("events:%d, fd:%d map:%d\n", events[i].events, events[i].data.fd, mFdMap.size());
			Record::getInstance()->printStatus();
			if (mServerFd == events[i].data.fd)
			{
				addChannel(mServerFd);
			}
			else
			{
				int fd = events[i].data.fd;
				FDTYPE type = mFdMap[fd].type;
				printf("event:%d, EPOLLIN:%d\n", events[i].events, EPOLLIN);
				if (events[i].events == EPOLLIN)
				{
					if (type == DATA_SERVER)
					{
						addChannel(fd);
					}
					else if (type == CMD_CLIENT)
					{
						recvCmd(fd);
					}
					else
					{
						recvData(fd);
					}
				}
				else if ((events[i].events & EPOLLERR)
						|| (events[i].events & EPOLLHUP)
						|| (!(events[i].events & EPOLLOUT)))
				{
					FDTYPE type = mFdMap[fd].type;
					FTPProtol* tmpPtr = (FTPProtol *)mFdMap[fd].ptr;
					if (type == DATA_CLIENT)
					{
						tmpPtr->closeWrite();
					}
					else
					{
						tmpPtr->releaseCount();
						if (!tmpPtr->getCount())
						{
							delete tmpPtr;
						}
						printf("fd:%d error\n", fd);
						continue;
					}
				}
			}
		}
	}
	delete []events;
}
