#pragma once

#include "MzPoint.h"

void SplitLine(const CString& strFields, const CString& strDelimiters, CStringArray& arFields);
void SplitString(CString& buf, CString& delimiter, CStringArray& words);
void num3sort(int a, int b, int c, int& s0, int& s1, int& s2);
double AreaTriangle(CMzPoint& p0, CMzPoint& p1, CMzPoint& p2);
double AreaQuad(CMzPoint& p0, CMzPoint& p1, CMzPoint& p2, CMzPoint& p3);
bool IsInteger(CString& s);