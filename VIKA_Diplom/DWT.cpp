#include "pch.h"
#include "DWT.h"
using namespace std;

Interpolation::Interpolation(vector<double>& source)
{
	data = source;
}

size_t Interpolation::Size()
{
	return data.size();
}

SymmetricInterpolation::SymmetricInterpolation(vector<double>& source)
	:Interpolation(source)
{
}

int SymmetricInterpolation::get_index(int index) const {
	int N = data.size();
	if (index >= 0 && index < N)
		return index;

	// Зеркальное отражение относительно границ
	index = std::abs(index); // Отрицательные индексы становятся положительными
	index %= (2 * N - 2);    // Циклическое зеркалирование

	if (index >= N) {
		index = 2 * N - index - 2; // Корректное зеркалирование
	}

	return index;
}

double SymmetricInterpolation::operator[](int index) const {
	return data[get_index(index)];
}

vector<double>DWT::Downscale(vector<double>& source)
{
	vector<double>res(source.size() / 2);
	for (int i = 0; i < res.size(); ++i)res[i] = source[i << 1];
	return res;
}

vector<double> DWT::UpScale(vector<double>& source)
{
	vector<double>res;
	res.reserve(source.size() * 2);
	for (auto& item : source)
	{
		res.push_back(item);
		res.push_back(0);
	}
	return res;
}

SymmetricInterpolation DWT::ProduceH1(SymmetricInterpolation& h0)
{
	size_t hSize = h0.Size();
	vector<double>h1(hSize);
	for (int i = 0; i < hSize; ++i)
	{
		int sign = (i % 2 == 0) ? 1 : -1; // (-1)^i
		h1[i] = sign * h0[hSize - 1 - i];
	}
	return SymmetricInterpolation(h1);
}

SymmetricInterpolation DWT::ProduceG(SymmetricInterpolation& h)
{
	size_t hSize = h.Size();
	vector<double>res(hSize);
	for (int i = 0; i < hSize; ++i)res[i] = h[hSize - i - 1];
	return SymmetricInterpolation(res);
}

void DWT::Decompose(SymmetricInterpolation& source, vector<double>& a, vector<double>& d, SymmetricInterpolation& h0, SymmetricInterpolation h1)
{
	size_t sSize = source.Size();
	vector<double>buffer(sSize);

	Convolution(source, h0, buffer);
	a = Downscale(buffer);

	Convolution(source, h1, buffer);
	d = Downscale(buffer);
}

void DWT::Recombine(vector<double>& a, vector<double>& d, SymmetricInterpolation& g0, SymmetricInterpolation g1, vector<double>& out)
{
	double summ = 0;
	SymmetricInterpolation asi(UpScale(a));
	SymmetricInterpolation dsi(UpScale(d));
	vector<double>abuf;
	vector<double>dbuf;
	Convolution(asi, g0, abuf);
	Convolution(dsi, g1, dbuf);
	out.resize(abuf.size());
	for (int i = 0; i < abuf.size(); ++i)out[i] = abuf[i] + dbuf[i];
}

void DWT::Convolution(SymmetricInterpolation& f, SymmetricInterpolation& g, vector<double>& out)
{
	size_t fSize = f.Size();
	size_t gSize = g.Size();
	size_t outSize = fSize; // Теперь хотим, чтобы результат был такой же длины
	out.resize(outSize);

	int halfFilter = gSize / 2;

	for (size_t i = 0; i < outSize; ++i)
	{
		double sum = 0;
		for (size_t j = 0; j < gSize; ++j)
		{
			int index = static_cast<int>(i) + static_cast<int>(j) - static_cast<int>(halfFilter);
			sum += f[index] * g[j];
		}
		out[i] = sum;
	}
}

void DWT::Transform(std::vector<double>& filter, vector<double>& source, uint64_t decompositionLevel, vector<DWTLevel>& out)
{
	if (decompositionLevel == 0)return;
	SymmetricInterpolation s(source);
	SymmetricInterpolation h0(filter);
	SymmetricInterpolation h1 = ProduceH1(h0);

	out.resize(decompositionLevel);

	Decompose(s, out[0].a, out[0].d, h0, h1);

	for (int i = 1; i < decompositionLevel; ++i)
	{
		Decompose(SymmetricInterpolation(out[i - 1].a), out[i].a, out[i].d, h0, h1);
	}
}

void DWT::Recover(std::vector<double>& filter, vector<DWTLevel>& in, vector<double>& out)
{
	SymmetricInterpolation h0(filter);
	SymmetricInterpolation h1 = ProduceH1(h0);
	SymmetricInterpolation g0 = ProduceG(h0);
	SymmetricInterpolation g1 = ProduceG(h1);

	size_t inSize = in.size();

	for (int i = 0; i < inSize - 1; ++i)
	{
		auto& cur = in[inSize - 1 - i];
		auto& next = in[inSize - 2 - i];
		Recombine(cur.a, cur.d, g0, g1, next.a);
	}

	Recombine(in[0].a, in[0].d, g0, g1, out);
}

uint32_t UpScale(uint32_t n) {
	if (n == 0) return 1; // специальный случай
	if (n == 1) return 1;

	uint32_t result = 1;
	while (result < n) {
		result <<= 1;
	}
	return result;
}

double ToBlack(Gdiplus::Color& c)
{
	return 0.299 * c.GetRed() + 0.587 * c.GetGreen() + 0.114 * c.GetBlue();
}


ULONG_PTR token;
vector<vector<double>> DispatchPicture(CString path)
{
	Gdiplus::Image img(path);
	int w = img.GetWidth();
	int h = img.GetHeight();
	int ws = UpScale(w);
	int hs = UpScale(h);
	float xs = float(ws) / float(w);
	float ys = float(hs) / float(h);
	Gdiplus::Bitmap bmp(ws, hs);
	Gdiplus::Graphics g(&bmp);
	g.ScaleTransform(xs, ys);
	g.DrawImage(&img, 0, 0);

	vector<vector<double>>res(ws, vector<double>(hs, 0));
	for (int i = 0; i < ws; ++i)
	{
		for (int j = 0; j < hs; ++j)
		{
			Gdiplus::Color c;
			bmp.GetPixel(i, j, &c);
			res[i][j] = ToBlack(c);
		}
	}
	return res;
}

void GDIINIT()
{
	Gdiplus::GdiplusStartupInput si;
	Gdiplus::GdiplusStartup(&token, &si, NULL);
}

void GDIDEINIT()
{
	Gdiplus::GdiplusShutdown(token);
}
