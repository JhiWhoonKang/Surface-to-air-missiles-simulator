// CLeftBottomDlg.cpp: 구현 파일

#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CLeftBottomDlg.h"
#include "CLSMoveDlg.h"
#include "CLSLaunchDlg.h"
#include "SAMtestDlg.h"  // 부모 다이얼로그 포인터 호출용

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
	DDX_Control(pDX, IDC_STATIC_NETWORK_LS, m_staticNetworkLS); // ✅ 연결 상태 원 Picture Control
}

BEGIN_MESSAGE_MAP(CLeftBottomDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_LS_ID, &CLeftBottomDlg::OnCbnSelchangeComboLsId)
	ON_BN_CLICKED(IDC_BTN_MODE_SWITCH, &CLeftBottomDlg::OnBnClickedBtnModeSwitch)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, &CLeftBottomDlg::OnBnClickedButtonMove)
	ON_BN_CLICKED(IDC_BUTTON_LAUNCH, &CLeftBottomDlg::OnBnClickedButtonLaunch)
	ON_STN_CLICKED(IDC_STATIC_NETWORK_LS, &CLeftBottomDlg::OnStnClickedStaticNetworkLs)
	ON_WM_DRAWITEM()  // ✅ Picture Control 그리기 메시지 처리
	ON_STN_CLICKED(IDC_STATIC_LS_POSITION3, &CLeftBottomDlg::OnStnClickedStaticLsPosition3)
END_MESSAGE_MAP()

BOOL CLeftBottomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateLSNetworkIndicator(false); // 시작은 연결 안된 상태
	return TRUE;
}

void CLeftBottomDlg::SetParentDlg(CSAMtestDlg* parent)
{
	m_parent = parent;
}

void CLeftBottomDlg::SetLSList(const std::vector<LSStatus>& list)
{
	m_lsList = list;
	m_comboLSID.ResetContent();

	for (const auto& ls : m_lsList) {
		CString idStr;
		idStr.Format(_T("%d"), ls.id);
		m_comboLSID.AddString(idStr);
	}

	if (!m_lsList.empty()) {
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
	if (sel == CB_ERR || sel >= static_cast<int>(m_lsList.size())) {
		UpdateLSNetworkIndicator(false);
		return;
	}

	CString strID;
	m_comboLSID.GetLBText(sel, strID);
	int id = _ttoi(strID);

	for (const auto& ls : m_lsList) {
		if (ls.id == id) {
			m_currentLSStatus = ls;
			SetLSUI(m_currentLSStatus);
			UpdateLSModeUI();
			UpdateLSNetworkIndicator(true); // ✅ 연결된 상태로 변경
			break;
		}
	}
}

void CLeftBottomDlg::SetLSUI(const LSStatus& status)
{
	double posX = static_cast<double>(status.position.x) / 10000000.0;
	double posY = static_cast<double>(status.position.y) / 10000000.0;
	long posZ = static_cast<double>(status.position.z);
	CString modeStr;
	switch (status.mode) {
	case LSStatus::STOP: modeStr = _T("정지"); break;
	case LSStatus::WAR:  modeStr = _T("교전"); break;
	case LSStatus::MOVE: modeStr = _T("이동"); break;
	default:             modeStr = _T("알수없음"); break;
	}

	GetDlgItem(IDC_STATIC_LS_MODE)->SetWindowText(modeStr);

	CString angleStr;
	angleStr.Format(_T("%.1f"), status.angle);
	GetDlgItem(IDC_STATIC_LS_ANGLE)->SetWindowText(angleStr);

	CString xStr, yStr, zStr;
	xStr.Format(_T("%.8f"), posX);
	yStr.Format(_T("%.8f"), posY);
	zStr.Format(_T("%ld"), posZ);
	GetDlgItem(IDC_STATIC_LS_POSITION)->SetWindowText(xStr);
	GetDlgItem(IDC_STATIC_LS_POSITION2)->SetWindowText(yStr);
	GetDlgItem(IDC_STATIC_LS_POSITION3)->SetWindowText(zStr);

	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(status.mode == LSStatus::WAR);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(status.mode == LSStatus::STOP);
	GetDlgItem(IDC_BTN_MODE_SWITCH)->EnableWindow(status.mode != LSStatus::MOVE);
}

void CLeftBottomDlg::UpdateLSModeUI()
{
	auto mode = m_currentLSStatus.mode;

	GetDlgItem(IDC_BTN_MODE_SWITCH)->EnableWindow(mode == 0 || mode == 2);
	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(mode == 0);
	GetDlgItem(IDC_BUTTON_MOVE)->EnableWindow(mode == 2);
}

void CLeftBottomDlg::OnLSModeChangeAck(uint8_t lsId, uint8_t newMode)
{
	if (m_currentLSStatus.id == lsId) {
		m_currentLSStatus.mode = newMode;
		SetLSUI(m_currentLSStatus);
		UpdateLSModeUI();
	}
}

void CLeftBottomDlg::OnLSModeChangeSuccess(uint8_t lsId, uint8_t newMode)
{
	if (m_currentLSStatus.id != lsId) return;
	m_currentLSStatus.mode = newMode;
	SetLSUI(m_currentLSStatus);
}

void CLeftBottomDlg::OnCbnSelchangeComboLsId()
{
	UpdateLSStatusFromSelection();
}

void CLeftBottomDlg::OnBnClickedBtnModeSwitch()
{
	unsigned int lsId = m_currentLSStatus.id; 

	if (m_currentLSStatus.mode == LSStatus::STOP)
	{
		if (m_parent)
			m_parent->sendLSModeChange(lsId, LSStatus::WAR);
	}
	else if (m_currentLSStatus.mode == LSStatus::WAR)
	{
		if (m_parent)
			m_parent->sendLSModeChange(lsId, LSStatus::STOP);
	}
}


void CLeftBottomDlg::OnBnClickedButtonMove()
{
	CLSMoveDlg dlg;
	dlg.SetLauncherID(m_currentLSStatus.id);
	dlg.SetParent(m_parent);
	if (dlg.DoModal() == IDOK)
	{
		AfxMessageBox(_T("발사대 이동"));
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
		AfxMessageBox(_T("미사일 발사"));
	}
}

void CLeftBottomDlg::OnStnClickedStaticNetworkLs()
{
	// 클릭 시 동작 없음
}

void CLeftBottomDlg::UpdateLSNetworkIndicator(bool isConnected)
{
	m_isLSConnected = isConnected;
	m_staticNetworkLS.Invalidate();  // 다시 그리기 요청
}

void CLeftBottomDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_STATIC_NETWORK_LS)
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;

		dc.FillSolidRect(&rect, ::GetSysColor(COLOR_BTNFACE));
		CBrush brush(m_isLSConnected ? RGB(0, 200, 0) : RGB(200, 0, 0));
		dc.SelectObject(&brush);
		dc.Ellipse(rect);

		dc.Detach();
	}
}


void CLeftBottomDlg::OnStnClickedStaticLsPosition3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
