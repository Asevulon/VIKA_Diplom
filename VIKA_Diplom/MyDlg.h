
// MyDlg.h: файл заголовка
//

#pragma once
#include"CustomPictureDlg.h"
#include"CustomPicture2DDlg.h"
#include"DWT.h"
// Диалоговое окно MyDlg
class MyDlg : public CDialogEx
{
// Создание
public:
	MyDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIKA_DIPLOM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDeleteChild(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk();

	void ShowImage(CString label, vector<vector<double>>& pic);
	void ShowImage(CString label, vector<vector<double>>& pic, vector<pair<RECT, double>>& matches);
};

