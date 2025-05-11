#pragma once
#include "afxdialogex.h"


// Диалоговое окно CustomPicture2DDlg
#include "Drawer2D.h"
#include "WMUSER.h"
class CustomPicture2DDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CustomPicture2DDlg)

public:
	CustomPicture2DDlg(CWnd* pParent = nullptr);   // стандартный конструктор
	CustomPicture2DDlg(const CString lab, int w, int h, CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~CustomPicture2DDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CUSTOM_PICTURE_2D };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	Drawer2D customPicture;
	virtual BOOL OnInitDialog();
	CString label = L"Custom picture";
	int width = 800;
	int height = 600;
	CWnd* parent{ nullptr };

	void MyShow();
};
