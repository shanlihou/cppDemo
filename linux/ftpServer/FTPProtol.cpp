#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "FTPProtol.h"
#include <list>
#include "myEpoll.h"
#include "Record.h"
using namespace std;
FTPProtol::FTPProtol(int sock):sockfd(sock), writeFd(-1), mPwd("/"), mType(BINARY), mChnType(CMD_CHANNEL), mFileName("")
{
	Record::getInstance()->addProtolNum();
	init();
}
FTPProtol::~FTPProtol()
{
	Record::getInstance()->releaseProtolNum();
	close(sockfd);
}

void FTPProtol::init()
{
	mKey2Func.insert(pair<std::string, pFunc>(std::string("USER"), &FTPProtol::USER));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("SYST"), &FTPProtol::SYST));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("PWD"), &FTPProtol::PWD));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("TYPE"), &FTPProtol::TYPE));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("PASV"), &FTPProtol::PASV));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("QUIT"), &FTPProtol::QUIT));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("OPTS"), &FTPProtol::OPTS));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("CWD"), &FTPProtol::CWD));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("LIST"), &FTPProtol::LIST));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("NOOP"), &FTPProtol::NOOP));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("HELP"), &FTPProtol::HELP));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("SIZE"), &FTPProtol::SIZE));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("RETR"), &FTPProtol::RETR));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("DELE"), &FTPProtol::DELE));
	mKey2Func.insert(pair<std::string, pFunc>(std::string("STOR"), &FTPProtol::STOR));
	//mKey2Func["USER"] = &FTPProtol::USER;
}
int FTPProtol::getFd()
{
	return sockfd;
}
void FTPProtol::releaseCount()
{
	mCount--;
}
void FTPProtol::addCount()
{
	mCount++;
}
int FTPProtol::getCount()
{
	return mCount;
}

void FTPProtol::setServerFd(int fd)
{
	serverFd = fd;
}

int FTPProtol::getServerFd()
{
	return serverFd;
}
void FTPProtol::sendList()
{
	list<string> fileList = mFileInfo.listDir(mPwd.c_str());
	list<string>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++)
	{
		printf("file:%s\n", it->c_str());
		write(dataFd, it->c_str(), strlen(it->c_str()));
	}
}

void FTPProtol::sendFile(const char* fileName)
{
	int fdRead = mFileInfo.getReadFd(fileName);
	int count = 0;
	int writeCount = 0;
	while(1)
	{
		char buf[1025];
		int len = read(fdRead, buf, 1024);
		if (len == -1 || len == 0)
		{
			printf("len is:%d, count:%d wCount:%d\n", len, count, writeCount);
			break;
		}
		int ret = 0;
		while(ret < len)
		{
			int tmp = write(dataFd, buf + ret, len - ret);
			ret += tmp;
		}
	}
	close(fdRead);
}

void FTPProtol::recvFile(const char *data, int len)
{
	if (writeFd == -1)
	{
		mRecvCount = 0;
		writeFd = mFileInfo.getWriteFd(mFileName.c_str());
	}
	int count = 0;
	while(count < len)
	{
		int ret = write(writeFd, data + count, len - count);
		count += ret;
	}
	mRecvCount += count;
	printf("now has get:%d\n", mRecvCount);
}
void FTPProtol::closeWrite()
{
	MyEpoll::getInstance()->closeFd(dataFd);
	releaseCount();
	releaseCount();
	dataFd = -1;
	if (writeFd != -1)
	{
		close(writeFd);
		writeFd = -1;
	}
	write(sockfd, "226 Transfer complete.\r\n", strlen("226 Transfer complete.\r\n"));
	MyEpoll::getInstance()->closeFd(serverFd);
	serverFd = -1;
}
string FTPProtol::getArgs(const char* data)
{
	int len = strlen(data);
	char* tmp = new char[len + 1];
	int tmpIndex = 0;
	bool start = false;
	int end = 0;
	string retStr("");
	for (int i = 0; i < len; i++)
	{
		printf("%d ", data[i]);
		if (data[i] != ' ' && data[i] != '\n' && data[i] != '\r')
		{
			start = true;
			end = tmpIndex + 1;
		}
		if (start)
		{
			tmp[tmpIndex++] = data[i];
		}
	}
	printf("\n");
	tmp[end] = '\0';
	retStr = tmp;
	delete [] tmp;
	return retStr;
}
void FTPProtol::USER(const char* name)
{
	printf("name is :%s\n", name);
	write(sockfd, "230 User logged in, proceed.\r\n", strlen("230 User logged in, proceed.\r\n"));
}
void FTPProtol::SYST(const char* nothing)
{
	write(sockfd, "215 linux type.\r\n", strlen("215 linux type.\r\n"));
}

void FTPProtol::PWD(const char* pwd)
{
	char buf[256];
	sprintf(buf, "257 \"%s\".\r\n", mPwd.c_str());
	write(sockfd, buf, strlen(buf));
}

void FTPProtol::TYPE(const char* type)
{
	printf("type:%s\n", type);
	if (type[0] == 'I')
	{
		mType = BINARY;
		write(sockfd, "200 Binary mode.\r\n", strlen("200 Binary mode.\r\n"));
	}
	else
	{
		mType = ASCII;
		write(sockfd, "200 Ascii mode.\r\n", strlen("200 Ascii mode.\r\n"));
	}
}

void FTPProtol::PASV(const char* nothing)
{
	char buf[256];
	int port = MyEpoll::getInstance()->addDataServer(this);
	sprintf(buf, "227 Entering Passive Mode (%s,%u,%u).\r\n", "172,29,4,132", port & 0xff, port >> 8 & 0xff);
	write(sockfd, buf, strlen(buf));

}

void FTPProtol::QUIT(const char* nothing)
{
	write(sockfd, "221 Goodbye.\r\n", strlen("221 Goodbye.\r\n"));
}

void FTPProtol::OPTS(const char *args)
{
	char buf[256];
	sprintf(buf, "501 '%s': option not supported.\r\n", args);
	write(sockfd, buf, strlen(buf));

}

void FTPProtol::CWD(const char *args)
{
	mPwd = args;
	mFileInfo.cwddir(args);
	printf("cwd %s,ok\n", args);
	write(sockfd, "250 CWD Command successful.\r\n", strlen("250 CWD Command successful.\r\n"));
}
void FTPProtol::NOOP(const char *args)
{
	write(sockfd, "200 noop command successful.\r\n", strlen("200 noop command successful.\r\n"));
}

void FTPProtol::LIST(const char *path)
{
	write(sockfd, "125 Data connection already open; Transfer starting.\r\n", strlen("125 Data connection already open; Transfer starting.\r\n"));
	sendList();
	MyEpoll::getInstance()->closeFd(serverFd);
	MyEpoll::getInstance()->closeFd(dataFd);
	releaseCount();
	releaseCount();
	printf("count is:%d\n", mCount);
	write(sockfd, "226 Transfer complete.\r\n", strlen("226 Transfer complete.\r\n"));
}
void FTPProtol::SIZE(const char *name)
{
	long fileSize = mFileInfo.getFileSize(name);
	char buf[256];
	sprintf(buf, "213 %d\r\n", fileSize);
	write(sockfd, buf, strlen(buf));
	printf("count is:%d\n", mCount);
	write(sockfd, "226 Transfer complete.\r\n", strlen("226 Transfer complete.\r\n"));
}

void FTPProtol::RETR(const char *name)
{
	write(sockfd, "125 Data connection already open; Transfer starting.\r\n", strlen("125 Data connection already open; Transfer starting.\r\n"));
	sendFile(name);
	MyEpoll::getInstance()->closeFd(dataFd);
	MyEpoll::getInstance()->closeFd(serverFd);
	releaseCount();
	releaseCount();
	dataFd = -1;
	serverFd = -1;
	printf("count is:%d\n", mCount);
	write(sockfd, "226 Transfer complete.\r\n", strlen("226 Transfer complete.\r\n"));
}
void FTPProtol::DELE(const char *name)
{
	mFileInfo.deleteFile(name);
	write(sockfd, "250 File deleted.\r\n", strlen("250 File deleted.\r\n"));
}
void FTPProtol::STOR(const char *name)
{
	mFileInfo.deleteFile(name);
	mFileName = name;
	write(sockfd, "150 Opening data connection.\r\n", strlen("150 Opening data connection.\r\n"));
}

void FTPProtol::HELP(const char *args)
{
	char help[] = "\
            214\
            USER [name], Its argument is used to specify the user's string. It is used for user authentication.\
            PASS [password], Its argument is used to specify the user password string.\
            PASV The directive requires server-DTP in a data port.\
            PORT [h1, h2, h3, h4, p1, p2] The command parameter is used for the data connection data port\
            LIST [dirpath or filename] This command allows the server to send the list to the passive DTP. If\
                 the pathname specifies a path or The other set of files, the server sends a list of files in\
                 the specified directory. Current information if you specify a file path name, the server will\
                 send the file.\
            CWD Type a directory path to change working directory.\
            PWD Get current working directory.\
            CDUP Changes the working directory on the remote host to the parent of the current directory.\
            DELE Deletes the specified remote file.\
            MKD Creates the directory specified in the RemoteDirectory parameter on the remote host.\
            RNFR [old name] This directive specifies the old pathname of the file to be renamed. This command\
                 must be followed by a \"heavy Named \"command to specify the new file pathname.\
            RNTO [new name] This directive indicates the above \"Rename\" command mentioned in the new path name\
                 of the file. These two Directive together to complete renaming files.\
            REST [position] Marks the beginning (REST) ​​The argument on behalf of the server you want to re-start\
                 the file transfer. This command and Do not send files, but skip the file specified data checkpoint.\
            RETR This command allows server-FTP send a copy of a file with the specified path name to the data\
                 connection The other end.\
            STOR This command allows server-DTP to receive data transmitted via a data connection, and data is\
                 stored as A file server site.\
            APPE This command allows server-DTP to receive data transmitted via a data connection, and data is stored\
                 as A file server site.\
            SYS  This command is used to find the server's operating system type.\
            HELP Displays help information.\
            QUIT This command terminates a user, if not being executed file transfer, the server will shut down\
                 Control connection\r\n.";
	write(sockfd, help, strlen(help));
}

void FTPProtol::processData(const char* data)
{
	printf("data is :%s, len:%d\n", data, strlen(data));
	char cmd[5];
	int cmdIndex = 0;
	if (strlen(data) < 4)
	{
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		if (data[i] != ' ' && data[i] != '\n' && data[i] != '\r')
		{
			if (data[i] >= 'a' && data[i] <= 'z')
			{
				cmd[cmdIndex++] = data[i] - 32;
			}
			else
			{
				cmd[cmdIndex++] = data[i];
			}

		}
	}
	cmd[cmdIndex] = 0;
	key2FuncMap::iterator it = mKey2Func.find(string(cmd));
	if (it != mKey2Func.end())
	{
		printf("im in:%s\n", cmd);
		(this->*mKey2Func[cmd])(getArgs(data + 4).c_str());
	}
	else
	{
		printf("not find cmd:%s, %s, %d, %d\n", data, cmd, strlen(cmd), strcmp(cmd, "noop"));
		write(sockfd, "500 Syntax error, command unrecognized.\r\n", strlen("500 Syntax error, command unrecognized.\r\n"));
	}
}


void FTPProtol::setChnType(CMD_TYPE type)
{
	mChnType = type;
}

void FTPProtol::setDataFd(int fd)
{
	dataFd = fd;
}

CMD_TYPE FTPProtol::getChnType()
{
	return mChnType;
}
