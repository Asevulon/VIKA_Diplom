#pragma once

#include <vector>
#include <array>
#include <string>
#include <gdiplus.h>
using namespace std;
namespace Filters
{
	static std::vector<  double>db4
	{
		0.482962913144534, 0.836516303737469,
	0.224143868042013, -0.129409522551260
	};
	static std::vector<double>db8
	{
		0.32580343, 1.01094572, 0.89220014, -0.03957503,
		-0.26450717, 0.0436163, 0.0465036, -0.01498699
	};
	static std::vector<double>haar
	{
		1,1
	};
}

class Interpolation
{
protected:
	std::vector<double>data;
public:
	Interpolation(vector<double>& source);
	virtual double operator[](int index) const = 0;
	size_t Size();
};

class SymmetricInterpolation :public Interpolation
{
protected:
	int get_index(int index) const;
public:
	SymmetricInterpolation(vector<double>& source);
	double operator[](int index) const override;
};

struct DWTLevel
{
	vector<double> a;
	vector<double> d;
};

class DWT
{
private:
protected:
	vector<double>Downscale(vector<double>& source);
	vector<double>UpScale(vector<double>& source);
	SymmetricInterpolation ProduceH1(SymmetricInterpolation& h0);
	SymmetricInterpolation ProduceG(SymmetricInterpolation& h);
	void Decompose(SymmetricInterpolation& source, vector<double>& a, vector<double>& d, SymmetricInterpolation& h0, SymmetricInterpolation h1);
	void Recombine(vector<double>& a, vector<double>& d, SymmetricInterpolation& g0, SymmetricInterpolation g1, vector<double>& out);
	void Convolution(SymmetricInterpolation& f, SymmetricInterpolation& g, vector<double>& out);
public:
	void Transform(std::vector<double>& filter, vector<double>& source, uint64_t decompositionLevel, vector<DWTLevel>& out);
	void Recover(std::vector<double>& filter, vector<DWTLevel>& in, vector<double>& out);
	void DWT::dwt2d(std::vector<std::vector<double>>& image, std::vector<double>& filter, int decompositionLevel);
	void DWT::idwt2d(std::vector<std::vector<double>>& coeffs, std::vector<double>& filter, int decompositionLevel);
};

vector<vector<double>> DispatchPicture(CString path);
void GDIINIT();
void GDIDEINIT();

void transposeMatrix(std::vector<std::vector<double>>& matrix);
void splitSubbands(const std::vector<std::vector<double>>& coeffs,
	std::vector<std::vector<double>>& LL,
	std::vector<std::vector<double>>& LH,
	std::vector<std::vector<double>>& HL,
	std::vector<std::vector<double>>& HH);
void MergeSubbands(const std::vector<std::vector<double>>& LL,
	const std::vector<std::vector<double>>& LH,
	const std::vector<std::vector<double>>& HL,
	const std::vector<std::vector<double>>& HH,
	std::vector<std::vector<double>>& out);

void Noise(vector<vector<double>>& target, double NoiseLevel);
double Ediff(vector<vector<double>>& s1, vector<vector<double>>& s2);
void ApplyThreshold(std::vector<std::vector<double>>& coeffs, double threshold);
double EstimateSigma(const std::vector<std::vector<double>>& coeffs);
void DenoiseImage(std::vector<std::vector<double>>& image,
	std::vector<double>& filter,
	int decompositionLevel,
	double noiseSigma);

void Normalize(vector<vector<double>>& pic);