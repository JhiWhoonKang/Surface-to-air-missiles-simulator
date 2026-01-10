#pragma once
#include "afxdialogex.h"
#include "LCStatus.h"
#include <vector>

// CLaunchControlDlg 대화 상자
class CLaunchControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLaunchControlDlg)

public:
	CLaunchControlDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLaunchControlDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LC_INFO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog() override;

private:
	LCStatus m_lcStatus;
	const double coordsScale = 1e8;

public:
	void SetLCStatus(const LCStatus& status);
	void UpdateUI();
	void SetLcConnected(bool connected);
	bool isLcConnected = false;

private:
	/// <summary>
	/// 발사통제기 정보 List Control
	/// </summary>
	CListCtrl lc_LcInfo;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
