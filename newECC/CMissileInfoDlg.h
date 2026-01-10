#pragma once
#include "afxdialogex.h"
#include "MissileStatus.h"
#include <vector>

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
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	// 내부 상태
	std::vector<MissileStatus> m_missileList;
	MissileStatus m_missileStatus;

	// 컨트롤
	CListCtrl lc_MissileInfo;
	CComboBox cb_MissileInfo;

	// 동작 메서드
	void UpdateUI();
	void UpdateComboBox();

public:
	void SetMissileList(const std::vector<MissileStatus>& list);
	void SetMissileStatus(const MissileStatus& status);
	afx_msg void OnCbnSelchangeComboMissileId();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
