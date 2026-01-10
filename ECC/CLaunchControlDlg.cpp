#include "pch.h"
#include "SAMtest.h"
#include "afxdialogex.h"
#include "CLaunchControlDlg.h"

IMPLEMENT_DYNAMIC(CLaunchControlDlg, CDialogEx)

CLaunchControlDlg::CLaunchControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LC_INFO_DLG, pParent)
{
}

CLaunchControlDlg::~CLaunchControlDlg()
{
}

void CLaunchControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 컨트롤 연결
	DDX_Control(pDX, IDC_LC_STATIC_ID2, m_staticID);
	DDX_Control(pDX, IDC_LC_STATIC_POSX, m_staticPosX);
	DDX_Control(pDX, IDC_LC_STATIC_POSY, m_staticPosY);
	DDX_Control(pDX, IDC_LC_STATIC_POSZ, m_staticPosZ);
	DDX_Control(pDX, IDC_LIST_LC_INFO, m_listLcInfo);
}

BEGIN_MESSAGE_MAP(CLaunchControlDlg, CDialogEx)
	ON_STN_CLICKED(IDC_LC_STATIC_ID2, &CLaunchControlDlg::OnStnClickedLcStaticId2)
	ON_STN_CLICKED(IDC_LC_STATIC_POSX, &CLaunchControlDlg::OnStnClickedLcStaticPosx)
	ON_STN_CLICKED(IDC_LC_STATIC_POSY, &CLaunchControlDlg::OnStnClickedLcStaticPosy)
	ON_STN_CLICKED(IDC_LC_STATIC_POSZ, &CLaunchControlDlg::OnStnClickedLcStaticPosz)
END_MESSAGE_MAP()

BOOL CLaunchControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listLcInfo.MoveWindow(10, 10, 650, 280);

	m_listLcInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listLcInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 50);
	m_listLcInfo.InsertColumn(1, _T("위도"), LVCFMT_LEFT, 100);
	m_listLcInfo.InsertColumn(2, _T("경도"), LVCFMT_LEFT, 100);
	m_listLcInfo.InsertColumn(3, _T("고도"), LVCFMT_LEFT, 100);

	return TRUE;
}

void CLaunchControlDlg::SetLCStatus(const LCStatus& status)
{
    m_lcStatus = status;

    // 유효하지 않은 ID일 경우 무시
    if (status.id == 0)
        return;

    CString str;

    // 리스트가 비어 있으면 새로 삽입, 아니면 업데이트
    if (m_listLcInfo.GetItemCount() == 0)
    {
        // ID (index 0, column 0)
        str.Format(_T("%d"), status.id);
        m_listLcInfo.InsertItem(0, str);
    }
    else
    {
        // ID 업데이트
        str.Format(_T("%d"), status.id);
        m_listLcInfo.SetItemText(0, 0, str);
    }

    // 위도(x)
    str.Format(_T("%.8f"), static_cast<double>(status.position.x) / coordsScale);
    m_listLcInfo.SetItemText(0, 1, str);

    // 경도(y)
    str.Format(_T("%.8f"), static_cast<double>(status.position.y) / coordsScale);
    m_listLcInfo.SetItemText(0, 2, str);

    // 고도(z)
    str.Format(_T("%.2f"), static_cast<double>(status.position.z) / coordsScale);
    m_listLcInfo.SetItemText(0, 3, str);
}


// 클릭 핸들러 (필요 없으면 삭제 가능)
void CLaunchControlDlg::OnStnClickedLcStaticId2() {}
void CLaunchControlDlg::OnStnClickedLcStaticPosx() {}
void CLaunchControlDlg::OnStnClickedLcStaticPosy() {}


void CLaunchControlDlg::OnStnClickedLcStaticPosz()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
