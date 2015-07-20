// TBoxDlg.h : ͷ�ļ�
//

#pragma once
#include "TboxContextMain.h"
#include "TBoxViewNotify.h"
#include "Resource.h"


// CTBoxDlg �Ի���
class CTBoxDlg : public CDialog
{
// ����
public:
	CTBoxDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TBOX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	TBoxViewNotify m_viewNotify;
};
