#ifndef __RECORD_H__
#define __RECORD_H__
class Record
{
public:
	static Record *getInstance();
	void addSockFdNum();
	void releaseSockFdNum();

	void addProtolNum();
	void releaseProtolNum();

	void printStatus();
private:
	static Record* instance;
	Record():protolNum(0), sockFdNum(0)
	{
	};

	int protolNum;
	int sockFdNum;
};
#endif
