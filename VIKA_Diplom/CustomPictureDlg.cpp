// CustomPictureDlg.cpp: файл реализации
//

#include "pch.h"
#include "VIKA_Diplom.h"
#include "afxdialogex.h"
#include "CustomPictureDlg.h"


// Диалоговое окно CustomPictureDlg

IMPLEMENT_DYNAMIC(CustomPictureDlg, CDialogEx)

CustomPictureDlg::CustomPictureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_PICTURE, pParent)
{
	Create(IDD_DIALOG_CUSTOM_PICTURE);
	parent = pParent;
}

CustomPictureDlg::CustomPictureDlg(const CString& lab, const int w, const int h, CWnd* pParent)
	: CDialogEx(IDD_DIALOG_CUSTOM_PICTURE, pParent)
{
	Create(IDD_DIALOG_CUSTOM_PICTURE);
	label = lab;
	width = w;
	height = h;
	parent = pParent;
}

CustomPictureDlg::~CustomPictureDlg()
{
	if (parent != nullptr)parent->SendMessage(WM_DELETE_CHILD, CUSTOM_PICTURE_DLG, (LPARAM)this);
}

void CustomPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_PICTURE, customPicture);
}


BEGIN_MESSAGE_MAP(CustomPictureDlg, CDialogEx)
END_MESSAGE_MAP()


// Обработчики сообщений CustomPictureDlg

BOOL CustomPictureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Добавить дополнительную инициализацию
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// Исключение: страница свойств OCX должна возвращать значение FALSE
}

void CustomPictureDlg::MyShow()
{
	SetWindowTextW(label);
	customPicture.SetPadding(22, 5, 10, 15);
	customPicture.SetTitle(label);
	ShowWindow(SW_SHOW);
}

