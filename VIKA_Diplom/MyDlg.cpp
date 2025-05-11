
// MyDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "VIKA_Diplom.h"
#include "MyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно MyDlg



MyDlg::MyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIKA_DIPLOM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_DELETE_CHILD, &MyDlg::OnDeleteChild)
	ON_BN_CLICKED(IDOK, &MyDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Обработчики сообщений MyDlg

BOOL MyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void MyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void MyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR MyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


afx_msg LRESULT MyDlg::OnDeleteChild(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CUSTOM_PICTURE_DLG:
	{
		CustomPictureDlg* p = (CustomPictureDlg*)(lParam);
		delete p;
		break;
	}
	case CUSTOM_PICTURE_2D_DLG:
	{
		CustomPicture2DDlg* p1 = (CustomPicture2DDlg*)(lParam);
		delete p1;
		break;
	}
	default:
		break;
	}
	return 0;
}

void MyDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	/*GDIINIT();
	CString path = L"testimg.bmp";
	auto pic = DispatchPicture(path);
	auto win = new CustomPicture2DDlg(path, 1000, 800, this);
	win->customPicture.SetGraphRange(0, pic[0].size(), 0, pic.size());
	win->customPicture.SetData(pic);
	win->MyShow();
	GDIDEINIT();

	DWT dwt;
	vector<vector<double>>a;
	vector<vector<double>>d;
	vector<double>rec;
	dwt.Transform(Filters::db4, pic.front(), 1, a, d);
	dwt.Recover(Filters::db4, a, d, 1, rec);

	auto win1 = new CustomPictureDlg(L"a", 1000, 800, this);
	win1->customPicture.SetData(a.front());
	win1->MyShow();

	win1 = new CustomPictureDlg(L"d", 1000, 800, this);
	win1->customPicture.SetData(d.front());
	win1->MyShow();

	win1 = new CustomPictureDlg(L"source", 1000, 800, this);
	win1->customPicture.SetData(pic.front());
	win1->MyShow();

	win1 = new CustomPictureDlg(L"rec", 1000, 800, this);
	win1->customPicture.SetData(rec);
	win1->MyShow();*/
	double dt = 0.01;
	size_t size = 100;
	vector<double> source(size);
	for (int i = 0; i < size; ++i)
	{
		source[i] = sin(6.28 * 1 * i * dt);
	}

	DWT dwt;
	vector<DWTLevel> ad;
	vector<double>rec;
	dwt.Transform(Filters::db4, source, 1, ad);
	dwt.Recover(Filters::db4, ad, rec);

	auto win1 = new CustomPictureDlg(L"a", 1000, 800, this);
	win1->customPicture.SetData(ad[0].a);
	win1->MyShow();

	win1 = new CustomPictureDlg(L"d", 1000, 800, this);
	win1->customPicture.SetData(ad[0].d);
	win1->MyShow();

	win1 = new CustomPictureDlg(L"source", 1000, 800, this);
	win1->customPicture.SetData(source);
	win1->MyShow();

	win1 = new CustomPictureDlg(L"rec", 1000, 800, this);
	win1->customPicture.SetData(rec);
	win1->MyShow();
}
