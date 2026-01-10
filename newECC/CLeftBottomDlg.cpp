// CLeftBottomDlg.cpp: 구현 파일

#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CLeftBottomDlg.h"
#include "CLSMoveDlg.h"
#include "CLSLaunchDlg.h"
#include "SAMtestDlg.h"  // 부모 다이얼로그 포인터 호출용
#include <iostream>

IMPLEMENT_DYNAMIC(CLeftBottomDlg, CDialogEx)

CLeftBottomDlg::CLeftBottomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LEFT_BOTTOM_DLG, pParent)
{
}

CLeftBottomDlg::~CLeftBottomDlg()
{
}

void CLeftBottomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LS_ID, m_comboLSID);            // 콤보박스
	DDX_Control(pDX, IDC_LIST_LAUNCHER_INFO, lc_LauncherInfo);
	DDX_Control(pDX, IDC_BUTTON_LAUNCH, btn_LsLaunch);
	DDX_Control(pDX, IDC_BUTTON_MODE_CHANGE, btn_LsModeChange);
	DDX_Control(pDX, IDC_BUTTON_MOVE, btn_LsMove);
}

BEGIN_MESSAGE_MAP(CLeftBottomDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_LS_ID, &CLeftBottomDlg::OnCbnSelchangeComboLsId)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, &CLeftBottomDlg::OnBnClickedButtonMove)
	ON_BN_CLICKED(IDC_BUTTON_LAUNCH, &CLeftBottomDlg::OnBnClickedButtonLaunch)
	ON_STN_CLICKED(IDC_STATIC_NETWORK_LS, &CLeftBottomDlg::OnStnClickedStaticNetworkLs)
	ON_BN_CLICKED(IDC_BUTTON_MODE_CHANGE, &CLeftBottomDlg::OnBnClickedButtonModeChange)
END_MESSAGE_MAP()

BOOL CLeftBottomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateLSNetworkIndicator(false); // 시작은 연결 안된 상태

	CRect clientRect;
	GetClientRect(&clientRect);

	int width = clientRect.Width();
	int height = clientRect.Height();
	std::cout << "ls width: " << width << std::endl;

	const int margin = 10;
	int listTop = height * 3 / 5;
	int listBottom = height - margin;
	int listHeight = listBottom - listTop;

	int comboHeight = 30;
	const int buttonHeight = 40;
	int spacing = 5;

	// 콤보박스 위치 (상단)
	m_comboLSID.MoveWindow(
		margin,
		margin,
		width - 2 * margin,
		comboHeight
	);

	// 모드 변경 버튼 위치
	int buttonTop = listTop - spacing - buttonHeight;
	btn_LsModeChange.MoveWindow(
		margin,
		buttonTop,
		width / 2 - 2 * margin - spacing,
		buttonHeight
	);

	// 좌표 입력 버튼 위치
	btn_LsMove.MoveWindow(
		margin + spacing + width / 2,
		buttonTop,
		width / 2 - 2 * margin - spacing,
		buttonHeight
	);

	// 발사 버튼 위치
	int buttonLaunchTop = listTop - spacing - buttonHeight * 2;
	btn_LsLaunch.MoveWindow(
		margin,
		buttonLaunchTop,
		width - 2 * margin,
		buttonHeight
	);


	// 리스트 컨트롤 위치 (하단 3/5 ~ 5/5 영역)
	lc_LauncherInfo.MoveWindow(
		margin,               // 좌측 마진
		listTop,              // 시작 y
		width - 2 * margin,   // 너비
		listHeight            // 높이
	);

	lc_LauncherInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lc_LauncherInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 100);
	lc_LauncherInfo.InsertColumn(1, _T("위도"), LVCFMT_LEFT, 130);
	lc_LauncherInfo.InsertColumn(2, _T("경도"), LVCFMT_LEFT, 130);
	lc_LauncherInfo.InsertColumn(3, _T("고도"), LVCFMT_LEFT, 100);
	lc_LauncherInfo.InsertColumn(4, _T("모드"), LVCFMT_LEFT, 100);
	lc_LauncherInfo.InsertColumn(5, _T("회전각"), LVCFMT_LEFT, 100);
	lc_LauncherInfo.InsertColumn(6, _T("네트워크 상태"), LVCFMT_LEFT, 120);

	return TRUE;
}

void CLeftBottomDlg::OnOK() {}
void CLeftBottomDlg::OnCancel() {}

BOOL CLeftBottomDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_NCLBUTTONDOWN && pMsg->wParam == HTCAPTION)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLeftBottomDlg::OnClose()
{
	// Alt+F4 방지
}

void CLeftBottomDlg::SetParentDlg(CSAMtestDlg* parent)
{
	m_parent = parent;
}

void CLeftBottomDlg::SetLSList(const std::vector<LSStatus>& list)
{
	m_lsList = list;
	m_comboLSID.ResetContent();
	lc_LauncherInfo.DeleteAllItems(); // 리스트 초기화

	for (size_t i = 0; i < list.size(); ++i) {
		const auto& ls = list[i];
		CString str;

		// 콤보박스
		str.Format(_T("%d"), ls.id);
		m_comboLSID.AddString(str);

		// 리스트
		lc_LauncherInfo.InsertItem(static_cast<int>(i), str); // ID

		str.Format(_T("%.8f"), static_cast<double>(ls.position.x) / coordsScale);
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 1, str); // 위도

		str.Format(_T("%.8f"), static_cast<double>(ls.position.y) / coordsScale);
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 2, str); // 경도

		str.Format(_T("%.2f"), static_cast<double>(ls.position.z));
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 3, str); // 고도

		switch (ls.mode)
		{
			case LSStatus::STOP: str = _T("정지"); break;
			case LSStatus::WAR:  str = _T("교전"); break;
			case LSStatus::MOVE: str = _T("이동"); break;
			default:             str = _T("알수없음"); break;
		}
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 4, str); // 모드

		str.Format(_T("%.1f"), ls.angle);
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 5, str); // 회전각

		str = _T("Disconnected");
		lc_LauncherInfo.SetItemText(static_cast<int>(i), 6, str); // 연결 상태 (초기값)
	}

	if (!list.empty())
	{
		m_comboLSID.SetCurSel(0);
		UpdateLSStatusFromSelection();
	}
}

void CLeftBottomDlg::SetTargetList(const std::vector<TargetStatus>& targetList)
{
	m_targetList = targetList;
}

void CLeftBottomDlg::UpdateLSStatusFromSelection()
{
	int sel = m_comboLSID.GetCurSel();
	if (sel == CB_ERR || sel >= static_cast<int>(m_lsList.size()))
	{
		UpdateLSNetworkIndicator(false);
		return;
	}

	CString strID;
	m_comboLSID.GetLBText(sel, strID);
	int id = _ttoi(strID);

	for (const auto& ls : m_lsList)
	{
		if (ls.id == id)
		{
			m_currentLSStatus = ls;
			SetLSUI(m_currentLSStatus);
			UpdateLSModeUI();
			UpdateLSNetworkIndicator(true);
			break;
		}
	}
}

void CLeftBottomDlg::SetLSUI(const LSStatus& status)
{
	double posX = static_cast<double>(status.position.x) / 100000000.0;
	double posY = static_cast<double>(status.position.y) / 100000000.0;
	long posZ = static_cast<double>(status.position.z);
	CString modeStr;
	switch (status.mode)
	{
	case LSStatus::STOP: modeStr = _T("정지"); break;
	case LSStatus::WAR:  modeStr = _T("교전"); break;
	case LSStatus::MOVE: modeStr = _T("이동"); break;
	default:             modeStr = _T("알수없음"); break;
	}

	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(status.mode == LSStatus::WAR);
	GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(status.mode == LSStatus::STOP);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(status.mode != LSStatus::MOVE);
}

void CLeftBottomDlg::UpdateLSModeUI()
{
	auto mode = m_currentLSStatus.mode;

	GetDlgItem(IDC_BUTTON_MODE_CHANGE)->EnableWindow(mode == 0 || mode == 2);
	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(mode == 0);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(mode == 2);
}

void CLeftBottomDlg::OnLSModeChangeAck(uint8_t lsId, uint8_t newMode)
{
	if (m_currentLSStatus.id == lsId)
	{
		m_currentLSStatus.mode = newMode;
		SetLSUI(m_currentLSStatus);
		UpdateLSModeUI();
	}
}

void CLeftBottomDlg::OnLSModeChangeSuccess(uint8_t lsId, uint8_t newMode)
{
	if (m_currentLSStatus.id != lsId)
	{
		return;
	}
	m_currentLSStatus.mode = newMode;
	SetLSUI(m_currentLSStatus);
}

void CLeftBottomDlg::OnCbnSelchangeComboLsId()
{
	UpdateLSStatusFromSelection();
}

void CLeftBottomDlg::OnBnClickedButtonMove()
{
	CLSMoveDlg dlg;
	dlg.SetLauncherID(m_currentLSStatus.id);
	dlg.SetParent(m_parent);
	if (dlg.DoModal() == IDOK)
	{
		AfxMessageBox(_T("모드 선택 완료!"));
	}
}

void CLeftBottomDlg::OnBnClickedButtonLaunch()
{
	CLSLaunchDlg dlg;
	dlg.SetTargetList(m_targetList);
	dlg.SetLauncherID(m_currentLSStatus.id);
	dlg.SetParent(m_parent);
	if (dlg.DoModal() == IDOK)
	{
		AfxMessageBox(_T("모드 선택 완료!"));
	}
}

void CLeftBottomDlg::OnStnClickedStaticNetworkLs()
{
	// 클릭 시 동작 없음
}

void CLeftBottomDlg::UpdateLSNetworkIndicator(bool isConnected)
{
	m_isLSConnected = isConnected;
}

void CLeftBottomDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

void CLeftBottomDlg::OnBnClickedButtonModeChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	unsigned int lsId = m_currentLSStatus.id;

	if (m_currentLSStatus.mode == LSStatus::STOP)
	{
		if (m_parent)
		{
			m_parent->sendLSModeChange(lsId, LSStatus::WAR);
		}			
	}
	else if (m_currentLSStatus.mode == LSStatus::WAR)
	{
		if (m_parent)
		{
			m_parent->sendLSModeChange(lsId, LSStatus::STOP);
		}
	}
}
