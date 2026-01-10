#pragma once

#include "afxdialogex.h"
#include "TargetStatus.h"  // ✅ 표적 구조체
#include "resource.h"
#include<vector>

class CSAMtestDlg;  // ✅ 메인 다이얼로그 포인터 호출용

class CMFRModeSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFRModeSelectDlg)

private:
	bool m_modeChanged = false;
	unsigned int m_selectedTargetID = -1;
	CComboBox m_comboTargetID;
	CSAMtestDlg* m_parent = nullptr;
	unsigned int m_radarId = 0;
	std::vector<TargetStatus> m_targetList;  // ✅ 서버에서 받은 표적 리스트

public:
	void SetContext(CSAMtestDlg* parent, unsigned int radarId) {
		m_parent = parent;
		m_radarId = radarId;
	}
	CMFRModeSelectDlg(CWnd* pParent = nullptr);
	virtual ~CMFRModeSelectDlg();

	enum { IDD = IDD_MFRMODE_SELECT_DLG };

	// ✅ 외부에서 표적 리스트 전달받기
	void SetTargetList(const std::vector<TargetStatus>& targets) { m_targetList = targets; }

	// ✅ 선택한 표적 ID 반환
	int GetSelectedTargetID() const { return m_selectedTargetID; }

	bool IsModeChanged() const { return m_modeChanged; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonMfrmodeChange();
	afx_msg void OnCbnSelchangeComboMfrTarget();
};
