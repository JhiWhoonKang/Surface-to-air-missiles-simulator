#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CLaunchControlDlg.h"
#include <iostream>

IMPLEMENT_DYNAMIC(CLaunchControlDlg, CDialogEx)

CLaunchControlDlg::CLaunchControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LC_INFO_DLG, pParent)
{
}

CLaunchControlDlg::~CLaunchControlDlg()
{
}

void CLaunchControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 컨트롤 연결
	DDX_Control(pDX, IDC_LIST_LC_INFO, lc_LcInfo);
}

BEGIN_MESSAGE_MAP(CLaunchControlDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CLaunchControlDlg::SetLCStatus(const LCStatus& status)
{
	m_lcStatus = status;
	UpdateUI();
}

void CLaunchControlDlg::SetLcConnected(bool connected)
{
	isLcConnected = connected;
	UpdateUI();
}

void CLaunchControlDlg::UpdateUI()
{
	CString str;
	str.Format(_T("%d"), m_lcStatus.id);
	lc_LcInfo.SetItemText(0, 0, str);

	str.Format(_T("%.8f"), static_cast<double>(m_lcStatus.position.x) / coordsScale);
	lc_LcInfo.SetItemText(0, 1, str);

	str.Format(_T("%.8f"), static_cast<double>(m_lcStatus.position.y) / coordsScale);
	lc_LcInfo.SetItemText(0, 2, str);

	str.Format(_T("%ld"), m_lcStatus.position.z);
	lc_LcInfo.SetItemText(0, 3, str);

	str = "Connected";
	lc_LcInfo.SetItemText(0, 4, str);
}


BOOL CLaunchControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rcClient;
	GetClientRect(&rcClient);

	lc_LcInfo.MoveWindow(0, 0, rcClient.Width(), rcClient.Height());
	lc_LcInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	lc_LcInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 100);
	lc_LcInfo.InsertColumn(1, _T("위도"), LVCFMT_LEFT, 150);
	lc_LcInfo.InsertColumn(2, _T("경도"), LVCFMT_LEFT, 150);
	lc_LcInfo.InsertColumn(3, _T("고도"), LVCFMT_LEFT, 150);
	lc_LcInfo.InsertColumn(4, _T("네트워크 연결 상태"), LVCFMT_LEFT, 150);

	lc_LcInfo.InsertItem(0, _T("")); // Row 0
	lc_LcInfo.InsertItem(1, _T("")); // Row 1
	lc_LcInfo.InsertItem(2, _T("")); // Row 2
	lc_LcInfo.InsertItem(3, _T("")); // Row 3

	return TRUE;
}


void CLaunchControlDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (lc_LcInfo.GetSafeHwnd())
	{
		lc_LcInfo.MoveWindow(0, 0, cx, cy);
	}
}