#pragma once


// Drawer
#include<gdiplus.h>
#include<vector>
#include<algorithm>
using namespace std;
using namespace Gdiplus;


class Drawer : public CStatic
{
	DECLARE_DYNAMIC(Drawer)
private:
	vector<double>data;
	vector<double>keys;

	double lPadding = 22;
	double rPadding = 5;
	double tPadding = 10;
	double bPadding = 10;
	int MarksLen;

	double left;
	double right;
	double bot;
	double top;


	ULONG_PTR token;
	
	CString title;
	bool ShowAvgFlag = false;
	double Avg = 0;
	bool UpdateAvgFlag = false;
public:
	Drawer();
	virtual ~Drawer();

protected:
	double CalcStringLen(HDC hDC, CString string);
	void AutoKeys();
	void CalcAvg();
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void SetData(vector<double>&y);
	void SetKeys(vector<double>&x);
	void SetData(vector<double>&y, vector<double>&x);

	//Отступ задается в процентах (диапазон значений от 0 до 100)
	void SetPadding(double lPadding, double rPadding, double tPadding, double bPadding);
	void SetMarksLen(int len);
	void SetTitle(CString str);
	void ShowAvg(bool show = true);
};


