#include "stdafx.h"
#include "readFile.h"

FileReader::FileReader(const char * szPath)
{
	fp = fopen(szPath, "r");
}

FileReader::~FileReader()
{
	if (fp != NULL)
	{
		fclose(fp);
	}
}

int FileReader::read(char* buf, int offset, int size)
{
	int ret = fseek(fp, offset, SEEK_SET);
	int ch;
	int i;
	if (ret == -1)
		return -1;
	for (i = 0; i < size; i++)
	{
		ch = fgetc(fp);
		if (ch == EOF)
		{
			break;
		}
		buf[i] = ch;
	}
	buf[i] = '\0';
	return 0;
}

int FileReader::readNext(char *buf, int& offset, int size)
{
	int ret = fseek(fp, offset, SEEK_SET);
	int i = 0;
	int ch;
	if (ret == -1)
		return -1;
	while (i < size - 1)
	{
		ch = fgetc(fp);
		if (ch == EOF)
		{
			break;
		}else if(ch & 0x80)
		{
			ch = fgetc(fp);
			i++;
			if (ch == EOF)
			{
				break;	
			}
		}
		i++;
	}
	offset += i;
	i = 0;
	while (i < size)
	{
		ch = fgetc(fp);
		if (ch == EOF)
		{
			break;
		}else if (ch == '\n')
		{
			ch = ';';
		}
		buf[i] = (char)ch;
		i++;
	}
	buf[i] = '\0';
	return 0;
}

int FileReader::readPrev(char *buf, int& offset, int size)
{
	int tmpOffset = offset;
	int i = 0;
	int ch;
	int end = size - 1;
	bool state = false;
	bool move = false;
	offset -= size;
	if (offset < 0)
	{
		end = tmpOffset - 1;
		offset = 0;
	}
	int ret = fseek(fp, offset, SEEK_SET);
	if (ret == -1)
	{
		return -1;
	}

	while (i < size)
	{
		ch = fgetc(fp);
		if (ch == EOF)
		{
			break;
		}else if (ch == '\n')
		{
			ch = ';';
		}else if (ch & 0x80)
		{
			state = !state;
		}
		
		if (i == end && state)
		{
			move = true;
		}
		buf[i] = ch;
		i++;
	}
	buf[i] = '\0';
	if (move)
	{
		i = 0;
		while(i < size)
		{
			buf[i] = buf[i + 1];
			i++;
		}
	}
	return 0;
}