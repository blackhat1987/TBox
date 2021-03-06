#include "StdAfx.h"
#include "TBoxViewNotify.h"
#include "Resource.h"
#include "TBoxDlgController.h"


TBoxViewNotify::TBoxViewNotify(void)
{
	InitializeCriticalSection(&m_plugsNotifyMapSection);
}

TBoxViewNotify::~TBoxViewNotify(void)
{
	::Shell_NotifyIcon(NIM_DELETE,&m_notify);
	DeleteCriticalSection(&m_plugsNotifyMapSection);
}


void TBoxViewNotify::Init(CDialog &dlg)
{
	m_notify.cbSize=sizeof NOTIFYICONDATA;  
	m_notify.hWnd=dlg.m_hWnd;    
	m_notify.uID=IDR_MAINFRAME;  
	m_notify.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));  
	_tcscpy_s(m_notify.szTip,_T("TBOX 软件"));  
	m_notify.uCallbackMessage=WM_USER_NOTIFYICON;  
	m_notify.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP|NIF_INFO;  //OK,下面就是托盘产生了.
	//_tcscpy_s(m_notify.szInfo,_T("XX"));
	//_tcscpy_s(m_notify.szInfoTitle,_T("XXXX"));
	//m_notify.dwInfoFlags=NIIF_INFO;  
	//m_notify.uTimeout=2000;  
	Shell_NotifyIcon(NIM_ADD,&m_notify);   
}

LRESULT TBoxViewNotify::NotifyProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (NULL == m_pdlg)
	{
		return NULL;
	}
	switch(message)
	{
	case WM_USER_NOTIFYICON:
		{
			if(lParam==WM_RBUTTONUP)
			{ //鼠标右键单击弹出选单 
				CreateNotifyMenu();
			} 
		}
		break;
	case WM_COMMAND:
		{
			//MessageBox(NULL,_T("231"),NULL,NULL);
			UINT uMsg=LOWORD(wParam);
			OnNotifyMenuClick(uMsg);
		}
		break;
	default:
		break;
	}
	return NULL;
}

void TBoxViewNotify::CreateNotifyMenu()
{
	CPoint pos; 
	GetCursorPos(&pos); 
	CMenu pPopMenu;
	pPopMenu.CreatePopupMenu();

	CMenu plugmenu;
	plugmenu.CreatePopupMenu();

	EnterCriticalSection(&m_plugsNotifyMapSection);
	for(NotifyMap::iterator it = m_plugsNotifyMap.begin();it != m_plugsNotifyMap.end();it++)
	{
		plugmenu.AppendMenu(MF_POPUP,it->second.menuid,it->second.name);
	}
	LeaveCriticalSection(&m_plugsNotifyMapSection);

	pPopMenu.AppendMenu(MF_POPUP,(UINT_PTR)plugmenu.Detach(),_T("功能列表"));
	pPopMenu.AppendMenu(MF_POPUP,WM_USER_MENUCLICK_SET,_T("设置"));
	pPopMenu.AppendMenu(MF_POPUP,WM_USER_MENUCLICK_ABOUT,_T("关于"));
	pPopMenu.AppendMenu(MF_POPUP,WM_USER_MENUCLICK_EXIT,_T("退出"));
	AfxGetMainWnd()->SetForegroundWindow(); //加上这一句就可以了
	pPopMenu.TrackPopupMenu(TPM_LEFTALIGN,pos.x,pos.y,AfxGetMainWnd());
	pPopMenu.DestroyMenu();
}

void TBoxViewNotify::OnNotifyMenuClick(UINT menuid)
{
	if(WM_USER_MENUCLICK_SET == menuid)
	{
		MessageBox(NULL,_T("设置"),NULL,NULL);
	}
	else if (WM_USER_MENUCLICK_ABOUT == menuid)
	{
		MessageBox(NULL,_T("关于"),NULL,NULL);
	}
	else if (WM_USER_MENUCLICK_EXIT == menuid)
	{
		TBoxDlgController::GetInstance().Destory();
		::Shell_NotifyIcon(NIM_DELETE,&m_notify);
		exit(0);
	}
	else
	{
		EnterCriticalSection(&m_plugsNotifyMapSection);
		if (m_plugsNotifyMap.find(menuid) == m_plugsNotifyMap.end())
		{
			LeaveCriticalSection(&m_plugsNotifyMapSection);
		}
		else
		{
			NotifyInfo info = m_plugsNotifyMap[menuid];
			LeaveCriticalSection(&m_plugsNotifyMapSection);
			if (info.clickEvent != NULL)
			{
				info.clickEvent(menuid);
			}
		}

	}
}

UINT TBoxViewNotify::AddPlugsMenu(UINT menuid,TCHAR *menuname,ContexMenuEvent OnClickEvent)
{
	EnterCriticalSection(&m_plugsNotifyMapSection);
	if (m_plugsNotifyMap.find(menuid) != m_plugsNotifyMap.end() || menuid < WM_USER + 200)
	{
		LeaveCriticalSection(&m_plugsNotifyMapSection);
		return ERROR_FILE_EXISTS;
	}
	if (_tcslen(menuname) > 100)
	{
		LeaveCriticalSection(&m_plugsNotifyMapSection);
		return ERROR_PORT_MESSAGE_TOO_LONG;
	}
	NotifyInfo info;
	info.menuid = menuid;
	_tcscpy_s(info.name,menuname);
	info.clickEvent = OnClickEvent;
	m_plugsNotifyMap[menuid] = info;
	LeaveCriticalSection(&m_plugsNotifyMapSection);
	return ERROR_SUCCESS;
}