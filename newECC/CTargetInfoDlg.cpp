#include "pch.h"
#include "SAMtest.h"
#include "CTargetInfoDlg.h"
#include "afxdialogex.h"
#include <algorithm>
IMPLEMENT_DYNAMIC(CTargetInfoDlg, CDialogEx)

CTargetInfoDlg::CTargetInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TARGET_INFO_DLG, pParent)
{
}

CTargetInfoDlg::~CTargetInfoDlg()
{
}

void CTargetInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TARGET_ID, m_comboTargetID);
	DDX_Control(pDX, IDC_STATIC_TARGET_ID2, m_staticID);
	DDX_Control(pDX, IDC_STATIC_TARGET_POSITIONX, m_staticPosX);
	DDX_Control(pDX, IDC_STATIC_TARGET_POSITIONY, m_staticPosY);
	DDX_Control(pDX, IDC_STATIC_TARGET_HEIGHT2, m_staticPosZ);
	DDX_Control(pDX, IDC_STATIC_TARGET_SPEED2, m_staticSpeed);
	DDX_Control(pDX, IDC_STATIC_TARGET_ANGLE2, m_staticAngle1);
	DDX_Control(pDX, IDC_STATIC_TARGET_ANGLE3, m_staticAngle2);
	DDX_Control(pDX, IDC_STATIC_TARGET_PRIORITY2, m_staticPriority);
	DDX_Control(pDX, IDC_STATIC_TARGET_DETECT2, m_staticDetect);
	DDX_Control(pDX, IDC_STATIC_TARGET_HIT2, m_staticHit);
	DDX_Control(pDX, IDC_LIST_TARGET_INFO, lc_TargetInfo);
}

BEGIN_MESSAGE_MAP(CTargetInfoDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET_ID, &CTargetInfoDlg::OnCbnSelchangeComboTargetId)
END_MESSAGE_MAP()

BOOL CTargetInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect clientRect;
	GetClientRect(&clientRect);

	const int comboHeight = 30;
	const int margin = 5;

	// 콤보박스 위치 조정
	m_comboTargetID.MoveWindow(
		0,
		0,
		clientRect.Width(),
		comboHeight
	);

	// 리스트 컨트롤 위치 조정
	lc_TargetInfo.MoveWindow(
		0,
		comboHeight + margin,
		clientRect.Width(),
		clientRect.Height() - comboHeight - margin
	);

	// 리스트 초기화
	lc_TargetInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	lc_TargetInfo.InsertColumn(0, _T("항목"), LVCFMT_LEFT, 150);
	lc_TargetInfo.InsertColumn(1, _T("값"), LVCFMT_LEFT, clientRect.Width() - 150);  // 남은 공간

	lc_TargetInfo.InsertItem(0, _T("ID"));
	lc_TargetInfo.InsertItem(1, _T("위도"));
	lc_TargetInfo.InsertItem(2, _T("경도"));
	lc_TargetInfo.InsertItem(3, _T("고도"));
	lc_TargetInfo.InsertItem(4, _T("속도"));
	lc_TargetInfo.InsertItem(5, _T("각도1"));
	lc_TargetInfo.InsertItem(6, _T("각도2"));
	lc_TargetInfo.InsertItem(7, _T("우선순위"));
	lc_TargetInfo.InsertItem(8, _T("첫 탐지 시간"));
	lc_TargetInfo.InsertItem(9, _T("피격 여부"));

	return TRUE;
}

void CTargetInfoDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	// 현재 선택된 ID 기억
	CString selectedIDStr;
	int selIndex = m_comboTargetID.GetCurSel();
	if (selIndex != CB_ERR)
		m_comboTargetID.GetLBText(selIndex, selectedIDStr);

	// 정렬된 리스트 복사
	std::vector<TargetStatus> sortedTargets = targets;
	std::sort(sortedTargets.begin(), sortedTargets.end(), [](const TargetStatus& a, const TargetStatus& b) {
		return a.priority > b.priority; // 우선순위 높은 순
		});

	// 업데이트 여부 판단
	bool needUpdate = (sortedTargets.size() != m_targetList.size());
	if (!needUpdate)
	{
		for (size_t i = 0; i < sortedTargets.size(); ++i)
		{
			if (sortedTargets[i].id != m_targetList[i].id ||
				sortedTargets[i].priority != m_targetList[i].priority)
			{
				needUpdate = true;
				break;
			}
		}
	}

	if (needUpdate)
	{
		m_comboTargetID.ResetContent();
		int restoreIndex = 0;

		for (size_t i = 0; i < sortedTargets.size(); ++i)
		{
			CString idStr;
			idStr.Format(_T("%d"), sortedTargets[i].id);
			m_comboTargetID.AddString(idStr);
			if (idStr == selectedIDStr)
			{
				restoreIndex = static_cast<int>(i);
			}				
		}

		m_comboTargetID.SetCurSel(restoreIndex);
		m_targetList = sortedTargets;
	}
	else
	{
		m_targetList = sortedTargets;
	}

	UpdateUIFromSelection();
}

void CTargetInfoDlg::UpdateUIFromSelection()
{
	int sel = m_comboTargetID.GetCurSel();
	if (sel == CB_ERR || sel >= m_targetList.size()) return;

	const TargetStatus& t = m_targetList[sel];

	CString str;
	str.Format(_T("%d"), t.id);
	lc_TargetInfo.SetItemText(0, 1, str);

	str.Format(_T("%.8f"), static_cast<double>(t.position.x) / 1e8);
	lc_TargetInfo.SetItemText(1, 1, str);

	str.Format(_T("%.8f"), static_cast<double>(t.position.y) / 1e8);
	lc_TargetInfo.SetItemText(2, 1, str);

	str.Format(_T("%lld"), t.position.z);
	lc_TargetInfo.SetItemText(3, 1, str);

	str.Format(_T("%d"), t.speed);
	lc_TargetInfo.SetItemText(4, 1, str);

	str.Format(_T("%.1f"), t.angle1);
	lc_TargetInfo.SetItemText(5, 1, str);

	str.Format(_T("%.1f"), t.angle2);
	lc_TargetInfo.SetItemText(6, 1, str);

	str.Format(_T("%d"), t.priority);
	lc_TargetInfo.SetItemText(7, 1, str);

	str.Format(_T("%lld"), t.first_detect_time);
	lc_TargetInfo.SetItemText(8, 1, str);

	str.Format(_T("%d"), static_cast<int>(t.hit));
	lc_TargetInfo.SetItemText(9, 1, str);
}

void CTargetInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_comboTargetID.GetSafeHwnd() && lc_TargetInfo.GetSafeHwnd())
	{
		const int comboHeight = 30;
		const int margin = 5;

		m_comboTargetID.MoveWindow(0, 0, cx, comboHeight);
		lc_TargetInfo.MoveWindow(0, comboHeight + margin, cx, cy - comboHeight - margin);
	}
}


void CTargetInfoDlg::OnCbnSelchangeComboTargetId()
{
	UpdateUIFromSelection();
}