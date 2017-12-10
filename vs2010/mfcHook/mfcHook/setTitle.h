#ifndef _SET_TITLE_H_
#define _SET_TITLE_H_
#include "stdafx.h"
#include "readFile.h"
class SetTitle
{
public:
	enum CHOICE
	{
		PREV = 0,
		NEXT,
		MINUS,
		CUR,
		QUIT,
		SCOPE_UP,
		SCOPE_DOWN,
		PASS_UP,
		PASS_DOWN
	};
	static SetTitle *getInstance();
	void setWnd(HWND hwnd);
	BOOL getWnd();
	void changeWnd(LPCTSTR lpString);
	void changeWnd(int choice);
private:
	static SetTitle *instance;
	SetTitle();
	~SetTitle();
	void readConfig();
	void setConfig();
	int mOffset;
	int mScope;
	HWND mHwnd;
	FileReader *mReadFile;
	int size;
};
#endif