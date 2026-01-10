#pragma once

#include "afxdialogex.h"
#include "TargetStatus.h"  // 타겟 정보 구조체
#include <vector>

// CTargetInfoDlg 대화 상자
class CTargetInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTargetInfoDlg)

public:
	CTargetInfoDlg(CWnd* pParent = nullptr);
	virtual ~CTargetInfoDlg();

	enum { IDD = IDD_TARGET_INFO_DLG };

	// 외부에서 타겟 리스트 전달받기
	void SetTargetList(const std::vector<TargetStatus>& targets);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	std::vector<TargetStatus> m_targetList;

	// 컨트롤 멤버
	CComboBox m_comboTargetID;
	CStatic m_staticID;
	CStatic m_staticPosX;
	CStatic m_staticPosY;
	CStatic m_staticPosZ;
	//CStatic m_staticHeight;
	CStatic m_staticSpeed;
	CStatic m_staticAngle1;
	CStatic m_staticAngle2;
	CStatic m_staticPriority;
	CStatic m_staticDetect;
	CStatic m_staticHit;

	// 콤보박스 선택에 따라 UI 업데이트
	void UpdateUIFromSelection();

public:
	afx_msg void OnCbnSelchangeComboTargetId();

private:
	CListCtrl lc_TargetInfo;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
