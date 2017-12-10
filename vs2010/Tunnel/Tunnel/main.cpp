// LibeventTest.cpp : 定义控制台应用程序的入口点。
//

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif
#include <string>
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"
#include <WinSock2.h>
#include "Session.h"

static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 80;

static int count = 0;
#define LOG(fmt, ...) //fprintf(sess->m_fp, fmt, ##__VA_ARGS__)
static void conn_writecb(struct bufferevent *bev, void *user_data)
{
//	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    struct evbuffer *output = bufferevent_get_output(bev);
	Session *sess = (Session *)user_data;
	LOG("write:%d\n", bufferevent_getfd(bev));
	LOG("write buf length:%d\n", evbuffer_get_length(output));
	if (evbuffer_get_length(output) == 0) 
    {
//        printf("flushed answer\n");
		sess->setIsSend(false);
		if (sess->getNeedClose())
		{
			delete sess;
		}
    }
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
//	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);	
	Session *sess = (Session *)user_data;
	LOG("read:%d\n", bufferevent_getfd(bev));
    if (events & BEV_EVENT_EOF) 
    {
        LOG("Connection closed.\n");
    } 
    else if (events & BEV_EVENT_ERROR) 
    {
        LOG("Got an error on the connection: %s\n",
	            strerror(errno));/*XXX win32*/
    }else if (events & BEV_EVENT_CONNECTED) {
        LOG("Client has successfully cliented.\n");
		return;
    }
    /* None of the other events can happen here, since we haven't enabled
     * timeouts */
	Session *other_sess = sess->getFriend();
	if (other_sess)
	{
		if (!other_sess->getIsSend())
		{
			delete other_sess;
		}else
		{
			other_sess->setNeedClose(true);
		}
	}
	delete sess;
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
//	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    struct event_base *base = (struct event_base *)user_data;
    struct timeval delay = { 2, 0 };

//    printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

    event_base_loopexit(base, &delay);
}

static void conn_readcb(struct bufferevent *bev, void *user_data)
{
//	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
	char buf[1024];
	int n;
	std::string tmpSend("");

	struct evbuffer *input = bufferevent_get_input(bev);
	Session *sess = (Session *)user_data;
	Session *other_sess = sess->getFriend();
	
	LOG("read:%d\n", bufferevent_getfd(bev));
	while((n = evbuffer_remove(input, buf, sizeof(buf) - 1)) > 0)
	{
		if (other_sess && other_sess->m_bev)
		{
			int ret = bufferevent_write(other_sess->m_bev, buf, n);
			LOG("\nret:%d\n", ret);
			other_sess->setIsSend(true);
		}
		buf[n] = 0;
		tmpSend += buf;
//		buf[n] = 0;
//		printf("%s", buf);
	}
	LOG("%s", tmpSend.c_str());
	LOG("\n\n");
}

Session *tcpConnectServer(event_base *base, FILE *fp)
{
    bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
	server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.108");

    bufferevent_socket_connect(bev, (sockaddr *)&server_addr, sizeof(server_addr));
	
	Session *sess = new Session(bev, fp);
    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void *)sess);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
 //   printf("Finished\n");
    return sess;
};
FILE * getFp()
{
	char buf[32];
	sprintf(buf, "D:\\http\\%d.txt", count);
	count++;
	return fopen(buf, "w");
}

static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
//	printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
    event_base *base = (struct event_base *)user_data;
    bufferevent *bev;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) 
    {
        fprintf(stderr, "Error constructing bufferevent!");
        event_base_loopbreak(base);
        return;
    }
	FILE *fp = NULL;
	Session *sess = new Session(bev, fp);

	Session *cl_sess = tcpConnectServer(base, fp);
	LOG("%d->%d\n", bufferevent_getfd(bev), bufferevent_getfd(cl_sess->m_bev));

	sess->setFriend(cl_sess);
	cl_sess->setFriend(sess);

    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void *)sess);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main(int argc, char **argv)
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct event *signal_event;

    struct sockaddr_in sin;
#ifdef WIN32
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);
#endif

    base = event_base_new();
    if (!base) 
    {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);

    listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
        LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin,
        sizeof(sin));

    if (!listener) 
    {
        fprintf(stderr, "Could not create a listener!\n");
        return 1;
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

    if (!signal_event || event_add(signal_event, NULL)<0) 
    {
        fprintf(stderr, "Could not create/add a signal event!\n");
        return 1;
    }

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    printf("done\n");
    return 0;
}