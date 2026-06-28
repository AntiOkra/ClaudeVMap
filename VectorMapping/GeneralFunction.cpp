#include "stdafx.h"
#include "GeneralFunction.h"
#include "MzPoint.h"

// https://www.codeproject.com/Tips/676464/How-to-Parse-Empty-Tokens-using-CString-Tokenize
void SplitLine(const CString& strFields, const CString& strDelimiters, CStringArray& arFields)
{
	arFields.RemoveAll();

	// Do not process empty strings.
	if (!strFields.IsEmpty() && !strDelimiters.IsEmpty())
	{
		int nPosition = 0, nTotalFields = 0;

		do
		{
			int nOldPosition = nPosition;   // Store the previous position value.

			CString strField = strFields.Tokenize(strDelimiters, nPosition);
			if (nPosition != -1)
			{
				nTotalFields += (nPosition - nOldPosition - strField.GetLength());
			}
			else
			{
				nTotalFields += (strFields.GetLength() + 1 - nOldPosition);
			}

			// By using SetAtGrow(), empty strings are automatically added to the array.
			arFields.SetAtGrow(nTotalFields - 1, strField);
		} while (nPosition != -1 && nPosition <= strFields.GetLength());
	}
}

//  Split() : 文字列分割(共用関数) ( 指定記号で分割 )

void SplitString(CString& buf, CString& delimiter, CStringArray& words)
{
	CString word;
	int i;

	// 初期化
	words.RemoveAll();

	enum { sBegin, sEnd, sSkip, sOut } status;

	status = sBegin;
	word = "";

	// 分割Loop
	for (i = 0; i < buf.GetLength(); i++) {

		bool is_skipchar = ( delimiter.Find(buf.GetAt(i))>=0 );

		switch (status) {

		case sBegin:	// 開始時の状態
			if (is_skipchar) {
				status = sSkip;
			}
			else {
				status = sOut;
				word += buf.GetAt(i);
			}
			break;

		case sSkip:	// スキップ状態
			if (is_skipchar) {
				status = sSkip;
			}
			else {
				status = sOut;
				word += buf[i];
			}
			break;

		case sOut:	// アウトプット状態
			if (is_skipchar) {
				status = sSkip;
				words.Add(word);
				word = "";
			}
			else {
				status = sOut;
				word += buf[i];
			}
			break;
		}
	}

	// 終了ステータス
	switch (status) {
	case sBegin: break;
	case sSkip: break;
	case sOut: words.Add(word); break;
	}

}

void num3sort(int a, int b, int c, int& s0, int& s1, int& s2)
{
	s0 = -1;
	s1 = -1;
	s2 = -1;

	// a<b<c
	if (a < b && b < c) {
		s0 = a;
		s1 = b;
		s2 = c;
	}
	// a<c<b
	else if (a < c && c < b) {
		s0 = a;
		s1 = c;
		s2 = b;
	}
	// b<a<c
	else if (b < a && a < c) {
		s0 = b;
		s1 = a;
		s2 = c;
	}
	// b<c<a
	else if (b < c && c < a) {
		s0 = b;
		s1 = c;
		s2 = a;
	}
	// c<a<b
	else if (c < a && a < b) {
		s0 = c;
		s1 = a;
		s2 = b;
	}
	// c<b<a
	else if (c < b && b < a) {
		s0 = c;
		s1 = b;
		s2 = a;
	}
}

double AreaTriangle(CMzPoint& p0, CMzPoint& p1, CMzPoint& p2 )
{
	CMzPoint v0 = p2 - p1;
	CMzPoint v1 = p0 - p1;

	CMzPoint vcrs = v0.CrossProduct(v1);
	
	return vcrs.Length() / 2.0;
}

double AreaQuad(CMzPoint& p0, CMzPoint& p1, CMzPoint& p2, CMzPoint& p3)
{
	double a1, a2;

	a1 = AreaTriangle(p0, p1, p2);
	a2 = AreaTriangle(p2, p3, p0);

	return a1 + a2;
}

bool IsInteger(CString& s)
{
	CString res = s.SpanIncluding(_T("0123456789"));
	if (res == s) {
		return true;
	}
	else {
		return false;
	}
}