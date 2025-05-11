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

};

vector<vector<double>> DispatchPicture(CString path);
void GDIINIT();
void GDIDEINIT();