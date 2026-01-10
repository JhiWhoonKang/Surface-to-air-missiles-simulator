#pragma once

#include "afxdialogex.h"
#include "RadarStatus.h"
#include "TargetStatus.h"
#include "resource.h"
#include "afxwin.h"
#include <vector>

class CSAMtestDlg;

class CLeftTopDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLeftTopDlg)

public:
	CLeftTopDlg(CWnd* pParent = nullptr);
	virtual ~CLeftTopDlg();

	enum { IDD = IDD_LEFT_TOP_DLG };

	void SetParentDlg(CSAMtestDlg* parent);
	void SetRadarList(const std::vector<RadarStatus>& radarList);
	void SetTargetList(const std::vector<TargetStatus>& targetList);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnClose();

	afx_msg void OnBnClickedBtnModeSwitch();
	afx_msg void OnCbnSelchangeComboRadarId();
	afx_msg void OnNMCustomdrawRadarList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_comboRadarID;
	CStatic m_staticNetwork;
	CStatic m_staticNetworkMfr;
	bool m_isRadarConnected = false;

	RadarStatus m_currentRadarStatus;
	std::vector<RadarStatus> m_radarList;
	std::vector<TargetStatus> m_targetList;
	CSAMtestDlg* m_parent = nullptr;

	void UpdateRadarStatusFromSelection();
	void UpdateNetworkIndicator(bool isConnected);

	CListCtrl lc_RadarInfo;
	const double coordsScale = 1e8;
	CButton btn_ModeChange;

	void SetAngle(double angle);
	void LayoutUI(int width, int height);

protected:
	double m_radarAngle = 0.0;

private:
	CStatic m_RadarAngleDrawArea;
};
