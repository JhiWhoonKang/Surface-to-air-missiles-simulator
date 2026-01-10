#pragma once

#include "CLeftTopDlg.h"
#include "CLeftBottomDlg.h"
#include "CRightPaneDlg.h"
#include <afxsock.h>
#include "IReceiver.h"
#include "ECC_TCP.h"
#include "MessageCommon.h"
#include "CTargetListDlg.h"
#include "CMockTrack.h"
#include "CLSLaunchDlg.h"
#include "CLaunchControlDlg.h"
#include <chrono>
#include <memory>  // ✅ 스마트 포인터

// CSAMtestDlg 대화 상자
class CSAMtestDlg : public CDialogEx, public IReceiver
{
private:
	CLeftTopDlg m_leftTop;
	CLeftBottomDlg m_leftBottom;
	CRightPaneDlg m_rightPane;
	CTargetListDlg m_targetListDlg;
	CLaunchControlDlg m_lcControlDlg;
	CStatic m_mapArea; // 지도
	CMockTrack m_mockTrack;
	HICON m_hIcon;
    std::unique_ptr<ECC_TCP> m_tcp;
	int goalTargetId = -1;

public:
	CSAMtestDlg(CWnd* pParent = nullptr);
	//void init(ECC_TCP* tcp);  // ❓ 외부에서 주입한다면 여전히 포인터일 수도 있음
	void sendStatusRequest();
	void sendRadarModeChange(unsigned int radar_id, uint8_t mode, uint8_t priority_select, unsigned int target_id);
	void sendLSModeChange(unsigned int ls_id, uint8_t mode);
	void sendMissileLaunch(unsigned int ls_id, unsigned int target_id);
	void sendLSMove(unsigned int ls_id, Pos2D pos);
	void receive(int len, const char* packet) override;
	void setGoalTargetId(int id);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAMTEST_DIALOG };
#endif
	enum { TIMER_ID_REQUEST = 1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnTimer(UINT_PTR nIDEvent);   // ⏰ 상태 요청 타이머
	afx_msg void OnDestroy();                 // ⛔ 타이머 해제 등 다른 자원 해제 용도

	DECLARE_MESSAGE_MAP()
};
