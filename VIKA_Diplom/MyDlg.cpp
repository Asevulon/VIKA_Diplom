
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
	g_pParent = this;

	auto& filter = Filters::haar;
	int decLevel =1;
	GDIINIT();
	CString path = L"смешарики1.jpg";
	auto pic = DispatchPicture(path);
	ShowImage(L"Исходное", pic);
	GDIDEINIT();

	auto pattern = DispatchPicture(L"совунья.jpg");
	ShowImage(L"pattern", pattern);

	auto noisedPic = pic;
	Noise(noisedPic,0.25);
	double ediff = Ediff(pic, noisedPic);
	path.Format(L"noisedPic, Ediff(pic, noisedPic) = %.3f", ediff);
	ShowImage(path, noisedPic);

	DWT dwt;
	//auto matches = FindObject(pic, pattern, dwt, filter, 1, 0.75);
	auto matches = FindObjectAdaptive(noisedPic, pattern, dwt, filter, 1, 0.445, 2, 1);
	path.Format(L"Found patterns: %d", matches.size());
	ShowImage(path, noisedPic, matches);
	//auto noisedPic = pic;
	//Noise(noisedPic, 0.2);
	//double ediff = Ediff(pic, noisedPic);
	//path.Format(L"noisedPic, Ediff(pic, noisedPic) = %.3f", ediff);

	//auto win = new CustomPicture2DDlg(path, 1000, 800, this);
	//win->customPicture.SetGraphRange(0, noisedPic[0].size(), 0, noisedPic.size());
	//win->customPicture.SetData(noisedPic);
	//win->MyShow();

	//auto noiseddwt = noisedPic;
	//DWT dwt;
	//dwt.dwt2d(noiseddwt, filter, decLevel);
	//std::vector<std::vector<double>> LL, LH, HL, HH;
	//splitSubbands(noiseddwt, LL, LH, HL, HH);

	//win = new CustomPicture2DDlg(L"LL", 1000, 800, this);
	//win->customPicture.SetGraphRange(0, LL[0].size(), 0, LL.size());
	//win->customPicture.SetData(LL);
	//win->customPicture.SetLogarithmic(true);
	//win->MyShow();

	//win = new CustomPicture2DDlg(L"LH", 1000, 800, this);
	//win->customPicture.SetGraphRange(0, LH[0].size(), 0, LH.size());
	//win->customPicture.SetData(LH);
	//win->customPicture.SetLogarithmic(true);
	//win->MyShow();

	//win = new CustomPicture2DDlg(L"HL", 1000, 800, this);
	//win->customPicture.SetGraphRange(0, HL[0].size(), 0, HL.size());
	//win->customPicture.SetData(HL);
	//win->customPicture.SetLogarithmic(true);
	//win->MyShow();

	//win = new CustomPicture2DDlg(L"HH", 1000, 800, this);
	//win->customPicture.SetGraphRange(0, HH[0].size(), 0, HH.size());
	//win->customPicture.SetData(HH);
	//win->customPicture.SetLogarithmic(true);
	//win->MyShow();

	//double sigma = EstimateSigma(HH);
	//int N = noisedPic.size() * noisedPic[0].size();
	//double threshold = sigma * sqrt(2.0 * log(N));

	//auto rec = noisedPic;
	//DenoiseImage(rec, filter, decLevel, sigma);
	////DenoiseImageByAvg(rec, filter, decLevel);
	//ediff = Ediff(pic, rec);
	//path.Format(L"rec, Ediff(pic, rec) = %.3f, sigma = %.3f", ediff, sigma);

	//win = new CustomPicture2DDlg(path, 1000, 800, this);
	//win->customPicture.SetGraphRange(0, rec[0].size(), 0, rec.size());
	//win->customPicture.SetData(rec);
	//win->MyShow();
}

void MyDlg::ShowImage(CString label, vector<vector<double>>& pic)
{
	auto win = new CustomPicture2DDlg(label, 1000, 800, this);
	win->customPicture.SetGraphRange(0, pic[0].size(), 0, pic.size());
	win->customPicture.SetData(pic);
	win->MyShow();
}

void MyDlg::ShowImage(CString label, vector<vector<double>>& pic, vector<pair<RECT, double>>&matches)
{
	auto win = new CustomPicture2DDlg(label, 1000, 800, this);
	win->customPicture.matches = matches;
	win->customPicture.SetGraphRange(0, pic[0].size(), 0, pic.size());
	win->customPicture.SetData(pic);
	win->MyShow();
}