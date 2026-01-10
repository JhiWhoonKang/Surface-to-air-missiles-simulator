#pragma once

#include "afxdialogex.h"
#include "TargetStatus.h"  // 타겟 정보 구조체
#include <vector>
#include <ctime>
#include <chrono>  // ✅ chrono 사용을 위한 헤더

// ✅ UTC → KST 변환 함수 선언 (C++17 std::chrono 사용)
CString FormatUtcToKST(unsigned long long utc_seconds);

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
	CStatic m_staticSpeed;
	CStatic m_staticAngle1;
	CStatic m_staticAngle2;
	CStatic m_staticPriority;
	CStatic m_staticDetect;
	CStatic m_staticHit;

	// 콤보박스 선택에 따라 UI 업데이트
	void UpdateUIFromSelection();
	void UpdateListSelectionFromCombo();

public:
	afx_msg void OnCbnSelchangeComboTargetId();

	// 클릭 핸들러 (필요 없으면 비워둠)
	afx_msg void OnStnClickedStaticTargetId2();
	afx_msg void OnStnClickedStaticTargetHeight2();
	afx_msg void OnStnClickedStaticTargetSpeed2();
	afx_msg void OnStnClickedStaticTargetAngle2();
	afx_msg void OnStnClickedStaticTargetPriority2();
	afx_msg void OnStnClickedStaticTargetDetect2();
	afx_msg void OnStnClickedStaticTargetHit2();
	afx_msg void OnStnClickedStaticTargetAngle3();
private:
	CListCtrl m_listTargetInfo;
	int coordsScale = 1e7;
};
