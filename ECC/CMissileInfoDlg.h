#pragma once

#include "afxdialogex.h"
#include "MissileStatus.h"
#include <chrono>  // ✅ UTC 시간 계산용

// CMissileInfoDlg 대화 상자
class CMissileInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMissileInfoDlg)

public:
	CMissileInfoDlg(CWnd* pParent = nullptr);
	virtual ~CMissileInfoDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MISSILE_INFO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog() override;
	virtual void OnDestroy();  // ✅ 타이머 정리용

	DECLARE_MESSAGE_MAP()

public:
	// ✅ 외부에서 미사일 정보 설정
	void SetMissileStatus(const MissileStatus& status);

	// ✅ 내부적으로 UI 갱신
	void UpdateUI();

	// 컨트롤 핸들러들
	afx_msg void OnStnClickedStaticMissileId2();
	afx_msg void OnStnClickedStaticMissilePositionx();
	afx_msg void OnStnClickedStaticMissilePositiony();
	afx_msg void OnStnClickedStaticMissileHeight2();
	afx_msg void OnStnClickedStaticMissileSpeed2();
	afx_msg void OnStnClickedStaticMissileAngle2();
	afx_msg void OnStnClickedStaticMissileShootdowntime2();

	afx_msg void OnTimer(UINT_PTR nIDEvent);  // ✅ 타이머 콜백

private:
	MissileStatus m_missileStatus;

	// ✅ 미사일 UI 컨트롤들
	CStatic m_staticID;
	CStatic m_staticPosX;
	CStatic m_staticPosY;
	CStatic m_staticPosZ;
	CStatic m_staticSpeed;
	CStatic m_staticAngle;
	CStatic m_staticShootTime;

	// ✅ 격추까지 남은 시간 (초 단위)
	int m_secondsRemaining = 0;
	int m_lastMissileID = -1;       // 마지막 갱신된 미사일 ID
	bool m_missileHitConfirmed = false;
	bool m_timeLocked = false;
	// ✅ 타이머 ID
	static const UINT_PTR TIMER_ID_MISSILE = 3001;
	CListCtrl m_listMissileInfo;

	int coordsScale = 1e7;
};