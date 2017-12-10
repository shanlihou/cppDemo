#include "stdafx.h"
#include "setTitle.h"
#include "log.h"
SetTitle *SetTitle::instance = NULL;
SetTitle::SetTitle()
{
	mHwnd = NULL;
	mReadFile = NULL;
	mOffset = 0;
	mScope = 1;
	size = 200;
	if (getWnd())
	{
		mReadFile = new FileReader("D:\\tmp\\cube\\cube.txt");
		readConfig();
		changeWnd(CHOICE::CUR);
	}
}
SetTitle::~SetTitle()
{
	changeWnd(_T("securecrt"));
}
SetTitle *SetTitle::getInstance()
{
	if (instance == NULL)
	{
		instance = new SetTitle();
	}
	return instance;
}

void SetTitle::setWnd(HWND hwnd)
{
	mHwnd = hwnd;
}
BOOL SetTitle::getWnd()
{
	if (mHwnd == NULL)
	{
		mHwnd = ::FindWindow(_T("VanDyke Software - SecureCRT"), NULL);
		if (mHwnd == NULL)
		{
			LOG("wnd is still null!");
		}
	}
	return mHwnd == AfxGetMainWnd()->m_hWnd;
}
	
void SetTitle::changeWnd(LPCTSTR lpString)
{
	if (getWnd())
	{
		SetWindowText(mHwnd, lpString);
	}
}
void log_buf(const char *buf)
{
	LOG("buf is:%s", buf);
	while(*buf != '\0')
	{
		ORI_LOG("0x%x ", *buf);
		buf++;
	}
	ORI_LOG("\n");
}
void SetTitle::changeWnd(int choice)
{
	char szBuf[201];
	int ret = -1;
	switch(choice)
	{
	case CHOICE::PREV:
		ret = mReadFile->readPrev(szBuf, mOffset, size);
		break;
	case CHOICE::NEXT:
		ret = mReadFile->readNext(szBuf, mOffset, size);
		break;
	case CHOICE::MINUS:
		if (mOffset > 0)
			mOffset--;
		ret = mReadFile->read(szBuf, mOffset, size);
		break;
	case CHOICE::PASS_UP:
		mOffset += size * mScope;
		ret = mReadFile->read(szBuf, mOffset, size);
		break;
	case CHOICE::PASS_DOWN:
		mOffset -= size * mScope;
		if (mOffset < 0)
			mOffset = 0;
		ret = mReadFile->read(szBuf, mOffset, size);
		break;
	case CHOICE::CUR:
		ret = mReadFile->read(szBuf, mOffset, size);
		break;
	case CHOICE::QUIT:
		changeWnd(_T("securecrt"));
		return;
	case CHOICE::SCOPE_UP:
		if (mScope < 10000)
			mScope *= 10;
		return;
	case CHOICE::SCOPE_DOWN:
		mScope /= 10;
		if (mScope == 0)
			mScope = 1;
		return;
	default:
		break;
	}
	setConfig(); 
	if (ret == -1)
	{
		LOG("ret == -1");
		return;
	}
	CString str(szBuf);
	LOG("offset is %d\n", mOffset);
	LOG("scope is %d\n", mScope);
//	log_buf(szBuf);
//	str.Format(_T("%s:%d:%d"), szBuf, mOffset, mScope);
	changeWnd(str.GetBuffer());
}

void SetTitle::readConfig()
{
	FILE *fp = fopen("D:\\cube.txt", "r");
	if (fp == NULL)
	{
		return ;
	}
	fscanf(fp, "offset=%d\nscope=%d", &mOffset, &mScope);
}
void SetTitle::setConfig()
{
	FILE *fp = fopen("D:\\cube.txt", "w");
	if (fp == NULL)
	{
		return ;
	}
	fprintf(fp, "offset=%d\nscope=%d", mOffset, mScope);
}