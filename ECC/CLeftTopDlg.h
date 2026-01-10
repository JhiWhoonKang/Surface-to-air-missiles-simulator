#pragma once

#include "afxdialogex.h"
#include "RadarStatus.h"
#include "TargetStatus.h"
#include "resource.h"
#include "afxwin.h"
#include <vector>

// ✅ forward declaration
class CSAMtestDlg;

// CLeftTopDlg 대화 상자
class CLeftTopDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLeftTopDlg)

public:
	CLeftTopDlg(CWnd* pParent = nullptr);
	virtual ~CLeftTopDlg();

	enum { IDD = IDD_LEFT_TOP_DLG };

	// ✅ 외부 연결 함수
	void SetParentDlg(CSAMtestDlg* parent);
	void SetRadarList(const std::vector<RadarStatus>& radarList);
	void SetTargetList(const std::vector<TargetStatus>& targetList);
	void OnRadarModeChangeSuccess(RadarStatus::Mode newMode);  // ✅ 모드 전환 후 호출

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	// ✅ 메시지 핸들러
	afx_msg void OnBnClickedBtnModeSwitch();
	afx_msg void OnCbnSelchangeComboRadarId();
	afx_msg void OnStnClickedStaticMode();
	afx_msg void OnStnClickedStaticNetwork();
	afx_msg void OnStnClickedStaticAngle();
	afx_msg void OnStnClickedStaticPosition();
	afx_msg void OnStnClickedStaticMfrMode();
	afx_msg void OnStnClickedStaticMfrPosition2();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);  // ✅ 원 그리기
	afx_msg void OnStnClickedStaticNetworkMfr(); // ✅ 새 통신상태 원 클릭 핸들러

	DECLARE_MESSAGE_MAP()

private:
	// ✅ 내부 상태 관리
	CComboBox m_comboRadarID;
	CStatic m_staticNetwork;      // 기존 통신 상태용 Static Text
	CStatic m_staticNetworkMfr;   // ✅ 새로 추가된 원 그리기 Picture Control
	bool m_isRadarConnected = false;

	RadarStatus m_currentRadarStatus;
	std::vector<RadarStatus> m_radarList;
	std::vector<TargetStatus> m_targetList;
	CSAMtestDlg* m_parent = nullptr;

	// ✅ 상태 UI 반영
	void UpdateRadarStatusFromSelection();
	void SetRadarUI(const RadarStatus& status);
	void UpdateNetworkIndicator(bool isConnected); // ✅ 연결상태 원 갱신 함수
public:
	afx_msg void OnStnClickedStaticMfrPosition3();
};
