#ifndef __FILEINFO__
#define __FILEINFO__
#include <string>
#include <list>
class FileInfo
{
public:

#ifdef __X86__
	FileInfo():mBaseDir("/home/32355"), mCurPath("")
#else
	FileInfo():mBaseDir("/mnt/sd"), mCurPath("")
#endif
	{

	}
	std::list<std::string> listDir(const char* path);
	long getFileSize(const char* fileName);
	int getReadFd(const char* fileName);
	void cwddir(const char* path);
	void deleteFile(const char *path);
	int getWriteFd(const char* fileName);
private:
	std::string getTimeStr(time_t time);

	std::string mBaseDir;
	std::string mCurPath;
};
#endif
