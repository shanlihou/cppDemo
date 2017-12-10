#include <stdio.h>
#include "Record.h"
Record *Record::instance = NULL;
Record *Record::getInstance()
{
	if (instance == NULL)
	{
		instance = new Record();
	}
	return instance;
}


void Record::addSockFdNum()
{
	sockFdNum++;
}
void Record::releaseSockFdNum()
{
	sockFdNum--;
}

void Record::addProtolNum()
{
	protolNum++;
}
void Record::releaseProtolNum()
{
	protolNum--;
}

void Record::printStatus()
{
	printf("cur status protol:%d, fd:%d\n", protolNum, sockFdNum);
}
