// CRightPaneDlg.cpp: 구현 파일
//


#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CRightPaneDlg.h"
#include <iostream>

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
END_MESSAGE_MAP()

BOOL CRightPaneDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect client;
	GetClientRect(&client);

	const int totalHeight = client.Height();  // == 900
	const int width = 684;

	const int checkBoxHeight = 70;
	const int launchHeight = 150;
	const int targetHeight = 385;
	const int missileHeight = 385;

	int y = checkBoxHeight;

	// 발사통제기
	m_launchDlg.Create(IDD_LC_INFO_DLG, this);
	m_launchDlg.MoveWindow(0, 0, width, launchHeight);

	// 표적
	m_targetDlg.Create(IDD_TARGET_INFO_DLG, this);
	m_targetDlg.MoveWindow(0, launchHeight, width, targetHeight);

	// 유도탄
	m_missileDlg.Create(IDD_MISSILE_INFO_DLG, this);
	m_missileDlg.MoveWindow(0, launchHeight + targetHeight, width, missileHeight);

	m_launchDlg.ShowWindow(SW_SHOW);
	m_targetDlg.ShowWindow(SW_SHOW);
	m_missileDlg.ShowWindow(SW_SHOW);


	return TRUE;
}


// CRightPaneDlg.cpp
void CRightPaneDlg::SetLCStatus(const LCStatus& status) {
	m_launchDlg.SetLCStatus(status);
}

void CRightPaneDlg::SetMissileStatus(const std::vector<MissileStatus>& missiles)
{
	if (!missiles.empty())
	{
		m_missileDlg.SetMissileStatus(missiles[0]);  // 가장 첫 번째 미사일 정보 표시
	}
}

void CRightPaneDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	m_targetList = targets;
	m_targetDlg.SetTargetList(targets);
}
