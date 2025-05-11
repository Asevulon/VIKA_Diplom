#pragma once
#include "afxdialogex.h"


// Диалоговое окно CustomPictureDlg
#include "Drawer.h"
#include "WMUSER.h"
class CustomPictureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CustomPictureDlg)

public:
	CustomPictureDlg(CWnd* pParent = nullptr);   // стандартный конструктор
	CustomPictureDlg(const CString& lab, const int w, const int h, CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~CustomPictureDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CUSTOM_PICTURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	Drawer customPicture;
	virtual BOOL OnInitDialog();

	CString label = L"Custom picture";
	int width = 800;
	int height = 600;
	CWnd* parent{ nullptr };
	void MyShow();
};
