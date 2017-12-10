#ifndef __FTP_PROTOL__
#define __FTP_PROTOL__
#include <map>
#include <string>
#include "FileInfo.h"
enum IA_TYPE
{
	BINARY = 0,
	ASCII
};
enum CMD_TYPE
{
	CMD_CHANNEL = 0,
	DATA_CHANNEL
};
class FTPProtol
{
public:
	FTPProtol(int sock);
	~FTPProtol();
	void init();
	void processData(const char* data);
	void setChnType(CMD_TYPE type);
	void setServerFd(int fd);
	void releaseCount();
	void addCount();
	void setDataFd(int fd);

	std::string getArgs(const char* data);
	int getFd();
	CMD_TYPE getChnType();
	int getServerFd();
	int getCount();
	void recvFile(const char *data, int len);
	void closeWrite();
private:
	void sendList();
	void sendFile(const char* fileName);

	void USER(const char* name);
	void SYST(const char* nothing);
	void PWD(const char* pwd);
	void TYPE(const char* type);
	void PASV(const char* nothing);
	void QUIT(const char* nothing);
	void OPTS(const char *args);
	void CWD(const char *CWD);
	void LIST(const char *path);
	void NOOP(const char *args);
	void HELP(const char *args);
	void SIZE(const char *name);
	void RETR(const char *name);
	void DELE(const char *name);
	void STOR(const char *name);


	int sockfd;
	int serverFd;
	int dataFd;
	int writeFd;
	int mRecvCount = 0;
	int mCount;
	FileInfo mFileInfo;
	typedef void (FTPProtol::*pFunc)(const char *data);
	typedef std::map<std::string, pFunc> key2FuncMap;
	key2FuncMap mKey2Func;
	std::string mPwd;
	std::string mFileName;
	IA_TYPE mType;
	CMD_TYPE mChnType;
};
#endif
