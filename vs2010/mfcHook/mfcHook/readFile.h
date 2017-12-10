#ifndef _READ_FILE_H_
#define _READ_FILE_H_
class FileReader
{
public:
	FileReader(const char *szPath);
	~FileReader();
	int read(char* buf, int offset, int size);
	int readNext(char *buf, int& offset, int size);
	int readPrev(char *buf, int& offset, int size);
private:
	FILE *fp;
};
#endif