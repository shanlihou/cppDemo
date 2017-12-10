#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "FileInfo.h"
using namespace std;
string FileInfo::getTimeStr(time_t time)
{
	char buf[20];
	tm *tmPtr = gmtime(&time);
	sprintf(buf, "%02d-%02d-%02d %02d:%02d%s", tmPtr->tm_mon + 1, tmPtr->tm_mday, tmPtr->tm_year % 100,
			tmPtr->tm_hour > 11 ? tmPtr->tm_hour - 12 : tmPtr->tm_hour, tmPtr->tm_min,
			tmPtr->tm_hour > 11 ? "PM" : "AM");
	string retStr(buf);
	return retStr;
}

long FileInfo::getFileSize(const char* fileName)
{
	struct stat statBuf;
	lstat(fileName, &statBuf);
	return statBuf.st_size;
}

int FileInfo::getReadFd(const char* fileName)
{
	int fd = open(fileName, O_RDONLY);
	return fd;
}

int FileInfo::getWriteFd(const char* fileName)
{
	int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IXOTH);
	return fd;
}
void FileInfo::cwddir(const char* path)
{
	mCurPath = mBaseDir + path;
	printf("will chdir:%s\n", mCurPath.c_str());
	chdir(mCurPath.c_str());
	printf("chdir ok:%s\n", mCurPath.c_str());
}

void FileInfo::deleteFile(const char *path)
{
	mCurPath = mBaseDir + path;
	int ret = unlink(mCurPath.c_str());
	printf("unlink path:%s, ret:%d\n", path, ret);
	if (ret == -1)
		perror("");
}

list<string> FileInfo::listDir(const char* path)
{
	mCurPath = mBaseDir + path;
	DIR* dirp = opendir(mCurPath.c_str());
	struct dirent* entry;
	struct stat statBuf;
	char tmpBuf[50];
	list<string> retList;
	if (dirp == NULL)
	{
		return retList;
	}

	chdir(mCurPath.c_str());
	while((entry = readdir(dirp)) != NULL)
	{
		string tmp("");
		lstat(entry->d_name, &statBuf);
		tmp += getTimeStr(statBuf.st_mtime) + " ";
		if (S_ISDIR(statBuf.st_mode))
		{
			sprintf(tmpBuf, "%s ", "<DIR> ");
		}
		else
		{
			sprintf(tmpBuf, "%d ", statBuf.st_size);
		}
		tmp += tmpBuf;
		tmp += entry->d_name;
		tmp += "\r\n";
		retList.push_back(tmp);
	}
	closedir(dirp);
	return retList;
}
