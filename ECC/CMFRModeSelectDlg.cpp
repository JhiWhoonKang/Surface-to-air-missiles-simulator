#include "pch.h"
#include "afxdialogex.h"
#include "CMFRModeSelectDlg.h"
#include "resource.h"
#include "SAMtestDlg.h"
#include <iostream>
IMPLEMENT_DYNAMIC(CMFRModeSelectDlg, CDialogEx)

CMFRModeSelectDlg::CMFRModeSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFRMODE_SELECT_DLG, pParent)
{
}

CMFRModeSelectDlg::~CMFRModeSelectDlg()
{
}

void CMFRModeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MFR_TARGET, m_comboTargetID);  // ✅ 콤보박스 컨트롤 연결
}

BEGIN_MESSAGE_MAP(CMFRModeSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MFRMODE_CHANGE, &CMFRModeSelectDlg::OnBnClickedButtonMfrmodeChange)
	ON_CBN_SELCHANGE(IDC_COMBO_MFR_TARGET, &CMFRModeSelectDlg::OnCbnSelchangeComboMfrTarget)
END_MESSAGE_MAP()

// ✅ 다이얼로그 초기화 시 콤보박스 데이터 설정
BOOL CMFRModeSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ✅ 고정 타겟 ID "0000"을 가장 위에 추가
	

	// ✅ 서버에서 전달받은 표적 목록을 콤보박스에 출력
	for (const auto& target : m_targetList) {
		CString strID;
		strID.Format(_T("%d"), target.id);
		m_comboTargetID.AddString(strID);
	}

	// ✅ 초기 선택을 첫 번째 항목("0000")으로 설정
	m_comboTargetID.SetCurSel(0);

	return TRUE;
}


void CMFRModeSelectDlg::OnBnClickedButtonMfrmodeChange()
{
	unsigned int sel = m_comboTargetID.GetCurSel();
	if (sel != CB_ERR)
	{
		CString str;
		m_comboTargetID.GetLBText(sel, str);
		m_selectedTargetID = _ttoi(str);
		m_modeChanged = true;

		// ✅ 여기서 서버로 명령 전송
		if (m_parent) {
			m_parent->sendRadarModeChange(m_radarId, RadarStatus::STOP, 2, m_selectedTargetID);
			//std::cout << "[모드 전환] 레이더 ID=" << (int)m_radarId << ", 정지 요청 (표적 " << m_selectedTargetID << ")\n";
		}

		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox(_T("표적을 선택하세요."));
	}
}


void CMFRModeSelectDlg::OnCbnSelchangeComboMfrTarget()
{
	// 선택 시 처리할 로직이 있다면 여기에 작성
}
