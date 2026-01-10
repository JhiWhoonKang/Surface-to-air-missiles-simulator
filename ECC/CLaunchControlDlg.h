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
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	LCStatus m_lcStatus;               // ✅ 현재 LC 상태 보관
	CStatic m_staticID;               // ✅ ID 출력용
	CStatic m_staticPosX;             // ✅ X 좌표 출력용
	CStatic m_staticPosY;             // ✅ Y 좌표 출력용
	CStatic m_staticPosZ;             // ✅ Z 좌표 출력용

public:
	void SetLCStatus(const LCStatus& status);    // ✅ 외부에서 LC 정보 설정
	void UpdateUI();                             // ✅ 화면에 출력

	afx_msg void OnStnClickedLcStaticId2();
	afx_msg void OnStnClickedLcStaticPosx();
	afx_msg void OnStnClickedLcStaticPosy();
	afx_msg void OnStnClickedLcStaticPosz();
private:
	CListCtrl m_listLcInfo;
	int coordsScale = 1e7;
};
