#include "pch.h"
#include "CLeftTopDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CMFRModeSelectDlg.h"
#include "SAMtestDlg.h"
#include <iostream>

IMPLEMENT_DYNAMIC(CLeftTopDlg, CDialogEx)

CLeftTopDlg::CLeftTopDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LEFT_TOP_DLG, pParent)
{
}

CLeftTopDlg::~CLeftTopDlg()
{
}

void CLeftTopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RADAR_ID, m_comboRadarID);
	DDX_Control(pDX, IDC_STATIC_NETWORK_MFR, m_staticNetworkMfr);  // ✅ 새로운 Picture Control
}

BEGIN_MESSAGE_MAP(CLeftTopDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MODE_SWITCH, &CLeftTopDlg::OnBnClickedBtnModeSwitch)
	ON_CBN_SELCHANGE(IDC_COMBO_RADAR_ID, &CLeftTopDlg::OnCbnSelchangeComboRadarId)
	ON_STN_CLICKED(IDC_STATIC_MODE, &CLeftTopDlg::OnStnClickedStaticMode)
	ON_STN_CLICKED(IDC_STATIC_NETWORK, &CLeftTopDlg::OnStnClickedStaticNetwork)
	ON_STN_CLICKED(IDC_STATIC_ANGLE, &CLeftTopDlg::OnStnClickedStaticAngle)
	ON_STN_CLICKED(IDC_STATIC_POSITION, &CLeftTopDlg::OnStnClickedStaticPosition)
	ON_STN_CLICKED(IDC_STATIC_MFR_MODE, &CLeftTopDlg::OnStnClickedStaticMfrMode)
	ON_STN_CLICKED(IDC_STATIC_MFR_POSITION2, &CLeftTopDlg::OnStnClickedStaticMfrPosition2)
	ON_WM_DRAWITEM()
	ON_STN_CLICKED(IDC_STATIC_NETWORK_MFR, &CLeftTopDlg::OnStnClickedStaticNetworkMfr)
	ON_STN_CLICKED(IDC_STATIC_MFR_POSITION3, &CLeftTopDlg::OnStnClickedStaticMfrPosition3)
END_MESSAGE_MAP()

BOOL CLeftTopDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateNetworkIndicator(false);  // 기본은 연결 안됨
	return TRUE;
}

void CLeftTopDlg::SetParentDlg(CSAMtestDlg* parent)
{
	m_parent = parent;
}

void CLeftTopDlg::SetRadarList(const std::vector<RadarStatus>& radarList)
{
	m_radarList = radarList;
	m_comboRadarID.ResetContent();

	for (const auto& radar : m_radarList) {
		CString str;
		str.Format(_T("%d"), radar.id);
		m_comboRadarID.AddString(str);
	}

	if (!m_radarList.empty()) {
		m_comboRadarID.SetCurSel(0);
		UpdateRadarStatusFromSelection();
	}
	else {
		UpdateNetworkIndicator(false);
	}
}

void CLeftTopDlg::SetTargetList(const std::vector<TargetStatus>& targetList)
{
	m_targetList = targetList;
}

void CLeftTopDlg::OnCbnSelchangeComboRadarId()
{
	UpdateRadarStatusFromSelection();
}

void CLeftTopDlg::UpdateRadarStatusFromSelection()
{
	int sel = m_comboRadarID.GetCurSel();
	if (sel == CB_ERR || sel >= static_cast<int>(m_radarList.size())) {
		UpdateNetworkIndicator(false);
		return;
	}

	CString strID;
	m_comboRadarID.GetLBText(sel, strID);
	int radarID = _ttoi(strID);

	for (const auto& radar : m_radarList) {
		if (radar.id == radarID) {
			m_currentRadarStatus = radar;
			SetRadarUI(radar);
			return;
		}
	}
	UpdateNetworkIndicator(false);  // 해당 ID에 일치하는 레이더가 없을 경우
}

void CLeftTopDlg::SetRadarUI(const RadarStatus& status)
{
	double posX = static_cast<double>(status.position.x) / 10000000.0;
	double posY = static_cast<double>(status.position.y) / 10000000.0;
	double posZ = static_cast<double>(status.position.z) / 10000000.0;
	CString modeStr = (status.mode == RadarStatus::ROTATE) ? _T("회전") : _T("정지");

	CString angleStr;
	angleStr.Format(_T("%.1f"), status.angle);

	CString posXStr, posYStr,posZStr;
	posXStr.Format(_T("%.8f"), posX);
	posYStr.Format(_T("%.8f"), posY);
	posZStr.Format(_T("%.2f"), posZ);

	GetDlgItem(IDC_STATIC_MFR_MODE)->SetWindowText(modeStr);
	GetDlgItem(IDC_STATIC_MFR_ANGLE)->SetWindowText(angleStr);
	GetDlgItem(IDC_STATIC_MFR_POSITION)->SetWindowText(posXStr);
	GetDlgItem(IDC_STATIC_MFR_POSITION2)->SetWindowText(posYStr);
	GetDlgItem(IDC_STATIC_MFR_POSITION3)->SetWindowText(posZStr);

	UpdateNetworkIndicator(true);
}

void CLeftTopDlg::UpdateNetworkIndicator(bool isConnected)
{
	m_isRadarConnected = isConnected;
	m_staticNetworkMfr.Invalidate();  // ✅ 새 Picture Control 다시 그리기
}

void CLeftTopDlg::OnBnClickedBtnModeSwitch()
{
	unsigned int radarId = m_currentRadarStatus.id;

	if (m_currentRadarStatus.mode == RadarStatus::ROTATE)
	{
		CMFRModeSelectDlg dlg;
		dlg.SetTargetList(m_targetList);
		dlg.SetContext(m_parent, static_cast<unsigned int>(m_currentRadarStatus.id));
		if (dlg.DoModal() == IDOK)
		{
			int selectedTargetId = dlg.GetSelectedTargetID();
			// 필요 시 처리
		}
	}
	else if (m_currentRadarStatus.mode == RadarStatus::STOP)
	{
		if (m_parent)
		{
			m_parent->sendRadarModeChange(radarId, RadarStatus::ROTATE, 2, 0);
			//std::cout << "레이더 정지모드로 변환"<<"\n";
		}
	}
}

void CLeftTopDlg::OnRadarModeChangeSuccess(RadarStatus::Mode newMode)
{
	m_currentRadarStatus.mode = newMode;
	SetRadarUI(m_currentRadarStatus);
}

void CLeftTopDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_STATIC_NETWORK_MFR)
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;

		// 배경 초기화
		dc.FillSolidRect(&rect, ::GetSysColor(COLOR_BTNFACE));

		// 연결 상태에 따라 색상 선택
		CBrush brush(m_isRadarConnected ? RGB(0, 200, 0) : RGB(200, 0, 0));
		dc.SelectObject(&brush);

		// 원 그리기
		dc.Ellipse(rect);
		dc.Detach();
	}
}

// 클릭 핸들러 (비워둠)
void CLeftTopDlg::OnStnClickedStaticMode() {}
void CLeftTopDlg::OnStnClickedStaticNetwork() {}
void CLeftTopDlg::OnStnClickedStaticAngle() {}
void CLeftTopDlg::OnStnClickedStaticPosition() {}
void CLeftTopDlg::OnStnClickedStaticMfrMode() {}
void CLeftTopDlg::OnStnClickedStaticMfrPosition2() {}
void CLeftTopDlg::OnStnClickedStaticNetworkMfr() {}


void CLeftTopDlg::OnStnClickedStaticMfrPosition3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
