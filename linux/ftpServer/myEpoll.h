#ifndef __MY_EPOLL_H__
#define __MY_EPOLL_H__
#include <stdio.h>
#include <map>
#include "FTPProtol.h"
#include "Record.h"
enum FDTYPE
{
	CMD_CLIENT = 0,
	DATA_SERVER,
	DATA_CLIENT
};
struct fdInfo
{
	fdInfo():type(CMD_CLIENT), ptr(NULL)
	{
		Record::getInstance()->addSockFdNum();
		//printf("fdInfo()\n");
	};
	~fdInfo()
	{
		Record::getInstance()->releaseSockFdNum();
		//printf("~fdInfo()\n");
	};
	FDTYPE type;
	FTPProtol* ptr;
};
class MyEpoll
{
public:
	static MyEpoll* getInstance();
	void init();
	void run();
	int addDataServer(FTPProtol* ftpPtr);
	void closeFd(int fd);
private:
	MyEpoll():mEpollFd(-1), mServerFd(-1), mDataFd(-1)
	{
		init();
	};
	~MyEpoll();
	int setNonBlock(int fd);
	void addChannel(int servFd);
	void recvCmd(int fd);
	void recvData(int fd);

	static MyEpoll* instance;
	int mEpollFd;
	int mServerFd;
	int mDataFd;
	std::map<int, fdInfo> mFdMap;
};
#endif
