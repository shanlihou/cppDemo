#include "Session.h"
#include <WinSock2.h>
#include <stdio.h>
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"

Session::Session(bufferevent *bev, FILE *fp)
{
	m_bev = bev;
	m_fp = fp;
	m_needClose = false;
	m_isSend = false;
	m_fpRef = 2;
}

void Session::release()
{
//	fprintf(m_fp, "free fd:%d\n", bufferevent_getfd(m_bev));
	if (m_bev)
	{
//		fprintf(m_fp, "free bev\n");
		bufferevent_free(m_bev);
		m_bev = NULL;
	}
	fpRelease();
	if ((!m_fpRef) && m_fp)
	{
//		fprintf(m_fp, "free fp\n");
		fclose(m_fp);
	}
	if (m_friend)
	{
		m_friend->setFriend(NULL);
		m_friend = NULL;
	}
}


void Session::setIsSend(bool isSend)
{
	m_isSend = isSend;
}
void Session::setNeedClose(bool needClose)
{
	m_needClose = needClose;
}
bool Session::getIsSend()
{
	return m_isSend;
}
bool Session::getNeedClose()
{
	return m_needClose;
}
void Session::fpRelease()
{
	m_fpRef--;
	if (m_friend)
	{
		m_friend->m_fpRef--;
	}
}

Session *Session::getFriend()
{
	return m_friend;
}
Session::~Session()
{
	release();
}

void Session::setFriend(Session *fri)
{
	m_friend = fri;
}

/*
static void conn_readcb(struct bufferevent *bev, void *user_data)
{
	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	char buf[1024];
	int n;
	struct evbuffer *input = bufferevent_get_input(bev);
    memset(buf, 0, sizeof(buf));
	while((n = evbuffer_remove(input, buf, sizeof(buf) - 1)) > 0)
	{
		buf[n] = '\0';
		printf("n is %d\n", n);
		printf("%s\n", buf);
	}
}

void event_cb(bufferevent *bev, short event, void *arg) {
    if (event & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("Some other error.\n");
    }
    else if (event & BEV_EVENT_CONNECTED) {
        printf("Client has successfully cliented.\n");
        return;
    }

    bufferevent_free(bev);

    // free event_cmd
    // need struct as event is defined as parameter
    struct event *ev = (struct event *)arg;
    event_free(ev);
}
int Session::tcpConnectServer(bufferevent *ev)
{
    bufferevent *bev = bufferevent_socket_new(m_base, -1, BEV_OPT_CLOSE_ON_FREE);

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
	server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.108");

    bufferevent_socket_connect(bev, (sockaddr *)&server_addr, sizeof(server_addr));

    bufferevent_setcb(bev, conn_readcb, NULL, event_cb, (void *)ev);
    bufferevent_enable(bev, EV_READ|EV_PERSIST);

    event_base_dispatch(m_base);

    printf("Finished\n");
    return 0;
};*/