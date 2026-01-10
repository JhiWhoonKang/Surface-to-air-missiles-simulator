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

	m_comboTargetID.AddString(_T("0000"));
	m_comboTargetID.AddString(_T("0000"));

	for (const auto& target : m_targetList)
	{
		CString str;
		str.Format(_T("%d"), target.id);
		m_comboTargetID.AddString(str);
	}
	m_comboTargetID.SetCurSel(0);

	return TRUE;
}

void CLSLaunchDlg::OnCbnSelchangeComboLsTarget()
{
	// 선택된 표적 ID 갱신
	int sel = m_comboTargetID.GetCurSel();
	if (sel != CB_ERR)
	{
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
		std::cout << "[ls] selsected id: " << m_selectedTargetID << std::endl;
		if (m_parent)
		{
			m_parent->setGoalTargetId(m_selectedTargetID);

			std::cout << "[OnBnClickedButtonLsLaunch] if (m_parent) " << m_selectedTargetID << std::endl;

			m_parent->sendMissileLaunch(
				m_lsId,
				m_selectedTargetID
			);
		}

		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox(_T("표적을 선택하세요."));
	}
}

void CLSLaunchDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	m_targetList = targets;
}

void CLSLaunchDlg::SetLauncherID(int id)
{
	m_lsId = id;
}

void CLSLaunchDlg::SetParent(CSAMtestDlg* parent)
{
	m_parent = parent;
}

int CLSLaunchDlg::GetSelectedTargetID() const
{
	return m_selectedTargetID;
}