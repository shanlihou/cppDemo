#ifndef _SESSION_H_
#define _SESSION_H_
#include "event2/event.h"
#include "event2/bufferevent.h"
class Session
{
public:
	Session(bufferevent *bev, FILE *fp);
	void setFriend(Session *fri);
	void release();
	void setIsSend(bool isSend);
	void setNeedClose(bool needClose);
	bool getIsSend();
	bool getNeedClose();
	void fpRelease();
	Session *getFriend();
	~Session();
//	int tcpConnectServer(bufferevent *bev);
public:
	FILE *m_fp;
	bufferevent *m_bev;
private:
	bool m_isSend;
	bool m_needClose;
	Session *m_friend;
	int m_fpRef;
};
#endif