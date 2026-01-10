#include "pch.h"
#include "afxdialogex.h"
#include "CLSMoveDlg.h"
#include "resource.h"
#include "SAMtestDlg.h"
#include <iostream>

IMPLEMENT_DYNAMIC(CLSMoveDlg, CDialogEx)

CLSMoveDlg::CLSMoveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LS_MOVE_DLG, pParent)
{
}

CLSMoveDlg::~CLSMoveDlg()
{
}

void CLSMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// EditBox 컨트롤 연결 (숫자 입력용)
	DDX_Text(pDX, IDC_EDIT_POS_X, m_posX);
	DDX_Text(pDX, IDC_EDIT_POS_Y, m_posY);
}

BEGIN_MESSAGE_MAP(CLSMoveDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_POS_X, &CLSMoveDlg::OnEnChangeEditPosX)
	ON_EN_CHANGE(IDC_EDIT_POS_Y, &CLSMoveDlg::OnEnChangeEditPosY)
	ON_BN_CLICKED(IDC_BUTTON_LS_MOVE, &CLSMoveDlg::OnBnClickedButtonLsMove)
END_MESSAGE_MAP()

// ✅ 좌표 변경 시 내부 변수 업데이트
void CLSMoveDlg::OnEnChangeEditPosX()
{
	UpdateData(TRUE);
}

void CLSMoveDlg::OnEnChangeEditPosY()
{
	UpdateData(TRUE);
}

// ✅ 이동 버튼 클릭 시 명령 전송
void CLSMoveDlg::OnBnClickedButtonLsMove()
{
	UpdateData(TRUE);  // EditBox → m_posX/m_posY

	if (m_parent && m_lsId >= 0)
	{
		// 좌표를 long long으로 변환하여 정밀도 유지
		Pos2D pos;
		pos.x = static_cast<long long>(m_posX * 100000000);  // 소수점 8자리
		pos.y = static_cast<long long>(m_posY * 100000000);

		m_parent->sendLSMove(m_lsId, pos);

		//std::cout << std::fixed;
		//std::cout.precision(5);
		//std::cout << "[이동 요청] 발사대 ID=" << m_lsId
		//	<< ", 좌표=(" << m_posX << ", " << m_posY << ")\n";

		EndDialog(IDOK);
	}
	else
	{
		AfxMessageBox(_T("이동 요청 실패: 발사대 ID 또는 부모가 설정되지 않았습니다."));
	}
}


// ✅ 외부에서 Parent와 ID 설정
void CLSMoveDlg::SetParent(CSAMtestDlg* parent) {
	m_parent = parent;
}

void CLSMoveDlg::SetLauncherID(int lsId) {
	m_lsId = lsId;
}
