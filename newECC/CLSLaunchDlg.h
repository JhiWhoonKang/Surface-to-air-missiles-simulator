#pragma once

#include "afxdialogex.h"
#include "TargetStatus.h"       // 표적 정보 구조체
#include "SAMtestDlg.h"        // 부모 다이얼로그 클래스 (정의 필요)

#include <vector>

// CLSLaunchDlg 대화 상자
class CLSLaunchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLSLaunchDlg)

public:
	CLSLaunchDlg(CWnd* pParent = nullptr);
	virtual ~CLSLaunchDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LS_LAUNCH_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_comboTargetID;                     // 콤보박스 컨트롤
	std::vector<TargetStatus> m_targetList;        // 표적 리스트
	int m_lsId = -1;                                // 현재 발사대 ID
	CSAMtestDlg* m_parent = nullptr;               // 상위 다이얼로그 포인터
	int m_selectedTargetID = -1;									// 선택한 표적 ID

public:
	void SetTargetList(const std::vector<TargetStatus>& targets);	// 표적 리스트 설정
	void SetLauncherID(int id);                                     // 발사대 ID 설정
	void SetParent(CSAMtestDlg* parent);                            // 상위 다이얼로그 설정
	int GetSelectedTargetID() const;                                // 선택된 표적 ID 조회

	afx_msg void OnCbnSelchangeComboLsTarget();
	afx_msg void OnBnClickedButtonLsLaunch();
};
