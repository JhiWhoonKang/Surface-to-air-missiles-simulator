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
	/// <summary>	
	/// 발사대 선택 Combo Box
	/// </summary>
	CComboBox m_comboLSID;
	/// <summary>	
	/// 발사대 목록 저장
	/// </summary>
	std::vector<LSStatus> m_lsList;
	/// <summary>	
	/// 현재 선택된 발사대 정보
	/// </summary>
	LSStatus m_currentLSStatus{};
	/// <summary>	
	/// CSAMtestDlg m_parent
	/// </summary>
	CSAMtestDlg* m_parent = nullptr;
	/// <summary>	
	/// Target 리스트
	/// </summary>
	std::vector<TargetStatus> m_targetList;
	/// <summary>	
	/// 발사대 네트워크 연결 여부
	/// </summary>
	bool m_isLSConnected = false;

public:
	/// <summary>	
	/// CLeftBottomDlg 생성자
	/// </summary>
	CLeftBottomDlg(CWnd* pParent = nullptr);
	/// <summary>	
	/// CLeftBottomDlg 소멸자
	/// </summary>
	virtual ~CLeftBottomDlg();
	/// <summary>	
	/// SetParentDlg 부모
	/// </summary>
	void SetParentDlg(CSAMtestDlg* parent);

	enum { IDD = IDD_LEFT_BOTTOM_DLG };
	/// <summary>	
	/// 외부에서 발사대 리스트 설정
	/// </summary>
	void SetLSList(const std::vector<LSStatus>& list);
	/// <summary>	
	/// Combo Box 선택 시 정보 갱신
	/// </summary>
	void UpdateLSStatusFromSelection();
	/// <summary>	
	/// 발사대 정보 가시화
	/// </summary>
	void SetLSUI(const LSStatus& status);
	/// <summary>	
	/// 발사대 버튼 설정
	/// 모드에 따른 버튼 활성화/비활성화
	/// </summary>
	void UpdateLSModeUI();
	/// <summary>	
	/// ACK 핸들러
	/// </summary>
	void OnLSModeChangeAck(uint8_t lsId, uint8_t mode);
	/// <summary>	
	/// ACK 핸들러
	/// </summary>
	void SetTargetList(const std::vector<TargetStatus>& targetList);

	void OnLSModeChangeSuccess(uint8_t lsId, uint8_t newMode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnClose();

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
private:
	const double coordsScale = 1e8;
	void UpdateLSModeState(uint8_t lsId, uint8_t newMode);
	CListCtrl lc_LauncherInfo;
	CButton btn_Launch;
	CButton btn_Mode;
	CButton btn_InputMoveCoords;
public:
	afx_msg void OnBtnLaunch();
	afx_msg void OnBtnModeChange();
	afx_msg void OnBtnInputLsCoords();
	afx_msg void OnBnClickedButtonModeChange();
private:
	CButton btn_LsLaunch;
	CButton btn_LsModeChange;
	CButton btn_LsMove;
};
