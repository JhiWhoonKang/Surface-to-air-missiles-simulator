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
	DDX_Control(pDX, IDC_LIST_RADAR_INFO, lc_RadarInfo);
	DDX_Control(pDX, IDC_BTN_MODE_SWITCH, btn_ModeChange);
	DDX_Control(pDX, IDC_STATIC_RADAR_DRAW_AREA, m_RadarAngleDrawArea);
}

BEGIN_MESSAGE_MAP(CLeftTopDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MODE_SWITCH, &CLeftTopDlg::OnBnClickedBtnModeSwitch)
	ON_CBN_SELCHANGE(IDC_COMBO_RADAR_ID, &CLeftTopDlg::OnCbnSelchangeComboRadarId)
	ON_WM_DRAWITEM()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_RADAR_INFO, &CLeftTopDlg::OnNMCustomdrawRadarList)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CLeftTopDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateNetworkIndicator(false);

	CRect clientRect;
	GetClientRect(&clientRect);

	int width = clientRect.Width();
	std::cout << "width: " << width << std::endl;
	int height = clientRect.Height();
	
	int comboHeight = 30;
	int buttonHeight = 40;
	const int margin = 10;
	int spacing = 5;

	// (1) lc_RadarInfo: 하단 고정 + 높이 보장
	int listTop = width * 3 / 5;
	int listHeight = height - margin;
	lc_RadarInfo.MoveWindow(
		margin,
		listTop,
		width - 2 * margin,
		listHeight
	);

	// (2) 버튼: 리스트 바로 위
	int buttonTop = listTop - spacing - buttonHeight;
	btn_ModeChange.MoveWindow(
		margin,
		buttonTop,
		width - 2 * margin,
		buttonHeight
	);

	// (3) 콤보박스: 맨 위 고정
	m_comboRadarID.MoveWindow(
		margin,
		margin,
		width - 2 * margin,
		comboHeight
	);

	// (4) 그림 영역: 콤보박스와 버튼 사이 중앙 정렬
	int drawTop = margin + comboHeight + spacing;
	int drawBottom = buttonTop - spacing;
	int drawHeight = drawBottom - drawTop;
	int drawSize = min(drawHeight, width - 2 * margin);

	int drawLeft = (width - drawSize) / 2;
	m_RadarAngleDrawArea.MoveWindow(
		drawLeft,
		drawTop,
		drawSize,
		drawSize
	);

	// 리스트 스타일 및 컬럼
	lc_RadarInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lc_RadarInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 100);
	lc_RadarInfo.InsertColumn(1, _T("위도"), LVCFMT_LEFT, 130);
	lc_RadarInfo.InsertColumn(2, _T("경도"), LVCFMT_LEFT, 130);
	lc_RadarInfo.InsertColumn(3, _T("고도"), LVCFMT_LEFT, 100);
	lc_RadarInfo.InsertColumn(4, _T("모드"), LVCFMT_LEFT, 100);
	lc_RadarInfo.InsertColumn(5, _T("회전각"), LVCFMT_LEFT, 100);
	lc_RadarInfo.InsertColumn(6, _T("네트워크 연결 상태"), LVCFMT_LEFT, 130);

	return TRUE;
}

void CLeftTopDlg::OnOK() {}
void CLeftTopDlg::OnCancel() {}

BOOL CLeftTopDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_NCLBUTTONDOWN && pMsg->wParam == HTCAPTION)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLeftTopDlg::OnClose()
{
}

void CLeftTopDlg::OnNMCustomdrawRadarList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		int row = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		int col = pLVCD->iSubItem;

		CString cellText = lc_RadarInfo.GetItemText(row, col);

		if (col == 1)
		{
			if (m_isRadarConnected)
			{
				pLVCD->clrTextBk = RGB(200, 255, 200);
				pLVCD->clrText = RGB(0, 0, 0);
			}
			else
			{
				pLVCD->clrTextBk = RGB(255, 200, 200);
				pLVCD->clrText = RGB(0, 0, 0);
			}
		}

		// 기존 회전/정지 모드 색상 처리
		/*if (col == 4)
		{
			if (cellText == _T("정지"))
			{
				pLVCD->clrTextBk = RGB(255, 200, 200);
				pLVCD->clrText = RGB(0, 0, 0);
			}
			else if (cellText == _T("회전"))
			{
				pLVCD->clrTextBk = RGB(200, 255, 200);
				pLVCD->clrText = RGB(0, 0, 0);
			}
		}*/

		*pResult = CDRF_DODEFAULT;
		return;
	}
	}

	*pResult = CDRF_DODEFAULT;
}

void CLeftTopDlg::SetParentDlg(CSAMtestDlg* parent)
{
	m_parent = parent;
}

void CLeftTopDlg::SetRadarList(const std::vector<RadarStatus>& radarList)
{	
	m_radarList = radarList;
	m_comboRadarID.ResetContent();
	lc_RadarInfo.DeleteAllItems(); // 리스트 초기화

	for (size_t i = 0; i < radarList.size(); ++i)
	{
		const auto& status = radarList[i];
		CString str;

		if (status.id == 0)
		{
			continue;
		}

		str.Format(_T("%d"), status.id);
		m_comboRadarID.AddString(str);

		lc_RadarInfo.InsertItem(static_cast<int>(i), str);

		str.Format(_T("%.8f"), static_cast<double>(status.position.x) / coordsScale);
		lc_RadarInfo.SetItemText(static_cast<int>(i), 1, str);

		str.Format(_T("%.8f"), static_cast<double>(status.position.y) / coordsScale);
		lc_RadarInfo.SetItemText(static_cast<int>(i), 2, str);

		str.Format(_T("%.2f"), static_cast<double>(status.position.z) / coordsScale);
		lc_RadarInfo.SetItemText(static_cast<int>(i), 3, str);

		lc_RadarInfo.SetItemText(static_cast<int>(i), 4, (status.mode == RadarStatus::ROTATE) ? _T("회전") : _T("정지"));

		str.Format(_T("%.1f"), static_cast<double>(status.angle));
		lc_RadarInfo.SetItemText(static_cast<int>(i), 5, str);
		SetAngle(status.angle);

		str = (m_isRadarConnected ? "Connected" : "Disconnected");
		lc_RadarInfo.SetItemText(static_cast<int>(i), 6, str);
	}

	if (!radarList.empty())
	{
		m_comboRadarID.SetCurSel(0);
		UpdateRadarStatusFromSelection();
	}
	else
	{
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
	if (sel == CB_ERR || sel >= static_cast<int>(m_radarList.size()))
	{
		UpdateNetworkIndicator(false);
		return;
	}

	CString strID;
	m_comboRadarID.GetLBText(sel, strID);
	int radarID = _ttoi(strID);

	for (const auto& radar : m_radarList)
	{
		if (radar.id == radarID)
		{
			m_currentRadarStatus = radar;
			UpdateNetworkIndicator(true);
			return;
		}
	}
	UpdateNetworkIndicator(false);
}

void CLeftTopDlg::UpdateNetworkIndicator(bool isConnected)
{
	m_isRadarConnected = isConnected;
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

void CLeftTopDlg::OnPaint()
{
	CPaintDC dc(this);

	// Picture Control 위치 가져오기
	CRect drawRect;
	m_RadarAngleDrawArea.GetWindowRect(&drawRect);
	ScreenToClient(&drawRect);

	CDC* pDC = &dc;
	pDC->FillSolidRect(drawRect, RGB(255, 255, 255));

	CPoint center = drawRect.CenterPoint();
	int radius = min(drawRect.Width(), drawRect.Height()) / 2 - 10;

	// 원
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Ellipse(center.x - radius, center.y - radius,
		center.x + radius, center.y + radius);

	// 각도 계산
	double angleRad = m_radarAngle * 3.141592 / 180.0;
	int x = center.x + radius * cos(angleRad);
	int y = center.y - radius * sin(angleRad); // y축 반전

	CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(center);
	pDC->LineTo(x, y);
	pDC->SelectObject(pOldPen);
}

void CLeftTopDlg::SetAngle(double angle)
{
	m_radarAngle = angle;

	m_RadarAngleDrawArea.Invalidate();
	m_RadarAngleDrawArea.UpdateWindow();
}

/*
void CLeftTopDlg::OnRadarModeChangeSuccess(RadarStatus::Mode newMode)
{
	m_currentRadarStatus.mode = newMode;
	SetRadarUI(m_currentRadarStatus);
}*/