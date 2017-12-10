// mfcHook.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mfcHook.h"
#include "setTitle.h"
#include "log.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma data_seg("SHARED")
static HHOOK hhk = NULL;
static HHOOK hKeyHook = NULL;
static HINSTANCE hinst = NULL;
#pragma data_seg()

#pragma comment(linker, "/section:SHARED,rws")


CString temp;
bool bHook = false;
bool m_bInjected = false;
BYTE OldCode[5];
BYTE NewCode[5];
typedef int (WINAPI *AddProc)(int a, int b);
AddProc add;
HANDLE hProcess = NULL;
FARPROC pfadd;
DWORD dwPid;
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CmfcHookApp

BEGIN_MESSAGE_MAP(CmfcHookApp, CWinApp)
END_MESSAGE_MAP()
//hook mouse start

void Inject();
void HookOn();
void HookOff();
int WINAPI Myadd(int a, int b);
typedef int (* MsgBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MsgBoxA OldMsgBoxA = NULL;
BYTE *pfApiFunc = NULL;
int WINAPI MyMsgBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
bool judgeHook(TCHAR *szPath);

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return  CallNextHookEx(hhk, nCode, wParam, lParam);
}
LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = CallNextHookEx(hKeyHook, nCode, wParam, lParam);
	if (nCode == HC_ACTION)
	{		
		if(lParam &0x80000000)
		{
			switch(wParam)
			{
			case 'J':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::PREV);
				break;
			case 'K':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::NEXT);
				break;
			case 'H':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::MINUS);
				break;
			case 'Q':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::QUIT);
				break;
			case 'W':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::SCOPE_UP);
				break;
			case 'S':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::SCOPE_DOWN);
				break;
			case 'U':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::PASS_UP);
				break;
			case 'I':
				SetTitle::getInstance()->changeWnd(SetTitle::CHOICE::PASS_DOWN);
				break;
			default:
				break;
			}
		}
	}
	return Result;
}
BOOL InstallHook()
{
	hhk = ::SetWindowsHookEx(WH_MOUSE, MouseProc, hinst, 0);
	hKeyHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyProc, hinst, 0);
	return true;
}
void UninstallHook()
{
	SetTitle::getInstance()->changeWnd(_T("Serial-COM2 - SecureCRT"));
	if (hhk != NULL)
	{
		::UnhookWindowsHookEx(hhk);
	}
	if (hKeyHook != NULL)
	{
		::UnhookWindowsHookEx(hKeyHook);
	}
	HookOff();
}
static HWND targHwnd = NULL;
LRESULT CALLBACK editHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		if (targHwnd != NULL)
		{
			char szText[130];
			SendMessage(targHwnd, WM_GETTEXT, 128, (LPARAM)szText);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
void createEdit(HINSTANCE hinst, HWND hParent)
{
	WNDCLASSEX wc;
	::ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.hInstance = hinst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = _T("filterWindow");
	wc.lpfnWndProc = editHandle;
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(_T("filterWindow"), NULL, WS_CHILDWINDOW |WS_VISIBLE, 100, 0, 400, 400, hParent, NULL, hinst, NULL);
}
BOOL CmfcHookApp::InitInstance()
{
	CWinApp::InitInstance();
	hinst = ::AfxGetInstanceHandle();
	DWORD dwPid = ::GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
//	Inject();
	TCHAR szFileFullPath[256];
	::GetModuleFileName(NULL, szFileFullPath, 256);
	
	if (judgeHook(szFileFullPath))
	{
		HWND parentHwnd = NULL;
		TCHAR text[256];
		parentHwnd = ::FindWindow(_T("LyncConversationWindowClass"), NULL);
		if (parentHwnd != NULL)
		{
			SetTitle::getInstance()->setWnd(parentHwnd);
			SetTitle::getInstance()->changeWnd(L"hello");

			HWND childHwnd = ::GetWindow(parentHwnd, GW_CHILD);
			childHwnd = ::GetWindow(childHwnd, GW_HWNDFIRST);
			AfxMessageBox(_T("find lync"));
			while(childHwnd)
			{
				::GetClassName(childHwnd, text, 256);
				if (_tcscmp(text, _T("AfxFrameOrView90u")) == 0)
				{
					AfxMessageBox(_T("find afx"));
					MessageBoxA(childHwnd, "hello", "hello", MB_OK);
					targHwnd = childHwnd;
					createEdit(hinst, childHwnd);
				}
				childHwnd = ::GetWindow(childHwnd, GW_HWNDNEXT);
			}
		}
		else
		{
			AfxMessageBox(L"not find");
		}
		HookOn();
	}
	return TRUE;
}
int CmfcHookApp::ExitInstance()
{
	HookOff();
	return CWinApp::ExitInstance();
}


void Inject()
{
	if (m_bInjected == false)
	{
		m_bInjected = true;
		TCHAR szFileFullPath[256];
		::GetModuleFileName(NULL, szFileFullPath, 256);
		AfxMessageBox(szFileFullPath);
		HMODULE hmod = ::LoadLibrary(_T("Add.dll"));
		if (hmod == NULL)
		{
			AfxMessageBox(L"cannot find Add.dll!");
			return ;
		}
		add = (AddProc)::GetProcAddress(hmod, "add");
		pfadd = (FARPROC)add;
		if (pfadd == NULL)
		{
			AfxMessageBox(L"cannot locate add()");
			return ;
		}
		_asm
		{
			lea edi,OldCode
				mov esi,pfadd
				cld//set df = 0 addr add
				movsd//esi -> edi
				movsb
		}
		NewCode[0] = 0xe9;
		_asm
		{
			lea eax,Myadd
				mov ebx,pfadd
				sub eax,ebx//eax = eax - ebx
				sub eax,5//eax = eax - 5
				mov dword ptr[NewCode+1],eax
		}
		HookOn();
	}
}


void HookOnNotUse()
{
	ASSERT(hProcess != NULL);

	DWORD dwTemp = 0;
	DWORD dwOldProtect;
	VirtualProtectEx(hProcess, pfadd, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfadd, NewCode, 5, 0);
	VirtualProtectEx(hProcess, pfadd, 5, dwOldProtect, &dwTemp);
	bHook = true;
}

void HookOffNotUse()
{
	ASSERT(hProcess != NULL);
	DWORD dwTemp = 0;
	DWORD dwOldProtect;
	VirtualProtectEx(hProcess, pfadd, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfadd, OldCode, 5, 0);
	VirtualProtectEx(hProcess, pfadd, 5, dwOldProtect, &dwTemp);
	bHook = false;
}


int WINAPI Myadd(int a, int b)
{
	a = a + 1;
	b += 1;
	HookOff();
	int ret;
	ret = add(a,b);
	HookOn();
	return ret;
}
//hook mouse end

//hook msgbox start
bool judgeHook(TCHAR *szPath)
{
	TCHAR *tempPath;
	while (*szPath != '\0')
	{
		if (*szPath == '\\')
		{
			tempPath = szPath + 1;
		}
		szPath++;
	}
	if (_tcscmp(tempPath, _T("testHook.exe")) == 0)
	{
		AfxMessageBox(tempPath);
		return true;
	}
	return false;
}
void HookOn()
{
	ASSERT(hProcess != NULL);
	HINSTANCE hInst = LoadLibrary(_T("User32.dll"));
	OldMsgBoxA = (MsgBoxA)::GetProcAddress(hInst, "MessageBoxA");
	if (OldMsgBoxA == NULL)
	{
		AfxMessageBox(_T("get msgboxA addr failed"));
		return ;
	}
	pfApiFunc = (BYTE *)OldMsgBoxA - 5;
/*
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());
	if (hProcess == NULL)
	{
		return;
	}
*/
	DWORD dwOldProtect = 0;
	VirtualProtectEx(hProcess, pfApiFunc, 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	pfApiFunc[0] = 0xE9;
	(int &)pfApiFunc[1] = (int)MyMsgBoxA - (int)OldMsgBoxA;
	pfApiFunc[5] = 0xEB;
	pfApiFunc[6] = 0xF9;
	VirtualProtectEx(hProcess, pfApiFunc, 7, dwOldProtect, NULL);
}

void HookOff()
{
	ASSERT(hProcess != NULL);
	if (pfApiFunc == NULL)
	{
		return;
	}
	DWORD dwOldProtect = 0;
	VirtualProtectEx(hProcess, pfApiFunc, 7, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memset(pfApiFunc, 0x90, 5);
	pfApiFunc[5] = 0x8B;
	pfApiFunc[6] = 0xFF;
	VirtualProtectEx(hProcess, pfApiFunc, 7, dwOldProtect, NULL);

}

int WINAPI MyMsgBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int nRet = 0;
	HookOff();
	nRet = ::MessageBoxA(NULL, "be hacked", "wow", MB_OK);
	HookOn();
	return nRet;
}
//hook msgbox end


// CmfcHookApp construction

CmfcHookApp::CmfcHookApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CmfcHookApp object

CmfcHookApp theApp;


// CmfcHookApp initialization
