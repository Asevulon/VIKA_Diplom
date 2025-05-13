#include "pch.h"
#include "DWT.h"
#include <algorithm>
#include <stdexcept>
#include<limits>
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
	int ws = (w % 2)? w + 1: w;// UpScale(w);
	int hs = (h % 2)? h + 1: h;// UpScale(h);
	float xs = float(ws) / float(w);
	float ys = float(hs) / float(h);
	Gdiplus::Bitmap bmp(ws, hs);
	Gdiplus::Graphics g(&bmp);
	g.ScaleTransform(xs, ys);
	g.DrawImage(&img, 0, 0);

	vector<vector<double>>res(hs, vector<double>(ws, 0));
	for (int i = 0; i < hs; ++i)
	{
		for (int j = 0; j < ws; ++j)
		{
			Gdiplus::Color c;
			bmp.GetPixel(j, hs - 1 - i, &c);
			res[i][j] = ToBlack(c) / 255.;
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

void transposeMatrix(std::vector<std::vector<double>>& matrix) {
	if (matrix.empty()) return;
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	std::vector<std::vector<double>> transposed(cols, std::vector<double>(rows));
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			transposed[j][i] = matrix[i][j];
		}
	}
	matrix = transposed;
}

void splitSubbands(const std::vector<std::vector<double>>& coeffs,
	std::vector<std::vector<double>>& LL,
	std::vector<std::vector<double>>& LH,
	std::vector<std::vector<double>>& HL,
	std::vector<std::vector<double>>& HH) {
	int halfRows = coeffs.size() / 2;
	int halfCols = coeffs[0].size() / 2;
	LL.resize(halfRows, std::vector<double>(halfCols));
	LH.resize(halfRows, std::vector<double>(halfCols));
	HL.resize(halfRows, std::vector<double>(halfCols));
	HH.resize(halfRows, std::vector<double>(halfCols));

	for (int i = 0; i < halfRows; ++i) {
		for (int j = 0; j < halfCols; ++j) {
			LL[i][j] = coeffs[i][j];                     // LL
			LH[i][j] = coeffs[i + halfRows][j];          // LH
			HL[i][j] = coeffs[i][j + halfCols];          // HL
			HH[i][j] = coeffs[i + halfRows][j + halfCols]; // HH
		}
	}
}

void Noise(vector<vector<double>>& target, double NoiseLevel)
{
	srand(time(NULL));
	const double NoiseNum = 12;
	auto noise = target;

	auto  three_point_derivative = [](const std::vector<double>& data, double h) {
		std::vector<double> derivative;
		size_t n = data.size();
		derivative.resize(n);
		derivative[0] = (-3.0 * data[0] + 4.0 * data[1] - data[2]) / (2.0 * h);
		derivative[n - 1] = (3.0 * data[n - 1] - 4.0 * data[n - 2] + data[n - 3]) / (2.0 * h);
		for (size_t i = 1; i < n - 1; ++i) {
			derivative[i] = (data[i + 1] - data[i - 1]) / (2.0 * h);
		}
		return derivative;
		};

	for (auto& row : noise)
	{

		for (auto& item : row)
		{
			item = 0;
			for (int i = 0; i < NoiseNum; ++i)
				item += -1. + 2. * double(rand()) / (double(RAND_MAX));
		}
		//row = three_point_derivative(row, 1);
	}

	auto calcE = [](vector<vector<double>>& source) -> double {
		double E = 0;
		for (auto& row : source)
			for (auto& item : row)
				E += item * item;
		return E;
		};

	double Es = calcE(target);
	double En = calcE(noise);
	double betta = sqrt(NoiseLevel * Es / En);

	for (int i = 0; i < target.size(); ++i)
	{
		for (int j = 0; j < target[i].size(); ++j)
		{
			target[i][j] += noise[i][j] * betta;
		}
	}
}

double Ediff(vector<vector<double>>& s1, vector<vector<double>>& s2)
{
	double res = 0;
	double Es = 0;
	for (int i = 0; i < s1.size(); ++i)
	{
		for (int j = 0; j < s1[i].size(); ++j)
		{
			double temp = s1[i][j] - s2[i][j];
			res += temp * temp;
			Es += s1[i][j] * s1[i][j];
		}
	}
	return res / Es;
}

double POSSH(vector<vector<double>>& s1, vector<vector<double>>& s2)
{
	double summ = 0;
	for (int i = 0; i < s1.size(); ++i)
	{
		for (int j = 0; j < s1[i].size(); ++j)
		{
			double temp = s1[i][j] - s2[i][j];
			summ += temp * temp;
		}
	}
	summ /= s1.size() * s1.front().size();
	return 20. * log10 (255. / sqrt(summ));
}

void DWT::dwt2d(std::vector<std::vector<double>>& image, std::vector<double>& filter, int decompositionLevel) {
	if (decompositionLevel == 0) return;

	SymmetricInterpolation h0(filter);
	SymmetricInterpolation h1 = ProduceH1(h0);

	// Обработка строк
	for (auto& row : image) {
		std::vector<double> a, d;
		Decompose(SymmetricInterpolation(row), a, d, h0, h1);
		row.clear();
		row.insert(row.end(), a.begin(), a.end());
		row.insert(row.end(), d.begin(), d.end());
	}

	// Транспонирование для обработки столбцов
	transposeMatrix(image);

	// Обработка столбцов (теперь строки)
	for (auto& col : image) {
		std::vector<double> a, d;
		Decompose(SymmetricInterpolation(col), a, d, h0, h1);
		col.clear();
		col.insert(col.end(), a.begin(), a.end());
		col.insert(col.end(), d.begin(), d.end());
	}

	// Транспонирование обратно
	transposeMatrix(image);

	// Выделение LL поддиапазона для следующего уровня
	int halfRows = image.size() / 2;
	int halfCols = image[0].size() / 2;

	std::vector<std::vector<double>> LL;
	for (int i = 0; i < halfRows; ++i) {
		std::vector<double> subRow(image[i].begin(), image[i].begin() + halfCols);
		LL.push_back(subRow);
	}

	// Рекурсивно применить DWT к LL
	if (decompositionLevel > 1) {
		dwt2d(LL, filter, decompositionLevel - 1);

		// Вставить обратно LL в соответствующее место
		for (int i = 0; i < halfRows; ++i) {
			for (int j = 0; j < halfCols; ++j) {
				image[i][j] = LL[i][j];
			}
		}
	}
}

void DWT::idwt2d(std::vector<std::vector<double>>& coeffs, std::vector<double>& filter, int decompositionLevel) {
	if (decompositionLevel == 0) return;

	SymmetricInterpolation h0(filter);
	SymmetricInterpolation g0 = ProduceG(h0);
	SymmetricInterpolation g1 = ProduceG(ProduceH1(h0));

	int halfRows = coeffs.size() / 2;
	int halfCols = coeffs[0].size() / 2;

	// Если есть более глубокий уровень, сначала восстановить LL
	if (decompositionLevel > 1) {
		std::vector<std::vector<double>> LL;
		for (int i = 0; i < halfRows; ++i) {
			std::vector<double> subRow(coeffs[i].begin(), coeffs[i].begin() + halfCols);
			LL.push_back(subRow);
		}
		idwt2d(LL, filter, decompositionLevel - 1);

		// Вставить обратно LL
		for (int i = 0; i < halfRows; ++i) {
			for (int j = 0; j < halfCols; ++j) {
				coeffs[i][j] = LL[i][j];
			}
		}
	}

	// Транспонирование для обработки столбцов как строк
	transposeMatrix(coeffs);

	// Обратное преобразование для столбцов
	for (auto& col : coeffs) {
		std::vector<double> a(col.begin(), col.begin() + halfRows);
		std::vector<double> d(col.begin() + halfRows, col.end());
		std::vector<double> restored;
		Recombine(a, d, g0, g1, restored);
		col = restored;
	}

	// Транспонирование обратно
	transposeMatrix(coeffs);

	// Обратное преобразование для строк
	for (auto& row : coeffs) {
		std::vector<double> a(row.begin(), row.begin() + halfCols);
		std::vector<double> d(row.begin() + halfCols, row.end());
		std::vector<double> restored;
		Recombine(a, d, g0, g1, restored);
		row = restored;
	}
}

void ApplyThreshold(std::vector<std::vector<double>>& coeffs, double threshold) {
	for (auto& row : coeffs) {
		for (auto& val : row) {
			// Мягкое пороговое значение (soft thresholding)
			if (val > threshold) val -= threshold;
			else if (val < -threshold) val += threshold;
			else val = 0;
		}
	}
}

double EstimateSigma(const std::vector<std::vector<double>>& coeffs) {
	// Извлекаем коэффициенты HH первого уровня (высокочастотные детали)
	std::vector<double> details;
	for (const auto& row : coeffs) {
		for (const auto& val : row) {
			details.push_back(val);
		}
	}

	// Вычисляем медиану абсолютных отклонений (MAD)
	std::sort(details.begin(), details.end());
	double median = details[details.size() / 2];
	std::vector<double> absDeviations;
	for (const auto& val : details) {
		absDeviations.push_back(fabs(val - median));
	}
	std::sort(absDeviations.begin(), absDeviations.end());
	double mad = absDeviations[absDeviations.size() / 2];

	// Для гауссовского шума: sigma = MAD / 0.6745
	return mad / 0.6745;
}

void MergeSubbands(const std::vector<std::vector<double>>& LL,
	const std::vector<std::vector<double>>& LH,
	const std::vector<std::vector<double>>& HL,
	const std::vector<std::vector<double>>& HH,
	std::vector<std::vector<double>>& out) {
	int rows = LL.size() * 2;
	int cols = LL[0].size() * 2;
	out.resize(rows, std::vector<double>(cols));

	for (int i = 0; i < LL.size(); ++i) {
		for (int j = 0; j < LL[0].size(); ++j) {
			out[i][j] = LL[i][j];          // LL
			out[i][j + LL[0].size()] = LH[i][j]; // LH
			out[i + LL.size()][j] = HL[i][j]; // HL
			out[i + LL.size()][j + LL[0].size()] = HH[i][j]; // HH
		}
	}
}

void DenoiseImage(std::vector<std::vector<double>>& image,
	std::vector<double>& filter,
	int decompositionLevel,
	double noiseSigma) {
	// Шаг 1: Прямое DWT

	
	DWT dwt;
	dwt.dwt2d(image, filter, decompositionLevel);

	// Шаг 2: Вычисление порога (по Доногоу)
	double threshold = noiseSigma * sqrt(2.0 * log(double(image.size() * image[0].size())));

	// Шаг 3: Пороговая обработка высокочастотных поддиапазонов
	for (int level = 0; level < decompositionLevel; ++level) {
		// Выделите поддиапазоны LH, HL, HH на текущем уровне
		std::vector<std::vector<double>> LL, LH, HL, HH;
		splitSubbands(image, LL, LH, HL, HH);

		// Примените порог к LH, HL, HH
		ApplyThreshold(LH, threshold);
		ApplyThreshold(HL, threshold);
		ApplyThreshold(HH, threshold);

		ShowImage(L"LH Clear", LH, g_pParent);
		ShowImage(L"HL Clear", HL, g_pParent);
		ShowImage(L"HH Clear", HH, g_pParent);

		// Объедините поддиапазоны обратно
		MergeSubbands(LL, LH, HL, HH, image);
	}

	// Шаг 4: Обратное DWT
	dwt.idwt2d(image, filter, decompositionLevel);
}

//void Normalize(vector<vector<double>>& pic)
//{
//	double min = 0;
//	double max = 0;
//	for (auto& row : pic)
//		for (auto& item : row)
//		{
//			if (item < min)min = item;
//			if (item > max)max = item;
//		}
//	double range = max - min;
//
//	for (auto& row : pic)
//		for (auto& item : row)
//		{
//			item = (item - min) / range;
//		}
//}

void Centralize(vector<vector<double>>& image)
{
	double min = 0;
	for (auto& row : image)
		for (auto& item : row)
			if (item < min)min = item;
	for (auto& row : image)
		for (auto& item : row)
			item -= min;
}

void DenoiseImageByAvg(std::vector<std::vector<double>>& image, std::vector<double>& filter, int decompositionLevel)
{
	DWT dwt;
	dwt.dwt2d(image, filter, decompositionLevel);

	auto AvgThreshold = [](vector<vector<double>>& pic, double mul) {
		double max = 0;
		double avg = 0;
		for (auto& row : pic)
			for (auto& item : row)
			{
				avg += item;
				if (item > max)max = item;
			}
		avg /= pic.size() * pic[0].size();

		double threshold = (avg);
		for (auto& row : pic)
			for (auto& item : row)
			{
				if (item < threshold)item *= mul;
			}
		};

	auto SmartThreshold = [](vector<vector<double>>& pic, double mul) {
		auto get = [](vector<vector<double>>& from, int i, int j, double min)
			{
				if (i < 0)return min;
				if (j < 0)return min;
				if (i >= from.size())return min;
				if (j >= from.front().size())return min;
				return from[i][j];
			};
		auto Check = [&get](vector<vector<double>>& target, int i, int j, double min, double threshold)
			{
				if (get(target, i, j, min) < threshold)return false;
				if (get(target, i + 1, j, min) > threshold)return true;
				if (get(target, i + 1, j + 1, min) > threshold)return true;
				if (get(target, i + 1, j - 1, min) > threshold)return true;
				if (get(target, i, j + 1, min) > threshold)return true;
				if (get(target, i, j - 1, min) > threshold)return true;
				if (get(target, i - 1, j - 1, min) > threshold)return true;
				if (get(target, i - 1, j, min) > threshold)return true;
				if (get(target, i - 1, j + 1, min) > threshold)return true;
				return false;
			};

		double max = 0;
		double min = 0;
		double avg = 0;
		for (auto& row : pic)
			for (auto& item : row)
			{
				avg += item;
				if (item > max)max = item;
				if (item < min)min = item;
			}
		avg /= pic.size() * pic[0].size();

		double threshold = (avg);
		for (int i = 0; i < pic.size(); ++i)
		{
			for (int j = 0; j < pic[i].size(); ++j)
			{
				if (!Check(pic, i, j, min, threshold))pic[i][j] *= mul;
			}
		}

		};

	double mul = 0.1;
	for (int level = 0; level < decompositionLevel; ++level) {
		// Выделите поддиапазоны LH, HL, HH на текущем уровне
		std::vector<std::vector<double>> LL, LH, HL, HH;
		splitSubbands(image, LL, LH, HL, HH);

		// Примените порог к LH, HL, HH
		AvgThreshold(LH, mul);
		AvgThreshold(HL, mul);
		AvgThreshold(HH, mul);

		/*SmartThreshold(LH, mul);
		SmartThreshold(HL, mul);
		SmartThreshold(HH, mul);*/

		ShowImage(L"LH Clear", LH, g_pParent);
		ShowImage(L"HL Clear", HL, g_pParent);
		ShowImage(L"HH Clear", HH, g_pParent);

		// Объедините поддиапазоны обратно
		MergeSubbands(LL, LH, HL, HH, image);
	}

	// Шаг 4: Обратное DWT
	dwt.idwt2d(image, filter, decompositionLevel);
}

#include"CustomPicture2DDlg.h"
void ShowImage(CString label, vector<vector<double>>& pic, CWnd* parent)
{
	auto win = new CustomPicture2DDlg(label, 1000, 800, parent);
	win->customPicture.SetGraphRange(0, pic[0].size(), 0, pic.size());
	win->customPicture.SetData(pic);
	win->MyShow();
}

LoopInterpolation::LoopInterpolation(vector<double>& source)
	:Interpolation(data)
{
}

double LoopInterpolation::operator[](int index) const
{
	int size = data.size();
	if (index < 0)return data[size + index];
	if (index >= size)return data[index - size];
	return data[index];
}


#ifdef max
#undef max
#endif

// Нормализация изображения [0, 1]
void Normalize(vector<vector<double>>& pic) {
	double min = numeric_limits<double>::max();
	double max = numeric_limits<double>::lowest();

	for (const auto& row : pic) {
		for (double val : row) {
			if (val < min) min = val;
			if (val > max) max = val;
		}
	}

	double range = max - min;
	if (range == 0) return;

	for (auto& row : pic) {
		for (double& val : row) {
			val = (val - min) / range;
		}
	}
}

// Вычисление корреляции между двумя матрицами
double Correlation(const vector<vector<double>>& a, const vector<vector<double>>& b) {
	if (a.size() != b.size() || a[0].size() != b[0].size()) {
		throw invalid_argument("Матрицы должны быть одинакового размера");
	}

	int rows = a.size();
	int cols = a[0].size();
	int n = rows * cols;

	double meanA = 0, meanB = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			meanA += a[i][j];
			meanB += b[i][j];
		}
	}
	meanA /= n;
	meanB /= n;

	double numerator = 0, denomA = 0, denomB = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			double aVal = a[i][j] - meanA;
			double bVal = b[i][j] - meanB;
			numerator += aVal * bVal;
			denomA += aVal * aVal;
			denomB += bVal * bVal;
		}
	}

	return numerator / (sqrt(denomA) * sqrt(denomB));
}

// Функция поиска объекта на изображении
vector<pair<RECT, double>> FindObject(
	vector<vector<double>>& image,
	const vector<vector<double>>& pattern,
	DWT& dwt,
	const vector<double>& filter,
	int decompositionLevel,
	double threshold
) {
	// Нормализация изображения и шаблона
	Normalize(image);
	vector<vector<double>> normalizedPattern = pattern;
	Normalize(normalizedPattern);

	// Проверка размеров
	int patternHeight = normalizedPattern.size();
	int patternWidth = normalizedPattern[0].size();
	int imageHeight = image.size();
	int imageWidth = image[0].size();

	if (patternHeight > imageHeight || patternWidth > imageWidth) {
		throw invalid_argument("Шаблон больше изображения");
	}

	// Применение DWT к шаблону
	vector<vector<double>> patternDWT = normalizedPattern;
	dwt.dwt2d(patternDWT, const_cast<vector<double>&>(filter), decompositionLevel);

	// Результаты поиска
	vector<pair<RECT, double>> matches;

	// Скользящее окно по изображению
	for (int y = 0; y <= imageHeight - patternHeight; ++y) {
		for (int x = 0; x <= imageWidth - patternWidth; ++x) {
			// Вырезаем участок изображения
			vector<vector<double>> window(patternHeight, vector<double>(patternWidth));
			for (int i = 0; i < patternHeight; ++i) {
				for (int j = 0; j < patternWidth; ++j) {
					window[i][j] = image[y + i][x + j];
				}
			}

			// Нормализация окна
			Normalize(window);

			// Применение DWT к окну
			vector<vector<double>> windowDWT = window;
			dwt.dwt2d(windowDWT, const_cast<vector<double>&>(filter), decompositionLevel);

			// Сравнение с шаблоном
			try {
				double score = Correlation(windowDWT, patternDWT);
				if (score >= threshold) {
					RECT rect = { x, y, x + patternWidth, y + patternHeight };
					matches.push_back({ rect, score });
				}
			}
			catch (const exception&) {
				// Игнорируем ошибки сравнения
			}
		}
	}

	return matches;
}

vector<pair<RECT, double>> FindObjectAdaptive(
	vector<vector<double>>& image,
	const vector<vector<double>>& pattern,
	DWT& dwt,
	const vector<double>& filter,
	int decompositionLevel,
	double threshold,
	int fastStep,   // Шаг при низкой корреляции
	int slowStep   // Шаг при высокой корреляции
) {
	// Нормализация изображения и шаблона
	Normalize(image);
	vector<vector<double>> normalizedPattern = pattern;
	Normalize(normalizedPattern);

	// Размеры
	int patternHeight = normalizedPattern.size();
	int patternWidth = normalizedPattern[0].size();
	int imageHeight = image.size();
	int imageWidth = image[0].size();

	if (patternHeight > imageHeight || patternWidth > imageWidth) {
		throw invalid_argument("Шаблон больше изображения");
	}

	// Применение DWT к шаблону
	vector<vector<double>> patternDWT = normalizedPattern;
	dwt.dwt2d(patternDWT, const_cast<vector<double>&>(filter), decompositionLevel);
	vector < vector<double>>pHH, pLL, pHL, pLH;
	splitSubbands(patternDWT, pLL, pLH, pHL, pHH);

	// Результаты
	vector<pair<RECT, double>> matches;

	// Адаптивный шаг
	for (int y = 0; y <= imageHeight - patternHeight; ) {
		for (int x = 0; x <= imageWidth - patternWidth; ) {
			// Вырезаем участок
			vector<vector<double>> window(patternHeight, vector<double>(patternWidth));
			for (int i = 0; i < patternHeight; ++i) {
				for (int j = 0; j < patternWidth; ++j) {
					window[i][j] = image[y + i][x + j];
				}
			}

			// Нормализация окна
			Normalize(window);

			// Применение DWT
			vector<vector<double>> windowDWT = window;
			dwt.dwt2d(windowDWT, const_cast<vector<double>&>(filter), decompositionLevel);

			vector<vector<double>>wHH, wLL, wHL, wLH;
			splitSubbands(windowDWT, wLL, wLH, wHL, wHH);

			// Сравнение
			double score = Correlation(wLL, pLL);
			if (score >= threshold) {
				RECT rect = { x, y, x + patternWidth, y + patternHeight };
				matches.push_back({ rect, score });
			}

			// Адаптивный шаг
			int step = (score < threshold * 0.7) ? fastStep : slowStep;
			x += step;
		}
		y += 1; // Обработка по строкам
	}

	return matches;
}