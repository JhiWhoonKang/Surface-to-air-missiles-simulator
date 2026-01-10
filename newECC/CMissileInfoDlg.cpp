#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CMissileInfoDlg.h"

IMPLEMENT_DYNAMIC(CMissileInfoDlg, CDialogEx)

CMissileInfoDlg::CMissileInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MISSILE_INFO_DLG, pParent)
{
}

CMissileInfoDlg::~CMissileInfoDlg()
{
}

void CMissileInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MISSILE_INFO, lc_MissileInfo);
	DDX_Control(pDX, IDC_COMBO1, cb_MissileInfo);
}

BEGIN_MESSAGE_MAP(CMissileInfoDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMissileInfoDlg::OnCbnSelchangeComboMissileId)
END_MESSAGE_MAP()

BOOL CMissileInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 위치 조정
	CRect rcClient;
	GetClientRect(&rcClient);
	const int comboHeight = 30;
	const int margin = 5;

	cb_MissileInfo.MoveWindow(0, 0, rcClient.Width(), comboHeight);
	lc_MissileInfo.MoveWindow(0, comboHeight + margin, rcClient.Width(), rcClient.Height() - comboHeight - margin);

	// 리스트 스타일 및 헤더 구성
	lc_MissileInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lc_MissileInfo.InsertColumn(0, _T("항목"), LVCFMT_LEFT, 100);
	lc_MissileInfo.InsertColumn(1, _T("값"), LVCFMT_LEFT, rcClient.Width() - 100);

	LPCTSTR labels[] = { _T("ID"), _T("위도"), _T("경도"), _T("고도"),
						 _T("속도"), _T("각도"), _T("작동 시간") };

	for (int i = 0; i < _countof(labels); ++i)
	{
		lc_MissileInfo.InsertItem(i, labels[i]);
		lc_MissileInfo.SetItemText(i, 1, _T(""));
	}

	return TRUE;
}

void CMissileInfoDlg::UpdateComboBox()
{
	cb_MissileInfo.ResetContent();

	for (const auto& missile : m_missileList)
	{
		CString idStr;
		idStr.Format(_T("%d"), missile.id);
		cb_MissileInfo.AddString(idStr);
	}

	if (!m_missileList.empty())
	{
		cb_MissileInfo.SetCurSel(0);
		m_missileStatus = m_missileList[0];
		UpdateUI();
	}
}

void CMissileInfoDlg::SetMissileStatus(const MissileStatus& status)
{
	m_missileStatus = status;
	UpdateUI();
}

void CMissileInfoDlg::SetMissileList(const std::vector<MissileStatus>& list)
{
	m_missileList = list;

	cb_MissileInfo.ResetContent();
	for (const auto& missile : m_missileList)
	{
		CString idStr;
		idStr.Format(_T("%d"), missile.id);
		cb_MissileInfo.AddString(idStr);
	}

	if (!m_missileList.empty())
	{
		cb_MissileInfo.SetCurSel(0);
		m_missileStatus = m_missileList[0];
		UpdateUI();
	}
}

void CMissileInfoDlg::OnCbnSelchangeComboMissileId()
{
	int sel = cb_MissileInfo.GetCurSel();
	if (sel >= 0 && sel < static_cast<int>(m_missileList.size()))
	{
		m_missileStatus = m_missileList[sel];
		UpdateUI();
	}
}

void CMissileInfoDlg::UpdateUI()
{
	CString str;

	str.Format(_T("%d"), m_missileStatus.id);
	lc_MissileInfo.SetItemText(0, 1, str);

	str.Format(_T("%.8f"), static_cast<double>(m_missileStatus.position.x) / 1e8);
	lc_MissileInfo.SetItemText(1, 1, str);

	str.Format(_T("%.8f"), static_cast<double>(m_missileStatus.position.y) / 1e8);
	lc_MissileInfo.SetItemText(2, 1, str);

	str.Format(_T("%lld"), m_missileStatus.position.z);
	lc_MissileInfo.SetItemText(3, 1, str);

	str.Format(_T("%d"), m_missileStatus.speed);
	lc_MissileInfo.SetItemText(4, 1, str);

	str.Format(_T("%.1f"), m_missileStatus.angle);
	lc_MissileInfo.SetItemText(5, 1, str);

	str.Format(_T("%lld"), m_missileStatus.predicted_time);
	lc_MissileInfo.SetItemText(6, 1, str);
}

void CMissileInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (cb_MissileInfo.GetSafeHwnd() && lc_MissileInfo.GetSafeHwnd())
	{
		const int comboHeight = 30;
		const int margin = 5;

		cb_MissileInfo.MoveWindow(0, 0, cx, comboHeight);
		lc_MissileInfo.MoveWindow(0, comboHeight + margin, cx, cy - comboHeight - margin);

		if (lc_MissileInfo.GetHeaderCtrl())
		{
			lc_MissileInfo.SetColumnWidth(0, 100);           // 항목
			lc_MissileInfo.SetColumnWidth(1, cx - 100 - 20);  // 값
		}
	}
}
