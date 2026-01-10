#pragma once
#include "afxdialogex.h"
#include "MessageCommon.h"  // Pos2D 정의 포함
#include "SAMtestDlg.h"    // 부모 다이얼로그 참조

// CLSMoveDlg 대화 상자

class CLSMoveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLSMoveDlg)

public:
	CLSMoveDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLSMoveDlg();

	// ✅ 부모 다이얼로그와 발사대 ID 설정
	void SetParent(CSAMtestDlg* parent);
	void SetLauncherID(int lsId);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	double m_posX = 0.0;               // 입력된 좌표 X
	double m_posY = 0.0;               // 입력된 좌표 Y
	int m_lsId = -1;              // 발사대 ID
	CSAMtestDlg* m_parent = nullptr;  // 부모 다이얼로그

public:
	enum { IDD = IDD_LS_MOVE_DLG };

	afx_msg void OnEnChangeEditPosX();
	afx_msg void OnEnChangeEditPosY();
	afx_msg void OnBnClickedButtonLsMove();

};
