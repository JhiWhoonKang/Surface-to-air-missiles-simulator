// CRightPaneDlg.cpp: 구현 파일
//


#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CRightPaneDlg.h"


// CRightPaneDlg 대화 상자

IMPLEMENT_DYNAMIC(CRightPaneDlg, CDialogEx)

CRightPaneDlg::CRightPaneDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RIGHT_PANE_DLG, pParent)
{

}

CRightPaneDlg::~CRightPaneDlg()
{

}

void CRightPaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRightPaneDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CRightPaneDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_launchDlg.Create(IDD_LC_INFO_DLG, this);
	m_targetDlg.Create(IDD_TARGET_INFO_DLG, this);
	m_missileDlg.Create(IDD_MISSILE_INFO_DLG, this);

	m_launchDlg.ShowWindow(SW_SHOW);
	m_targetDlg.ShowWindow(SW_SHOW);
	m_missileDlg.ShowWindow(SW_SHOW);

	return TRUE;
}

void CRightPaneDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (!m_launchDlg.GetSafeHwnd()) return;

	// 전체 높이 기준
	int unitHeight = cy / 5;

	int launchHeight = unitHeight * 1.5;
	int infoHeight = unitHeight * 3.5;	// target + missile (세로 크기 동일)
	int infoY = launchHeight;
	int infoWidth = cx / 2;				// 가로 반으로 나눔

	// 1. Launch Control (전체 너비, 1/5 높이)
	m_launchDlg.MoveWindow(0, 0, cx, launchHeight);

	// 2. Target (왼쪽 절반, 2/5 높이)
	m_targetDlg.MoveWindow(0, infoY, infoWidth, infoHeight);

	// 3. Missile (오른쪽 절반, 2/5 높이)
	m_missileDlg.MoveWindow(infoWidth, infoY, infoWidth, infoHeight);
}

void CRightPaneDlg::OnOK() {}
void CRightPaneDlg::OnCancel() {}

BOOL CRightPaneDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_NCLBUTTONDOWN && pMsg->wParam == HTCAPTION)
	{
		return TRUE;
	}		
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CRightPaneDlg::OnClose()
{
}

// CRightPaneDlg.cpp
void CRightPaneDlg::SetLCStatus(const LCStatus& status)
{
	m_launchDlg.SetLCStatus(status);
}

void CRightPaneDlg::SetMissileStatus(const std::vector<MissileStatus>& missiles)
{
	m_missileDlg.SetMissileList(missiles);
}

void CRightPaneDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	m_targetList = targets;
	m_targetDlg.SetTargetList(targets);
}