#include "pch.h"
#include "SAMtest.h"
#include "CTargetInfoDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <chrono>      // ✅ chrono 사용
#include <ctime>

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
	DDX_Control(pDX, IDC_LIST_TARGET_INFO, m_listTargetInfo);
}

BEGIN_MESSAGE_MAP(CTargetInfoDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET_ID, &CTargetInfoDlg::OnCbnSelchangeComboTargetId)
	ON_STN_CLICKED(IDC_STATIC_TARGET_ID2, &CTargetInfoDlg::OnStnClickedStaticTargetId2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_HEIGHT2, &CTargetInfoDlg::OnStnClickedStaticTargetHeight2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_SPEED2, &CTargetInfoDlg::OnStnClickedStaticTargetSpeed2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_ANGLE2, &CTargetInfoDlg::OnStnClickedStaticTargetAngle2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_PRIORITY2, &CTargetInfoDlg::OnStnClickedStaticTargetPriority2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_DETECT2, &CTargetInfoDlg::OnStnClickedStaticTargetDetect2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_HIT2, &CTargetInfoDlg::OnStnClickedStaticTargetHit2)
	ON_STN_CLICKED(IDC_STATIC_TARGET_ANGLE3, &CTargetInfoDlg::OnStnClickedStaticTargetAngle3)
END_MESSAGE_MAP()

BOOL CTargetInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_comboTargetID.MoveWindow(10, 10, 650, 25);
	m_listTargetInfo.MoveWindow(10, 50, 650, 280);

	m_listTargetInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listTargetInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 50);
	m_listTargetInfo.InsertColumn(1, _T("위도"), LVCFMT_LEFT, 100);
	m_listTargetInfo.InsertColumn(2, _T("경도"), LVCFMT_LEFT, 100);
	m_listTargetInfo.InsertColumn(3, _T("고도"), LVCFMT_LEFT, 50);
	m_listTargetInfo.InsertColumn(4, _T("속도"), LVCFMT_LEFT, 50);
	m_listTargetInfo.InsertColumn(5, _T("각도"), LVCFMT_LEFT, 50);
	m_listTargetInfo.InsertColumn(6, _T("격추"), LVCFMT_LEFT, 50);

	return TRUE;
}

void CTargetInfoDlg::SetTargetList(const std::vector<TargetStatus>& targets)
{
	// 🔹 현재 선택된 ID 기억
	CString selectedIDStr;
	int selIndex = m_comboTargetID.GetCurSel();
	if (selIndex != CB_ERR)
		m_comboTargetID.GetLBText(selIndex, selectedIDStr);

	m_comboTargetID.ResetContent();
	m_targetList.clear();

	if (targets.empty())
		return;

	std::vector<TargetStatus> sortedTargets = targets;
	std::sort(sortedTargets.begin(), sortedTargets.end(), [](const TargetStatus& a, const TargetStatus& b) {
		return a.priority > b.priority;
		});

	int restoreIndex = 0;
	for (size_t i = 0; i < sortedTargets.size(); ++i)
	{
		CString str;
		str.Format(_T("%d"), sortedTargets[i].id);
		m_comboTargetID.AddString(str);

		if (str == selectedIDStr)
			restoreIndex = static_cast<int>(i);

		m_targetList.push_back(sortedTargets[i]);
	}

	m_comboTargetID.SetCurSel(restoreIndex);
	UpdateListSelectionFromCombo(); // 콤보 선택된 항목 기준으로 리스트뷰 출력
}



void CTargetInfoDlg::UpdateListSelectionFromCombo()
{
	int sel = m_comboTargetID.GetCurSel();
	if (sel == CB_ERR)
		return;

	CString selectedIDStr;
	m_comboTargetID.GetLBText(sel, selectedIDStr);
	int selectedID = _ttoi(selectedIDStr);

	m_listTargetInfo.DeleteAllItems();

	for (const auto& t : m_targetList)
	{
		if (t.id != selectedID)
			continue; // 선택된 ID와 다르면 스킵

		CString str;
		int row = 0; // 하나만 출력

		str.Format(_T("%d"), t.id);
		m_listTargetInfo.InsertItem(row, str);

		str.Format(_T("%.8f"), static_cast<double>(t.position.x) / 1e7);
		m_listTargetInfo.SetItemText(row, 1, str);
		str.Format(_T("%.8f"), static_cast<double>(t.position.y) / 1e7);
		m_listTargetInfo.SetItemText(row, 2, str);
		str.Format(_T("%lld"), t.position.z);
		m_listTargetInfo.SetItemText(row, 3, str);
		str.Format(_T("%d"), t.speed);
		m_listTargetInfo.SetItemText(row, 4, str);
		str.Format(_T("%.1f"), t.angle1);
		m_listTargetInfo.SetItemText(row, 5, str);
		str.Format(_T("%d"), static_cast<int>(t.hit));
		m_listTargetInfo.SetItemText(row, 6, str);

		break; // 한 표적만 표시 후 종료
	}
}



#include <chrono>

// ✅ UTC 밀리초(ms) → KST 문자열 (CString) 변환
CString FormatUtcToKST(unsigned long long utc_milliseconds)
{
	using namespace std::chrono;

	// 1. 밀리초 → system_clock::time_point
	system_clock::time_point utc_time = system_clock::time_point(milliseconds(utc_milliseconds));

	// 2. UTC + 9시간 → KST
	system_clock::time_point kst_time = utc_time + hours(9);

	// 3. time_t로 변환
	time_t tt = system_clock::to_time_t(kst_time);

	// 4. tm 구조체로 변환
	struct tm tm_kst;
	gmtime_s(&tm_kst, &tt);  // ✅ gmtime_s 사용 (이미 +9시간 되었기 때문에 지역 시간 불필요)

	// 5. 문자열 생성
	CString result;
	result.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
		tm_kst.tm_year + 1900,
		tm_kst.tm_mon + 1,
		tm_kst.tm_mday,
		tm_kst.tm_hour,
		tm_kst.tm_min,
		tm_kst.tm_sec);

	return result;
}

// 클릭 핸들러 (비워둠 또는 로그 출력용)
void CTargetInfoDlg::OnCbnSelchangeComboTargetId() 
{ 
	UpdateListSelectionFromCombo();
}

void CTargetInfoDlg::OnStnClickedStaticTargetId2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetHeight2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetSpeed2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetAngle2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetPriority2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetDetect2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetHit2() {}
void CTargetInfoDlg::OnStnClickedStaticTargetAngle3() {}
