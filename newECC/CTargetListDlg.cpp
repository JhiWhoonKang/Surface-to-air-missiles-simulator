#include "pch.h"
#include "afxdialogex.h"
#include "CTargetListDlg.h"
#include "resource.h"
#include <algorithm>
#include <iostream>

IMPLEMENT_DYNAMIC(CTargetListDlg, CDialogEx)

CTargetListDlg::CTargetListDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TARGET_LIST_DLG, pParent)
{
}

CTargetListDlg::~CTargetListDlg()
{
}

void CTargetListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TARGET_LIST, m_listTarget);
}

BEGIN_MESSAGE_MAP(CTargetListDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TARGET_LIST, &CTargetListDlg::OnLvnItemchangedTargetList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TARGET_LIST, &CTargetListDlg::OnCustomDrawListTarget)
END_MESSAGE_MAP()

BOOL CTargetListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CRect clientRect;
	GetClientRect(&clientRect);

	const int margin = 10;
	const int topMargin = 20;
	
	m_listTarget.MoveWindow(
		margin,
		topMargin,
		clientRect.Width() - 2 * margin,
		clientRect.Height()
	);

	m_listTarget.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listTarget.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 70);
	m_listTarget.InsertColumn(1, _T("위도"), LVCFMT_CENTER, 120);
	m_listTarget.InsertColumn(2, _T("경도"), LVCFMT_CENTER, 120);
	m_listTarget.InsertColumn(3, _T("고도"), LVCFMT_CENTER, 70);
	m_listTarget.InsertColumn(4, _T("속도"), LVCFMT_CENTER, 100);
	m_listTarget.InsertColumn(5, _T("각도1"), LVCFMT_CENTER, 60);
	m_listTarget.InsertColumn(6, _T("각도2"), LVCFMT_CENTER, 60);
	m_listTarget.InsertColumn(7, _T("우선순위"), LVCFMT_CENTER, 80);
	m_listTarget.InsertColumn(8, _T("격추 여부"), LVCFMT_CENTER, 60);

	return TRUE;
}

void CTargetListDlg::OnOK() {}
void CTargetListDlg::OnCancel() {}

BOOL CTargetListDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_NCLBUTTONDOWN && pMsg->wParam == HTCAPTION)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTargetListDlg::OnClose()
{
	// Alt+F4 방지
}

void CTargetListDlg::OnCustomDrawListTarget(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;

	case CDDS_ITEMPREPAINT:
	{
		int index = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		if (index >= 0 && index < static_cast<int>(m_targetList.size()))
		{
			const auto& target = m_targetList[index];
			if (target.priority == 1)
			{
				pLVCD->clrTextBk = RGB(200, 255, 200);
			}
		}

		*pResult = CDRF_DODEFAULT;
		return;
	}
	}

	*pResult = CDRF_DODEFAULT;
}


void CTargetListDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	//if (targets.size() != m_prevTargetList.size() ||
	//	!std::equal(targets.begin(), targets.end(), m_prevTargetList.begin(),
	//		[](const TargetStatus& a, const TargetStatus& b) {
	//			return a.id == b.id &&
	//				a.priority == b.priority &&
	//				a.hit == b.hit;
	//		}))
	//{
	//	m_prevTargetList = targets;
	//	m_targetList = targets;
	//	std::cout << "needsUpdate?: x" << std::endl;
	//}
	//else
	//{
	//	std::cout << "needsUpdate?: x" << std::endl;
	//	return; // 업데이트 불필요
	//}

	m_prevTargetList = targets;
	m_targetList = targets;

	m_listTarget.SetRedraw(FALSE);
	m_listTarget.DeleteAllItems();

	std::sort(m_targetList.begin(), m_targetList.end(), [](const TargetStatus& a, const TargetStatus& b) {
		return a.priority > b.priority;
		});

	int highlightIndex = -1;

	for (size_t i = 0; i < m_targetList.size(); ++i)
	{
		const auto& t = m_targetList[i];

		CString idStr, xStr, yStr, hStr, sStr, aStr1, aStr2, pStr, hitStr;
		idStr.Format(_T("%d"), t.id);
		xStr.Format(_T("%.8f"), static_cast<double>(t.position.x) / 100000000.0);
		yStr.Format(_T("%.8f"), static_cast<double>(t.position.y) / 100000000.0);
		hStr.Format(_T("%lld"), t.position.z);
		sStr.Format(_T("%d"), t.speed);
		aStr1.Format(_T("%.1f"), t.angle1);
		aStr2.Format(_T("%.1f"), t.angle2);
		pStr.Format(_T("%d"), t.priority);
		hitStr = (t.hit != 0) ? _T("O") : _T("X");

		int row = static_cast<int>(i);
		m_listTarget.InsertItem(row, idStr);
		m_listTarget.SetItemText(row, 1, xStr);
		m_listTarget.SetItemText(row, 2, yStr);
		m_listTarget.SetItemText(row, 3, hStr);
		m_listTarget.SetItemText(row, 4, sStr);
		m_listTarget.SetItemText(row, 5, aStr1);
		m_listTarget.SetItemText(row, 6, aStr2);
		m_listTarget.SetItemText(row, 7, pStr);
		m_listTarget.SetItemText(row, 8, hitStr);

		if (highlightIndex == -1 && t.hit == 0)
		{
			highlightIndex = row;
		}
	}

	if (highlightIndex != -1)
	{
		m_listTarget.SetItemState(highlightIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_listTarget.EnsureVisible(highlightIndex, FALSE);
	}

	m_listTarget.SetRedraw(TRUE);
	//m_listTarget.Invalidate();
}


void CTargetListDlg::OnLvnItemchangedTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// 선택 항목 변경 시 필요한 처리
	*pResult = 0;
}
