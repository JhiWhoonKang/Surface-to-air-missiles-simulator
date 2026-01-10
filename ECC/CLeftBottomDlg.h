#pragma once
#include "afxdialogex.h"
#include "LSStatus.h"
#include <vector>
#include "resource.h"
#include "TargetStatus.h"

class CSAMtestDlg;

class CLeftBottomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLeftBottomDlg)

private:
	CComboBox m_comboLSID;                    // 콤보박스 컨트롤
	std::vector<LSStatus> m_lsList;           // ✅ 발사대 목록 저장
	LSStatus m_currentLSStatus{};             // ✅ 현재 선택된 발사대 정보
	CSAMtestDlg* m_parent = nullptr;
	std::vector<TargetStatus> m_targetList;

	CStatic m_staticNetworkLS;                // ✅ 발사대 통신 상태 표시 Picture Control
	bool m_isLSConnected = false;             // ✅ 현재 발사대 연결 여부

public:
	CLeftBottomDlg(CWnd* pParent = nullptr);
	virtual ~CLeftBottomDlg();
	void SetParentDlg(CSAMtestDlg* parent);
	enum { IDD = IDD_LEFT_BOTTOM_DLG };

	void SetLSList(const std::vector<LSStatus>& list);     // ✅ 외부에서 발사대 리스트 설정
	void UpdateLSStatusFromSelection();                    // ✅ 콤보박스 선택 시 정보 갱신
	void SetLSUI(const LSStatus& status);					// UI 출력 함수
	void UpdateLSModeUI();                                  // 버튼 UI 상태 설정
	void OnLSModeChangeAck(uint8_t lsId, uint8_t mode);     // ACK 핸들러
	void SetTargetList(const std::vector<TargetStatus>& targetList);
	void OnLSModeChangeSuccess(uint8_t lsId, uint8_t newMode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeComboLsId();
	afx_msg void OnBnClickedBtnModeSwitch();
	afx_msg void OnBnClickedButtonMove();
	afx_msg void OnBnClickedButtonLaunch();
	afx_msg void OnStnClickedStaticNetworkLs();            // ✅ 클릭 핸들러
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);  // ✅ 원 그리기
	void UpdateLSNetworkIndicator(bool isConnected);       // ✅ 연결 상태 표시 갱신
	afx_msg void OnStnClickedStaticLsPosition3();
};
