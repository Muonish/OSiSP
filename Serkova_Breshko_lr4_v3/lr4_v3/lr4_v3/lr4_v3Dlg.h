
// lr4_v3Dlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"
#include <vector>
#include <string>

class Clr4_v3DlgAutoProxy;


// диалоговое окно Clr4_v3Dlg
class Clr4_v3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(Clr4_v3Dlg);
	friend class Clr4_v3DlgAutoProxy;

// Создание
public:
	Clr4_v3Dlg(CWnd* pParent = NULL);	// стандартный конструктор
	virtual ~Clr4_v3Dlg();

// Данные диалогового окна
	enum { IDD = IDD_LR4_V3_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV

// Реализация
protected:
	Clr4_v3DlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;
	BOOL CanExit();
	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	void UniteResult(char **result, std::vector<char*> &resultsVector);
public:
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnChangeEdit6();
	CButton chkSurename;
	CButton chkAdress;
	CButton chkNumber;
	afx_msg void OnBnClickedButton1();
	CString surename;
	CString number;
	CString adress;
	CString resOfSearch;
};
