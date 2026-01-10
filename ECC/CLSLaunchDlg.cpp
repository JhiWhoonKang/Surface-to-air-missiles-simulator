#include "pch.h"
#include "afxdialogex.h"
#include "CLSLaunchDlg.h"
#include "resource.h"
#include <iostream>
IMPLEMENT_DYNAMIC(CLSLaunchDlg, CDialogEx)

CLSLaunchDlg::CLSLaunchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LS_LAUNCH_DLG, pParent)
{
}

CLSLaunchDlg::~CLSLaunchDlg()
{
}

void CLSLaunchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LS_TARGET, m_comboTargetID);  // 콤보박스와 연결
}

BEGIN_MESSAGE_MAP(CLSLaunchDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_LS_TARGET, &CLSLaunchDlg::OnCbnSelchangeComboLsTarget)
	ON_BN_CLICKED(IDC_BUTTON_LS_LAUNCH, &CLSLaunchDlg::OnBnClickedButtonLsLaunch)
END_MESSAGE_MAP()

BOOL CLSLaunchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ✅ 고정된 "0000" 타겟을 콤보박스에 가장 먼저 추가
	

	// ✅ 서버에서 전달받은 표적 리스트 삽입
	for (const auto& target : m_targetList)
	{
		CString str;
		str.Format(_T("%d"), target.id);
		m_comboTargetID.AddString(str);
	}

	// ✅ 초기 선택을 첫 번째 항목("0000")으로 설정
	m_comboTargetID.SetCurSel(0);

	return TRUE;
}


void CLSLaunchDlg::OnCbnSelchangeComboLsTarget()
{
	// 선택된 표적 ID 갱신
	int sel = m_comboTargetID.GetCurSel();
	if (sel != CB_ERR) {
		CString str;
		m_comboTargetID.GetLBText(sel, str);
		m_selectedTargetID = _ttoi(str);
	}
}

void CLSLaunchDlg::OnBnClickedButtonLsLaunch()
{
	int sel = m_comboTargetID.GetCurSel();
	if (sel != CB_ERR)
	{
		CString str;
		m_comboTargetID.GetLBText(sel, str);
		m_selectedTargetID = _ttoi(str);

		if (m_parent)
		{
			m_parent->SetGoalTargetId(m_selectedTargetID);
			m_parent->sendMissileLaunch(
				m_lsId,
				m_selectedTargetID
			);
			//std::cout << "[발사 요청] 발사대 ID=" << m_lsId << ", 표적 ID=" << m_selectedTargetID << "\n";
		}

		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox(_T("표적을 선택하세요."));
	}
}

void CLSLaunchDlg::SetTargetList(const std::vector<TargetStatus>& targets) {
	m_targetList = targets;
}

void CLSLaunchDlg::SetLauncherID(int id) {
	m_lsId = id;
}

void CLSLaunchDlg::SetParent(CSAMtestDlg* parent) {
	m_parent = parent;
}

int CLSLaunchDlg::GetSelectedTargetID() const {
	return m_selectedTargetID;
}
