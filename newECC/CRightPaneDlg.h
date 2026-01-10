#pragma once
#include "CLaunchControlDlg.h"
#include "CMissileInfoDlg.h"
#include "CTargetInfoDlg.h"
#include "afxdialogex.h"
#include "resource.h"
class CRightPaneDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRightPaneDlg)

public:
	CRightPaneDlg(CWnd* pParent = nullptr);
	enum { IDD = IDD_RIGHT_PANE_DLG };
	void SetLCStatus(const LCStatus& status);
	void SetMissileStatus(const std::vector<MissileStatus>& missiles);  // ✅ 외부에서 호출할 함수
	void SetTargetList(const std::vector<TargetStatus>& targets);
	virtual ~CRightPaneDlg();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

private:
	CLaunchControlDlg m_launchDlg;
	CMissileInfoDlg m_missileDlg;
	CTargetInfoDlg m_targetDlg;
	std::vector<TargetStatus> m_targetList;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
