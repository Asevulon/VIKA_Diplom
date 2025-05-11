// CustomPicture2DDlg.cpp: файл реализации
//

#include "pch.h"
#include "VIKA_Diplom.h"
#include "afxdialogex.h"
#include "CustomPicture2DDlg.h"


// Диалоговое окно CustomPicture2DDlg

IMPLEMENT_DYNAMIC(CustomPicture2DDlg, CDialogEx)

CustomPicture2DDlg::CustomPicture2DDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOM_PICTURE_2D, pParent)
{
	Create(IDD_DIALOG_CUSTOM_PICTURE_2D);
	parent = pParent;
}

CustomPicture2DDlg::CustomPicture2DDlg(const CString lab, int w, int h, CWnd* pParent)
{
	Create(IDD_DIALOG_CUSTOM_PICTURE_2D);
	label = lab;
	width = w;
	height = h;
	parent = pParent;
}

CustomPicture2DDlg::~CustomPicture2DDlg()
{
	if (parent != nullptr)SendMessage(WM_DELETE_CHILD, CUSTOM_PICTURE_2D_DLG, (LPARAM)this);
}

void CustomPicture2DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_PICTURE_2D, customPicture);
}


BEGIN_MESSAGE_MAP(CustomPicture2DDlg, CDialogEx)
END_MESSAGE_MAP()


// Обработчики сообщений CustomPicture2DDlg

BOOL CustomPicture2DDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE;
}

void CustomPicture2DDlg::MyShow()
{
    // Желаемая клиентская область
    CSize desiredClientSize(width, height);

    // Расчёт полного размера окна
    CRect windowRect(CPoint(0, 0), desiredClientSize);
    DWORD style = GetStyle();
    ::AdjustWindowRect(&windowRect, style, FALSE);

    // Получаем текущую позицию окна
    CRect currentWindowRect;
    GetWindowRect(&currentWindowRect);

    // Устанавливаем новый размер окна
    SetWindowPos(nullptr,
        currentWindowRect.left,
        currentWindowRect.top,
        windowRect.Width(),
        windowRect.Height(),
        SWP_NOZORDER | SWP_NOACTIVATE);

    // Обновляем контрол
    CRect clientRect;
    GetClientRect(&clientRect);
    customPicture.SetWindowPos(nullptr, 0, 0,
        clientRect.Width(),
        clientRect.Height(),
        SWP_NOZORDER | SWP_NOACTIVATE);

    // Принудительно обновляем окно

    customPicture.SetTitle(label);
    SetWindowTextW(label);
    UpdateWindow();
    ShowWindow(SW_SHOW);
}

//ShowWindow(SW_SHOW);
