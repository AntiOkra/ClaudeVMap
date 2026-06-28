#pragma once

// http://d.hatena.ne.jp/parasporospa/20050419/1113878542

class CSortParameter
{
public:
	CSortParameter();
	~CSortParameter();

	BOOL m_bSort;	//!< 昇順降順のフラグ
	int m_nIndex;	//!< ソートするIndexの番号
};