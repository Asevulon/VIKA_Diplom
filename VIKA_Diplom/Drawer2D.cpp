// Drawer2D.cpp: файл реализации
//

#include "pch.h"
#include "Drawer2D.h"
#include<fstream>
#include<math.h>
// Drawer2D

IMPLEMENT_DYNAMIC(Drawer2D, CStatic)

Drawer2D::Drawer2D()
{
	GdiplusStartupInput si;
	GdiplusStartup(&token, &si, NULL);

	//ifstream ifstr("wb.txt");
	ifstream ifstr("DivergentColorMap.txt");
	for (int i = 0; i < 11; i++)
	{
		double r, g, b;
		ifstr >> r >> g >> b;
		Colors.push_back(m_Color(r, g, b));
	}
	ifstr.close();
	logconstant = 1 / log(10);
}

Drawer2D::~Drawer2D()
{
	GdiplusShutdown(token);
}


BEGIN_MESSAGE_MAP(Drawer2D, CStatic)
END_MESSAGE_MAP()



// Обработчики сообщений Drawer2D



void Drawer2D::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_ShowPicture)
	{
		OnPicture(lpDrawItemStruct);
	}
	else
	{
		OnGraph(lpDrawItemStruct);
	}
}


void Drawer2D::SetData(std::vector<std::vector<double>>& in)
{
	if (in.empty())return;
	data = in;
	if (logarithmic)DataToLog();
	vector<double>Max;
	Max.resize(data.size());
	vector<double>Min = Max;
	for (int i = 0; i < data.size(); i++)
	{
		Max[i] = *max_element(data[i].begin(), data[i].end());
		Min[i] = *min_element(data[i].begin(), data[i].end());
	}
	zmax = *max_element(Max.begin(), Max.end());
	zmin = *min_element(Min.begin(), Min.end());
	if (zmax == zmin)
	{
		zmax++;
	}

	outzmax = zmax;
	outzmin = zmin;
	
	SetStretch(StretchFlag);

	Normalize();
	MakeColorData();
}

void Drawer2D::Normalize()
{
	double Norm = max(right, top) / (zmax - zmin);
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			data[i][j] -= zmin;
			data[i][j] *= Norm;
		}
	}
	zmax = max(right, top);;
	zmin = 0;
}

void Drawer2D::SetPadding(double lP, double rP, double tP, double bP)
{
	lPadding = lP;
	rPadding = rP;
	tPadding = tP;
	bPadding = bP;
}
void Drawer2D::SetMarksLen(int len)
{
	MarksLen = len;
}
void Drawer2D::SetTitle(CString str)
{
	title = str;
}

void Drawer2D::SetPicturePath(CString path)
{
	m_PicturePath = path;
}

void Drawer2D::ShowPicture(bool flag)
{
	m_ShowPicture = flag;
}

void Drawer2D::SetLogarithmic(bool flag)
{
	logarithmic = flag;
}

void Drawer2D::SetStretch(bool flag)
{
	StretchFlag = flag;
	if (data.empty())return;
	right = data[0].size() - 1;
	left = 0;
	top = data.size() - 1;
	bot = 0;
	if (!StretchFlag)
	{
		double lwidth = right - left;
		double lheight = top - bot;
		if (lwidth > lheight)
		{
			double mid = bot + lheight / 2.;
			bot = mid - lwidth / 2.;
			top = mid + lwidth / 2.;
		}
		else
		{
			double mid = left + lwidth / 2.;
			left = mid - lheight / 2.;
			right = mid + lheight / 2.;
		}
	}
}

void Drawer2D::SetGraphRange(double pleft, double pright, double pbot, double ptop)
{
	printingleft = pleft;
	printingright = pright;
	printingtop = ptop;
	printingbot = pbot;
}

double Drawer2D::CalcStringLen(HDC hDC, CString str)
{
	double numlen = 0;
	int size = str.GetLength() + 1;
	for (int i = 0; i < size; i++)
	{
		ABCFLOAT abc;
		GetCharABCWidthsFloatW(hDC, str[i], str[i], &abc);
		numlen += abc.abcfA + abc.abcfB + abc.abcfC;
	}
	return numlen;
}


inline m_Color Drawer2D::GetColor(double val)
{
	double step = (zmax - zmin) / 9;
	double start = zmin;
	int i = 0;
	if (val > zmax)val = zmax;
	if (step != 0)
	{
		double ii = (val - zmin) / step;
		i = ii;
		if (ii - i > 0)i++;
	}
	m_Color res;
	double k = (zmin + step * i - val) / step;
	res.r = (1 - k) * Colors[i + 1].r + k * Colors[i].r;
	res.g = (1 - k) * Colors[i + 1].g + k * Colors[i].g;
	res.b = (1 - k) * Colors[i + 1].b + k * Colors[i].b;
	return res;
}

inline Color Drawer2D::GetQuadreColor(int i, int j)
{
	m_Color res = Colordata[i][j];
	res = res.Avg(Colordata[i + 1][j], Colordata[i][j + 1], Colordata[i + 1][j + 1]);
	return res.AsColor();
}

inline void Drawer2D::MakeColorData()
{
	Colordata.resize(data.size());
	if (ToBlack)
	{
		double range = zmax - zmin;
		for (int i = 0; i < Colordata.size(); i++)
		{
			Colordata[i].resize(data[i].size());
			for (int j = 0; j < Colordata[i].size(); j++)
			{
				double z = data[i][j] - zmin;
				z /= range;
				z *= 255;
				Colordata[i][j] = m_Color(z, z, z);
			}
		}
	}
	else
	{
		for (int i = 0; i < Colordata.size(); i++)
		{
			Colordata[i].resize(data[i].size());
			for (int j = 0; j < Colordata[i].size(); j++)
			{
				Colordata[i][j] = GetColor(data[i][j]);
			}
		}
	}
	
}

void Drawer2D::OnPicture(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics ToWindow(lpDrawItemStruct->hDC);
	int width = lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
	int height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	Bitmap bmp(width, height);
	Graphics gr(&bmp);
	gr.Clear(Color::White);

	if (m_PicturePath.IsEmpty())
	{
		ToWindow.DrawImage(&bmp, 0, 0);
		return;
	}

	//gr.SetSmoothingMode(SmoothingModeAntiAlias);
	
	Image img(m_PicturePath);
	
	double xScale = (double)width / (double)img.GetWidth();
	double yScale = (double)height / (double)img.GetHeight();


	gr.ScaleTransform(xScale, yScale);
	gr.DrawImage(&img, 0, 0);

	ToWindow.DrawImage(&bmp, 0, 0);
}

void Drawer2D::OnGraph(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics ToWindow(lpDrawItemStruct->hDC);
	int width = lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
	int height = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	Bitmap bmp(width, height);
	Graphics gr(&bmp);
	gr.Clear(Color::White);

	if (data.empty())
	{
		ToWindow.DrawImage(&bmp, 0, 0);
		return;
	}

	gr.SetSmoothingMode(SmoothingModeAntiAlias);
	double actWidth(right - left);
	double actHeight(top - bot);
	double actLP = actWidth * lPadding / 100.;
	double actRP = actWidth * rPadding / 100.;
	double actTP = actHeight * tPadding / 100.;
	double actBP = actHeight * bPadding / 100.;

	double actTop = top + actTP;
	double actBot = bot - actBP;
	double actLeft = left - actLP;
	double actRight = right + actRP;

	actWidth = actRight - actLeft;
	actHeight = actTop - actBot;


	double xScale = width / actWidth;
	double yScale = -height / actHeight;

	Pen BackGroundPen(Color(200, 200, 200));

	Matrix matr;
	matr.Scale(xScale, yScale);
	matr.Translate(actLP - left, -actTop);


	float steplenY = (top - bot) / 10.;
	float steplenX = (right - left) / 10.;
	float printingsteplenY = (printingtop - printingbot) / 10.;
	float printingsteplenX = (printingright - printingleft) / 10.;

	FontFamily FF(L"Arial");
	Gdiplus::Font font(&FF, 12, FontStyle::FontStyleRegular, UnitPixel);
	SolidBrush brush(Color::Black);
	for (int i = 0; i < 11; i++)
	{
		CString str;
		str.Format(L"%.2f", printingtop - i * printingsteplenY);

		PointF strPoint;
		strPoint.X = left - actWidth / 100 - CalcStringLen(lpDrawItemStruct->hDC, str) / xScale;
		strPoint.Y = top - i * steplenY - 6 / yScale;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);

		str.Format(L"%.2f", printingright - i * printingsteplenX);

		strPoint.X = right - i * steplenX - CalcStringLen(lpDrawItemStruct->hDC, str) / xScale / 2.;
		strPoint.Y = bot + 6 / yScale;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);
	}

	Pen pen(Color::Black, 1.1F);
	PointF p1(left, top), p2(left, bot);
	matr.TransformPoints(&p1);
	matr.TransformPoints(&p2);
	gr.DrawLine(&pen, p1, p2);

	p1 = PointF(left, bot);
	p2 = PointF(right, bot);
	matr.TransformPoints(&p1);
	matr.TransformPoints(&p2);
	gr.DrawLine(&pen, p1, p2);


	if (!title.IsEmpty())
	{
		p1.X = actLeft + actWidth / 2 - CalcStringLen(lpDrawItemStruct->hDC, title) / 2 / xScale;
		p1.Y = (actTop + top) / 2 - 6 / yScale;
		matr.TransformPoints(&p1);
		gr.DrawString(title, title.GetLength() + 1, &font, p1, &brush);
	}


	PointF ps[5];
	SolidBrush databrush(Color::White);
	Pen datapen(Color::White);
	Bitmap map(Colordata[0].size(), Colordata.size());
	for (int i = 0; i < Colordata.size(); i++)
	{
		for (int j = 0; j < Colordata[i].size(); j++)
		{
			map.SetPixel(j, i, Colordata[i][j].AsColor());
		}
	}
	
	gr.SetTransform(&matr);
	RectF rect(0, 0, right - left, top - bot);
	gr.SetClip(rect);
	gr.DrawImage(&map, 0, 0);
	gr.ResetTransform();
	gr.ResetClip();

	/*for (int i = 0; i < Colordata.size(); i++)
	{
		for (int j = 0; j < Colordata[i].size(); j++)
		{
			ps[0].X = j;
			ps[0].Y = i;
			ps[1].X = j + 1;
			ps[1].Y = i + 1;
			matr.TransformPoints(ps, 2);
			databrush.SetColor(Colordata[i][j].AsColor());
			gr.FillRectangle(&databrush, ps[0].X, ps[1].Y, fabs(ps[1].X - ps[0].X), fabs(ps[1].Y - ps[0].Y));
		}
	}*/

	if (ToBlack)
	{
		double StepT = (outzmax - outzmin) / 10;
		PointF step;
		step.X = width * (100 - lPadding - rPadding) / 1100;
		step.Y = height * bPadding / 500.;
		ps[0].X = width * lPadding / 100. - step.X;
		ps[0].Y = height - 2 * step.Y;
		Pen blackpen(Color::Black);
		for (int i = 0; i < Colors.size(); i++)
		{
			double clr = 255 * i * StepT / (outzmax - outzmin);
			databrush.SetColor(Color(clr, clr, clr));
			ps[0].X += step.X;
			gr.FillRectangle(&databrush, ps[0].X, ps[0].Y, step.X, step.Y);
			gr.DrawRectangle(&blackpen, ps[0].X, ps[0].Y, step.X, step.Y);

			CString str;
			str.Format(L"%.1f", outzmin + i * StepT);
			double len = CalcStringLen(lpDrawItemStruct->hDC, str);
			PointF strp = ps[0];
			strp.X += (step.X - len) / 2;
			strp.Y += step.Y / 4;
			if (clr < 150)brush.SetColor(Color::White);
			else brush.SetColor(Color::Black);
			gr.DrawString(str, str.GetLength() + 1, &font, strp, &brush);
		}
	}
	else 
	{
		double StepT = (outzmax - outzmin) / 10;
		PointF step;
		step.X = width * (100 - lPadding - rPadding) / 1100;
		step.Y = height * bPadding / 500.;
		ps[0].X = width * lPadding / 100. - step.X;
		ps[0].Y = height - 2 * step.Y;
		Pen blackpen(Color::Black);
		for (int i = 0; i < Colors.size(); i++)
		{
			databrush.SetColor(Colors[i].AsColor());
			ps[0].X += step.X;
			gr.FillRectangle(&databrush, ps[0].X, ps[0].Y, step.X, step.Y);
			gr.DrawRectangle(&blackpen, ps[0].X, ps[0].Y, step.X, step.Y);

			CString str;
			str.Format(L"%.1f", outzmin + i * StepT);
			double len = CalcStringLen(lpDrawItemStruct->hDC, str);
			PointF strp = ps[0];
			strp.X += (step.X - len) / 2;
			strp.Y += step.Y / 4;
			gr.DrawString(str, str.GetLength() + 1, &font, strp, &brush);
		}
	}


	
	ToWindow.DrawImage(&bmp, 0, 0);
}

inline double Drawer2D::logTransform(double val)
{
	return sgn(val) * log10(1 + abs(val / logconstant));
}

void Drawer2D::DataToLog()
{
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			data[i][j] = logTransform(data[i][j]);
		}
	}
}


template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}