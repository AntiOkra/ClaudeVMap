
// VectorMappingDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "Adx.h"
#include "CNastran.h"
#include "SurfaceNode.h"
#include "afxcmn.h"
#include "AdxElementSet.h"
#include "SortParameter.h"

// CVectorMappingDlg ダイアログ
class CVectorMappingDlg : public CDialogEx
{
// コンストラクション
public:
	CVectorMappingDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VECTORMAPPING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CAdx			m_Adx;
	CNastran		m_Nastran;
	CSurfaceNode	m_SurfaceNode;

	CString		m_PathAdx;
	CString		m_PathNastran;
	CString		m_PathNormalPressure;
	CString		m_PathTangentPressure;
	double		m_SearchDistance;
	CString		m_PathForce;
	CListBox	m_ListElementSet;
	CListCtrl	m_ListParts;
	CStdioFile	m_LogFile;
	CString		m_str_selcount;
	CString		m_StrForceX;
	CString		m_StrForceY;
	CString		m_StrForceZ;
	CString		m_StrForce;
	double		m_MappingRatio;
	double		m_MappingRatio_X;
	double		m_MappingRatio_Y;
	double		m_MappingRatio_Z;
	bool		m_LogIsOpen;
	BOOL		m_RatioIsXYZ;
	CString		m_StrForceNorm;
	CSortParameter m_sortParam;	//! ソートの方法を指定する	

	afx_msg void OnBnClickedButtonAdx();
	afx_msg void OnBnClickedButtonNastran();
	afx_msg void OnBnClickedButtonNormal();
	afx_msg void OnBnClickedButtonTangent();
	afx_msg void OnBnClickedButtonOut();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAdxread();
	afx_msg void OnBnClickedButtonExec();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeListAdx();
	afx_msg void OnBnClickedButtonClearSelect();
	afx_msg void OnBnClickedButtonForceCalc();
	afx_msg void OnEnKillfocusEditMappingRatio();
	afx_msg void OnEnKillfocusEditUprLimit();
	afx_msg void OnEnKillfocusEditMappingRatioX();
	afx_msg void OnEnKillfocusEditMappingRatioY();
	afx_msg void OnEnKillfocusEditMappingRatioZ();
	afx_msg void OnLvnColumnclickListParts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListParts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckXyz();	
	void ListInsertElementSet(int no_row, CAdxElementSet &es);
	void ControlEnable(bool to_enable);
	static int CALLBACK CompareData(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	afx_msg void OnEnKillfocusEditNastran();
	afx_msg void OnEnKillfocusEditNormal();
	afx_msg void OnEnKillfocusEditTangent();
	afx_msg void OnEnKillfocusEditOutForce();
};
