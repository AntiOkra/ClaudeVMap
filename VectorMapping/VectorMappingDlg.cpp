
// VectorMappingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "VectorMapping.h"
#include "VectorMappingDlg.h"
#include "afxdialogex.h"
#include <math.h>
#include <locale.h>
#include <algorithm>

#include "Adx.h"
#include "AdxElementSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVectorMappingDlg ダイアログ



CVectorMappingDlg::CVectorMappingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VECTORMAPPING_DIALOG, pParent)
	, m_PathAdx(_T(""))
	, m_PathNastran(_T(""))
	, m_PathNormalPressure(_T(""))
	, m_PathTangentPressure(_T(""))
	, m_SearchDistance(0)
	, m_PathForce(_T(""))
	, m_str_selcount(_T(""))
	, m_StrForceX(_T("-"))
	, m_StrForceY(_T("-"))
	, m_StrForceZ(_T("-"))
	, m_MappingRatio(1)
	, m_MappingRatio_X(1)
	, m_MappingRatio_Y(1)
	, m_MappingRatio_Z(1)
	, m_RatioIsXYZ(FALSE)
	, m_StrForceNorm(_T("-"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVectorMappingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADX, m_PathAdx);
	DDX_Text(pDX, IDC_EDIT_NASTRAN, m_PathNastran);
	DDX_Text(pDX, IDC_EDIT_NORMAL, m_PathNormalPressure);
	DDX_Text(pDX, IDC_EDIT_TANGENT, m_PathTangentPressure);
	DDX_Text(pDX, IDC_EDIT_UPR_LIMIT, m_SearchDistance);
	DDV_MinMaxDouble(pDX, m_SearchDistance, 1.0, 30.0);
	DDX_Text(pDX, IDC_EDIT_OUT_FORCE, m_PathForce);
	DDX_Control(pDX, IDC_LIST_ADX, m_ListElementSet);
	DDX_Control(pDX, IDC_LIST_PARTS, m_ListParts);
	DDX_Text(pDX, IDC_STATIC_SEL_COUNT, m_str_selcount);
	DDV_MaxChars(pDX, m_str_selcount, 20);
	DDX_Text(pDX, IDC_EDIT_FORCE_X, m_StrForceX);
	DDX_Text(pDX, IDC_EDIT_FORCE_Y, m_StrForceY);
	DDX_Text(pDX, IDC_EDIT_FORCE_Z, m_StrForceZ);
	DDX_Text(pDX, IDC_EDIT_FORCE_X2, m_MappingRatio);
	DDX_Text(pDX, IDC_EDIT_MAPPING_RATIO, m_MappingRatio);
	DDX_Text(pDX, IDC_EDIT_MAPPING_RATIO_X, m_MappingRatio_X);
	DDX_Text(pDX, IDC_EDIT_MAPPING_RATIO_Y, m_MappingRatio_Y);
	DDX_Text(pDX, IDC_EDIT_MAPPING_RATIO_Z, m_MappingRatio_Z);
	DDX_Check(pDX, IDC_CHECK_XYZ, m_RatioIsXYZ);
	DDX_Text(pDX, IDC_EDIT_FORCE_NORM, m_StrForceNorm);
}

BEGIN_MESSAGE_MAP(CVectorMappingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADX, &CVectorMappingDlg::OnBnClickedButtonAdx)
	ON_BN_CLICKED(IDC_BUTTON_NASTRAN, &CVectorMappingDlg::OnBnClickedButtonNastran)
	ON_BN_CLICKED(IDC_BUTTON_NORMAL, &CVectorMappingDlg::OnBnClickedButtonNormal)
	ON_BN_CLICKED(IDC_BUTTON_TANGENT, &CVectorMappingDlg::OnBnClickedButtonTangent)
	ON_BN_CLICKED(IDC_BUTTON_OUT, &CVectorMappingDlg::OnBnClickedButtonOut)
	ON_BN_CLICKED(IDOK, &CVectorMappingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADXREAD, &CVectorMappingDlg::OnBnClickedButtonAdxread)
	ON_BN_CLICKED(IDC_BUTTON_EXEC, &CVectorMappingDlg::OnBnClickedButtonExec)
	ON_BN_CLICKED(IDCANCEL, &CVectorMappingDlg::OnBnClickedCancel)
	ON_LBN_SELCHANGE(IDC_LIST_ADX, &CVectorMappingDlg::OnLbnSelchangeListAdx)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SELECT, &CVectorMappingDlg::OnBnClickedButtonClearSelect)
	ON_BN_CLICKED(IDC_BUTTON_FORCE_CALC, &CVectorMappingDlg::OnBnClickedButtonForceCalc)
	ON_EN_KILLFOCUS(IDC_EDIT_MAPPING_RATIO, &CVectorMappingDlg::OnEnKillfocusEditMappingRatio)
	ON_EN_KILLFOCUS(IDC_EDIT_UPR_LIMIT, &CVectorMappingDlg::OnEnKillfocusEditUprLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_MAPPING_RATIO_X, &CVectorMappingDlg::OnEnKillfocusEditMappingRatioX)
	ON_EN_KILLFOCUS(IDC_EDIT_MAPPING_RATIO_Y, &CVectorMappingDlg::OnEnKillfocusEditMappingRatioY)
	ON_EN_KILLFOCUS(IDC_EDIT_MAPPING_RATIO_Z, &CVectorMappingDlg::OnEnKillfocusEditMappingRatioZ)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PARTS, &CVectorMappingDlg::OnLvnColumnclickListParts)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARTS, &CVectorMappingDlg::OnLvnItemchangedListParts)
	ON_BN_CLICKED(IDC_CHECK_XYZ, &CVectorMappingDlg::OnBnClickedCheckXyz)
	ON_EN_KILLFOCUS(IDC_EDIT_NASTRAN, &CVectorMappingDlg::OnEnKillfocusEditNastran)
	ON_EN_KILLFOCUS(IDC_EDIT_NORMAL, &CVectorMappingDlg::OnEnKillfocusEditNormal)
	ON_EN_KILLFOCUS(IDC_EDIT_TANGENT, &CVectorMappingDlg::OnEnKillfocusEditTangent)
	ON_EN_KILLFOCUS(IDC_EDIT_OUT_FORCE, &CVectorMappingDlg::OnEnKillfocusEditOutForce)
END_MESSAGE_MAP()


// CVectorMappingDlg メッセージ ハンドラー

BOOL CVectorMappingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	m_SearchDistance = 10.0;
	m_MappingRatio = 1.0;
	m_MappingRatio_X = 1.0;
	m_MappingRatio_Y = 1.0;
	m_MappingRatio_Z = 1.0;
	m_str_selcount = "0";

	m_StrForceX = "-";
	m_StrForceY = "-";
	m_StrForceZ = "-";
	m_StrForceNorm = "-";

	m_RatioIsXYZ = FALSE;

	DWORD dwStyle = m_ListParts.GetExtendedStyle();
	//dwStyle |= LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT| LVS_EX_HEADERINALLVIEWS;
	m_ListParts.SetExtendedStyle(dwStyle);

	m_ListParts.InsertColumn(0, _T("NAME"), LVCFMT_LEFT, 200);
	m_ListParts.InsertColumn(1, _T("ID"), LVCFMT_RIGHT, 100);
	m_ListParts.InsertColumn(2, _T("COMMENT"), LVCFMT_LEFT, 200);

	m_LogIsOpen = false;

	UpdateData(FALSE);

	ControlEnable(false);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CVectorMappingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CVectorMappingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CVectorMappingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVectorMappingDlg::OnBnClickedButtonAdx()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString         filter("ADX Files (*.adx)|*.adx||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);

	if (selDlg.DoModal() == IDOK)
	{
		m_PathAdx = selDlg.GetPathName();

		m_ListElementSet.ResetContent();
		m_ListParts.DeleteAllItems();

		CString sel_string;
		sel_string.Format(_T("%d"), 0);
		m_str_selcount = sel_string;

		UpdateData(FALSE);
	}

	return;
}


void CVectorMappingDlg::OnBnClickedButtonNastran()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString         filter("NASTRAN Files (*.nas)|*.nas||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);

	if (selDlg.DoModal() == IDOK)
	{
		m_PathNastran = selDlg.GetPathName();

		m_StrForceX = "-";
		m_StrForceY = "-";
		m_StrForceZ = "-";
		m_StrForceNorm = "-";

		UpdateData(FALSE);
	}

	return;
}


void CVectorMappingDlg::OnBnClickedButtonNormal()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString         filter("NormalContactPressureVector Files (*.asc)|*.asc||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);

	if (selDlg.DoModal() == IDOK)
	{
		m_PathNormalPressure = selDlg.GetPathName();

		m_StrForceX = "-";
		m_StrForceY = "-";
		m_StrForceZ = "-";
		m_StrForceNorm = "-";

		UpdateData(FALSE);
	}

	return;
}


void CVectorMappingDlg::OnBnClickedButtonTangent()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString         filter("TangentContactPressureVector Files (*.asc)|*.asc||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);

	if (selDlg.DoModal() == IDOK)
	{
		m_PathTangentPressure = selDlg.GetPathName();

		m_StrForceX = "-";
		m_StrForceY = "-";
		m_StrForceZ = "-";
		m_StrForceNorm = "-";

		UpdateData(FALSE);
	}

	return;
}


void CVectorMappingDlg::OnBnClickedButtonOut()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	// [名前を付けて保存]ダイアログ
	CString         filter("出力ファイル Files (*.adx)|*.adx||");
	CFileDialog selDlg(FALSE, _T("adx"), _T("force"), OFN_OVERWRITEPROMPT, filter);

	if (selDlg.DoModal() == IDOK) {
		m_PathForce = selDlg.GetPathName();
		UpdateData(FALSE);
	}

	return;
}


void CVectorMappingDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	CString log_path = _T("C:\\Users\\m871769\\Desktop\\SAMPLE\\log.txt");
	CStdioFile LogFile;

	// 出力ファイル
	BOOL ret;
	ret = LogFile.Open(log_path, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone |CFile::typeText);
	if (ret == FALSE) {
		return;
	}

	m_Nastran.m_LogFile = &LogFile;
	m_SurfaceNode.m_LogFile = &LogFile;

	if (m_Nastran.ReadNastranFile(m_PathNastran) != 0) {
		AfxMessageBox(_T("Nastranファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ReadNormalVectorFile(m_PathNormalPressure) != 0) {
		AfxMessageBox(_T("NormalPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ReadTangentVectorFile(m_PathTangentPressure) != 0) {
		AfxMessageBox(_T("TangentPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ForceCalc() != 0) {
		AfxMessageBox(_T("Nastranファイルの荷重を計算できませんでした"));
		return;
	}

	// マッピングするパーツを取得
	std::vector<int> v_sel;
	CStringArray v_name;

	for (int i = 0; i<m_ListElementSet.GetCount(); i++) {
		if (m_ListElementSet.GetSel(i)) {
			CString& name = (m_Adx.m_vElementSet[i])->m_NameAdx;
			v_name.Add(name);
		}
	}

	// Adxファイルの表面ノードを取得
	if (m_Adx.ExtractSurfaceNode(v_name, m_SurfaceNode) != 0) {
		AfxMessageBox(_T("表面ノードを取得できませんでした"));
		return;
	}
	else {
		// AfxMessageBox(_T("表面ノードを取得できました"));
	}

	// エリアマップ
	if (m_SurfaceNode.MakeAreaMap() != 0) {
		AfxMessageBox(_T("ノードマップ作成エラーです"));
		return;
	}

	CString dump_path;

	/* Dump ( debug用 )
	dump_path = _T("C:\\Users\\m871769\\Desktop\\SAMPLE\\nastran.txt");
	m_Nastran.Dump(dump_path);

	dump_path = _T("C:\\Users\\m871769\\Desktop\\SAMPLE\\dump_map.txt");
	m_SurfaceNode.Dump(dump_path);
	*/

	CMzPoint ratio;

	if (m_RatioIsXYZ) {
		ratio.x = m_MappingRatio_X;
		ratio.y = m_MappingRatio_Y;
		ratio.z = m_MappingRatio_Z;
	}
	else {
		ratio.x = m_MappingRatio;
		ratio.y = m_MappingRatio;
		ratio.z = m_MappingRatio;
	}

	// マッピング
	if (m_SurfaceNode.Mapping(m_Nastran, m_SearchDistance, ratio ) != 0) {
		AfxMessageBox(_T("マッピング実行時エラーです"));
		return;
	}

	// Forceデータ出力
	CString process = _T("TEST");
	if (m_SurfaceNode.ExportAdxForce(m_PathForce,process) != 0) {
		AfxMessageBox(_T("Forceファイル出力ができませんでした"));
	}

	CString buf;

	CMzPoint nastran_force;

	m_Nastran.GetTotalForce(nastran_force);
	buf.Format(_T("NASTRAN_FORCE:%10.3f,%10.3f,%10.3f\n"), nastran_force.x,nastran_force.y,nastran_force.z);
	LogFile.WriteString(buf);

	buf.Format(_T("MAPPED_FORCE:%10.3f,%10.3f,%10.3f\n"), m_SurfaceNode.m_MappedForce.x, m_SurfaceNode.m_MappedForce.y, m_SurfaceNode.m_MappedForce.z);
	LogFile.WriteString(buf);

	buf.Format(_T("LOSSED_FORCE:%10.3f,%10.3f,%10.3f\n"), m_SurfaceNode.m_LossForce.x, m_SurfaceNode.m_LossForce.y, m_SurfaceNode.m_LossForce.z);
	LogFile.WriteString(buf);

	LogFile.Close();

	CDialogEx::OnOK();
}

void DumpFileName(CString& nas_path, CString& e_path, CString& n_path)
{
	int ipos = nas_path.ReverseFind('.');
	CString folder = nas_path.Left(ipos);

	e_path.Format(_T("%s_element.csv"), folder);
	n_path.Format(_T("%s\\_node.csv"), folder);
}

void LogFileName(CString& adx_path, CString& log_path)
{
	int ipos = adx_path.ReverseFind('\\');
	CString folder = adx_path.Left(ipos);

	log_path.Format(_T("%s\\mapping_log.txt"), folder);
}

void ModFileName(CString& org_path, char period_char, CString& replace_str, CString& mod_path)
{
	int ipos = org_path.ReverseFind(period_char);
	CString org_left = org_path.Left(ipos);

	mod_path.Format(_T("%s%s"), org_left, replace_str);
}

void CVectorMappingDlg::OnBnClickedButtonAdxread()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	// ログファイルオープン
	CString log_path;
	LogFileName(m_PathAdx, log_path);

	CFileStatus fstatus;
	BOOL ret;

	if ( ! m_LogIsOpen ) {
		ret = m_LogFile.Open(log_path, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone | CFile::typeText);
		if (ret == FALSE) {
			return;
		}
		m_LogIsOpen = true;
	}

	// LOCALE設定
	setlocale(LC_CTYPE, "jpn");

	m_Adx.RemoveAll();

	if (m_Adx.Read(m_PathAdx) == 0) {

		if (m_Adx.Activate() != 0) {
			ControlEnable(false);
			AfxMessageBox(_T("ADXファイル構成できませんでした"));
			return;
		}

		// IDでソート
		//sort(m_Adx.m_vElementSet.begin(), m_Adx.m_vElementSet.end(), CAdxElementSet::Compare);

		CString s_item;
		for (int i = 0; i < m_Adx.m_vElementSet.size(); i++) {
			CAdxElementSet& es = *(m_Adx.m_vElementSet[i]);
			s_item.Format(_T("%-30s %-20s"), es.m_NameAdx.Trim(), es.m_NameUser.Trim());
			//m_ListElementSet.InsertString(i, es.m_NameAdx + " " + es.m_NameUser);
			m_ListElementSet.InsertString(i, s_item);
		}

		for (int i = 0; i < m_Adx.m_vElementSet.size(); i++) {
			ListInsertElementSet(i,*(m_Adx.m_vElementSet[i]));
		}

		ControlEnable(true);

		AfxMessageBox(_T("ADXファイル読み込み完了"));
	}
	else {

		ControlEnable(false);

		AfxMessageBox(_T("ADXファイル読み込めませんでした"));
	}


	CString buf;

	buf.Format(_T("-------------------------------------------------------\n"));
	m_LogFile.WriteString(buf);

	buf.Format(_T("ADX_FILE読み込みました:%s\n"), m_PathAdx);
	m_LogFile.WriteString(buf);

	buf.Format(_T("-------------------------------------------------------\n"));
	m_LogFile.WriteString(buf);

}

void CVectorMappingDlg::ControlEnable(bool to_enable)
{
	CWnd* myWnd;

	/*
	myWnd = GetDlgItem(IDC_EDIT_NASTRAN);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_BUTTON_NASTRAN);
	myWnd->EnableWindow(to_enable);

	myWnd = GetDlgItem(IDC_EDIT_NORMAL);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_BUTTON_NORMAL);
	myWnd->EnableWindow(to_enable);

	myWnd = GetDlgItem(IDC_EDIT_TANGENT);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_BUTTON_TANGENT);
	myWnd->EnableWindow(to_enable);

	myWnd = GetDlgItem(IDC_EDIT_UPR_LIMIT);
	myWnd->EnableWindow(to_enable);

	*/

	myWnd = GetDlgItem(IDC_EDIT_OUT_FORCE);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_BUTTON_FORCE);
	myWnd->EnableWindow(to_enable);

	myWnd = GetDlgItem(IDC_BUTTON_CLEAR_SELECT);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_LIST_ADX);
	myWnd->EnableWindow(to_enable);
	myWnd = GetDlgItem(IDC_BUTTON_EXEC);
	myWnd->EnableWindow(to_enable);
}

void CVectorMappingDlg::OnBnClickedButtonExec()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	//　初期化
	m_Nastran.RemoveAll();
	m_SurfaceNode.RemoveAll();

	m_Nastran.m_LogFile = &m_LogFile;
	m_SurfaceNode.m_LogFile = &m_LogFile;

	if (m_Nastran.ReadNastranFile(m_PathNastran) != 0) {
		AfxMessageBox(_T("Nastranファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ReadNormalVectorFile(m_PathNormalPressure) != 0) {
		AfxMessageBox(_T("NormalPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ReadTangentVectorFile(m_PathTangentPressure) != 0) {
		AfxMessageBox(_T("TangentPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (m_Nastran.ForceCalc() != 0) {
		AfxMessageBox(_T("Nastranファイルの荷重を計算できませんでした"));
		return;
	}

	// マッピングするパーツを取得
	//std::vector<int> v_sel;
	CStringArray v_name;

	/*
	for (int i = 0; i<m_ListElementSet.GetCount(); i++) {
		if (m_ListElementSet.GetSel(i)) {
			CString& name = (m_Adx.m_vElementSet[i])->m_NameAdx;
			v_name.Add(name);
		}
	}
	*/

	// マッピングするパーツを取得
	int  no_chk = 0;
	HWND hwndList = GetDlgItem(IDC_LIST_PARTS)->m_hWnd;
	BOOL CheckFlag = FALSE;	/*チェックがついているか*/
	CAdxElementSet *tmp_es;

	int nCount = m_ListParts.GetItemCount();	/*m_InfoListの項目数*/

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CheckFlag = ListView_GetCheckState(hwndList, nItem);
		if (CheckFlag == TRUE)
		{
			no_chk++;
			tmp_es = (CAdxElementSet*)(m_ListParts.GetItemData(nItem));
			v_name.Add(tmp_es->m_NameAdx);
		}
	}

	// Adxファイルの表面ノードを取得
	if (m_Adx.ExtractSurfaceNode(v_name, m_SurfaceNode) != 0) {
		AfxMessageBox(_T("表面ノードを取得できませんでした"));
		return;
	}
	else {
		// AfxMessageBox(_T("表面ノードを取得できました"));
	}

	// エリアマップ
	if (m_SurfaceNode.MakeAreaMap() != 0) {
		AfxMessageBox(_T("ノードマップ作成エラーです"));
		return;
	}

	// マッピング
	CMzPoint ratio;

	if (m_RatioIsXYZ) {
		ratio.x = m_MappingRatio_X;
		ratio.y = m_MappingRatio_Y;
		ratio.z = m_MappingRatio_Z;
	}
	else {
		ratio.x = m_MappingRatio;
		ratio.y = m_MappingRatio;
		ratio.z = m_MappingRatio;
	}

	if (m_SurfaceNode.Mapping(m_Nastran, m_SearchDistance, ratio) != 0) {
		AfxMessageBox(_T("マッピング実行時エラーです"));
		return;
	}

	// Forceデータ出力
	CString process = _T("AdvcStep-99999");
	if (m_SurfaceNode.ExportAdxForce(m_PathForce, process) != 0) {
		AfxMessageBox(_T("Forceファイル出力ができませんでした"));
	}

	// Dump(Debug用)

	// PAM-STAMP ノード
	CString n_path;
	CString mod_str = _T("_N.csv");
	ModFileName(m_PathNastran, '.', mod_str, n_path);
	m_Nastran.Dump_N(n_path);

	// PAM-STAMP 要素
	CString e_path;
	mod_str = _T("_E.csv");
	ModFileName(m_PathNastran, '.', mod_str, e_path);
	m_Nastran.Dump_E(e_path);

	// マッピング後データ
	CString m_path;
	mod_str = _T("_map.csv");
	ModFileName(m_PathForce, '.', mod_str, m_path);
	m_SurfaceNode.DumpNode(m_path);

	CString buf;

	buf.Format(_T("-------------------------------------------------------\n"));
	m_LogFile.WriteString(buf);

	buf.Format(_T("NASTRAN FILE:%s\n"), m_PathNastran);
	m_LogFile.WriteString(buf);

	buf.Format(_T("PAM_STAMP NormalVectorFile :%s\n"), m_PathNormalPressure);
	m_LogFile.WriteString(buf);

	buf.Format(_T("PAM_STAMP TangentVectorFile:%s\n"), m_PathTangentPressure);
	m_LogFile.WriteString(buf);

	buf.Format(_T("Adx Force Mapping File     :%s\n"), m_PathForce);
	m_LogFile.WriteString(buf);

	buf.Format(_T("マッピング時 係数          : X(%8.3f) Y(%8.3f) Z(%8.3f)\n"), ratio.x,ratio.y,ratio.z);
	m_LogFile.WriteString(buf);

	CMzPoint nastran_force;

	m_Nastran.GetTotalForce(nastran_force);
	buf.Format(_T("NASTRAN_荷重(kN) (元)           :%10.3f,%10.3f,%10.3f\n"), nastran_force.x/1000.0, nastran_force.y/1000.0, nastran_force.z/1000.0);
	m_LogFile.WriteString(buf);

	buf.Format(_T("NASTRAN_荷重(kN) (係数処理後)   :%10.3f,%10.3f,%10.3f\n"), nastran_force.x / 1000.0*ratio.x, nastran_force.y / 1000.0*ratio.y, nastran_force.z / 1000.0*ratio.z);
	m_LogFile.WriteString(buf);

	buf.Format(_T("マッピングされた荷重(kN)        :%10.3f,%10.3f,%10.3f\n"), m_SurfaceNode.m_MappedForce.x/1000.0, m_SurfaceNode.m_MappedForce.y/1000.0, m_SurfaceNode.m_MappedForce.z/1000.0);
	m_LogFile.WriteString(buf);

	buf.Format(_T("マッピングされなかった荷重(kN)  :%10.3f,%10.3f,%10.3f\n"), m_SurfaceNode.m_LossForce.x/1000.0, m_SurfaceNode.m_LossForce.y/1000.0, m_SurfaceNode.m_LossForce.z/1000.0);
	m_LogFile.WriteString(buf);

	buf.Format(_T("-------------------------------------------------------\n"));
	m_LogFile.WriteString(buf);

	m_LogFile.Flush();

	m_Nastran.RemoveAll();
	m_SurfaceNode.RemoveAll();

	AfxMessageBox(_T("マッピング完了しました"));
}

void CVectorMappingDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if ( m_LogIsOpen ) {
		m_LogFile.Close();
	}

	CDialogEx::OnCancel();
}


void CVectorMappingDlg::OnLbnSelchangeListAdx()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int sel_count = 0;
	for (int i = 0; i<m_ListElementSet.GetCount(); i++) {
		if (m_ListElementSet.GetSel(i)) {
			sel_count++;
		}
	}

	// 
	CString sel_string;
	sel_string.Format(_T("%d"), sel_count);
	m_str_selcount = sel_string;

	UpdateData(FALSE);
}


void CVectorMappingDlg::OnBnClickedButtonClearSelect()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	for (int i = 0; i < m_ListElementSet.GetCount(); i++) {
		m_ListElementSet.SetSel(i, FALSE);
	}

	HWND hwndList = GetDlgItem(IDC_LIST_PARTS)->m_hWnd;

	for (int i = 0; i < m_ListParts.GetItemCount(); i++) {
		ListView_SetCheckState(hwndList, i,0);
	}

	m_str_selcount = "0";
	UpdateData(FALSE);

}


void CVectorMappingDlg::OnBnClickedButtonForceCalc()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CNastran nas_test;

	if (nas_test.ReadNastranFile(m_PathNastran) != 0) {
		AfxMessageBox(_T("Nastranファイル読み込めませんでした"));
		return;
	}

	if (nas_test.ReadNormalVectorFile(m_PathNormalPressure) != 0) {
		AfxMessageBox(_T("NormalPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (nas_test.ReadTangentVectorFile(m_PathTangentPressure) != 0) {
		AfxMessageBox(_T("TangentPressureVectorファイル読み込めませんでした"));
		return;
	}

	if (nas_test.ForceCalc() != 0) {
		AfxMessageBox(_T("Nastranファイルの荷重を計算できませんでした"));
		return;
	}

	CMzPoint pam_force;

	nas_test.GetTotalForce(pam_force);

	m_StrForceX.Format(_T("%11.3f"), pam_force.x/1000.0);
	m_StrForceY.Format(_T("%11.3f"), pam_force.y/1000.0);
	m_StrForceZ.Format(_T("%11.3f"), pam_force.z/1000.0);
	m_StrForceNorm.Format(_T("%11.3f"), sqrt( pam_force.x*pam_force.x + pam_force.y*pam_force.y + pam_force.z*pam_force.z) / 1000.0);


	UpdateData(FALSE);
}

void CVectorMappingDlg::ListInsertElementSet(int no_row, CAdxElementSet &es)
{
	/*
	m_ListParts.InsertItem(0, _T("A"));
	m_ListParts.SetItemText(0, 1, L"1");
	m_ListParts.SetItemText(0, 2, L"3");
	*/

	CString sid;
	sid.Format(_T("%d"), es.m_ID);

	//m_ListParts.InsertItem(no_row, es.m_NameAdx);

	int insert_row = m_ListParts.InsertItem(LVIF_TEXT | LVIF_PARAM, no_row, es.m_NameAdx,0, 0, 0, (LPARAM)&(es));

	m_ListParts.SetItemText(insert_row, 1, sid);
	m_ListParts.SetItemText(insert_row, 2, es.m_NameUser);

	//UpdateData(TRUE);
}




void CVectorMappingDlg::OnLvnColumnclickListParts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	// 何列目がクリックされたか取得
	m_sortParam.m_nIndex = pNMLV->iSubItem;

	// ソートを行う
	m_ListParts.SortItems(CompareData, (LPARAM)&m_sortParam);

	// 次回は昇順・降順を逆にする
	m_sortParam.m_bSort = !m_sortParam.m_bSort;


	*pResult = 0;
}

int CVectorMappingDlg::CompareData(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int result = 0;

	CAdxElementSet* d1 = (CAdxElementSet*)lParam1;
	CAdxElementSet* d2 = (CAdxElementSet*)lParam2;

	CSortParameter* sortParam = (CSortParameter*)lParamSort;
	//CSortParameter* sortParam = &m_sortParam;

	if (sortParam->m_nIndex == 0) {
		result = d1->m_NameAdx.Compare(d2->m_NameAdx); 
	}
	else if (sortParam->m_nIndex == 1) {
		if (d1->m_ID > d2->m_ID) {
			result = 1;
		}
		else if (d1->m_ID < d2->m_ID) {
			result = -1;
		}
		else {
			result = 0;
		}
	}
	else if (sortParam->m_nIndex == 2) {
		result = d1->m_NameUser.Compare(d2->m_NameUser);
	}

	if (sortParam->m_bSort) {
		result *= -1;
	}

	return result;
}

void CVectorMappingDlg::OnLvnItemchangedListParts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	// チェック数カウント
	int  no_chk = 0;
	HWND hwndList = GetDlgItem(IDC_LIST_PARTS)->m_hWnd;
	BOOL CheckFlag = FALSE;	/*チェックがついているか*/

	int nCount = m_ListParts.GetItemCount();	/*m_InfoListの項目数*/

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CheckFlag = ListView_GetCheckState(hwndList, nItem);
		if (CheckFlag == TRUE)
		{
			no_chk++;
		}
	}

	// 
	CString sel_string;
	sel_string.Format(_T("%d"), no_chk);
	m_str_selcount = sel_string;

	UpdateData(FALSE);

	*pResult = 0;
}


void CVectorMappingDlg::OnBnClickedCheckXyz()
{

	CEdit *e;

	UpdateData(TRUE);

	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (m_RatioIsXYZ) {
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO); e->EnableWindow(FALSE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_X); e->EnableWindow(TRUE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_Y); e->EnableWindow(TRUE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_Z); e->EnableWindow(TRUE);
	}
	else {
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO); e->EnableWindow(TRUE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_X); e->EnableWindow(FALSE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_Y); e->EnableWindow(FALSE);
		e = (CEdit*)GetDlgItem(IDC_EDIT_MAPPING_RATIO_Z); e->EnableWindow(FALSE);
	}
}


void CVectorMappingDlg::OnEnKillfocusEditNastran()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditNormal()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditTangent()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditOutForce()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditMappingRatio()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditUprLimit()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditMappingRatioX()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditMappingRatioY()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}


void CVectorMappingDlg::OnEnKillfocusEditMappingRatioZ()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData(TRUE);
}