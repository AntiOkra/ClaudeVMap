
// VectorMapping.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CVectorMappingApp:
// このクラスの実装については、VectorMapping.cpp を参照してください。
//

class CVectorMappingApp : public CWinApp
{
public:
	CVectorMappingApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CVectorMappingApp theApp;