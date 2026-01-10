#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "TargetStatus.h"  // ✅ Target 구조체 포함
#include <vector>          // ✅ 리스트 저장용

// CTargetListDlg 대화 상자
class CTargetListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTargetListDlg)

public:
	CTargetListDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTargetListDlg();
	void SetTargetList(const std::vector<TargetStatus>& targets);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();                        // 초기화 시 리스트 설정
	afx_msg void OnCustomDrawListTarget(NMHDR* pNMHDR, LRESULT* pResult);

	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_listTarget;                          // 리스트 컨트롤
	std::vector<TargetStatus> m_targetList;          // 표적 리스트 저장
	std::vector<TargetStatus> m_prevTargetList;
public:
	enum { IDD = IDD_TARGET_LIST_DLG };              // 리소스 ID
	
	afx_msg void OnLvnItemchangedTargetList(NMHDR* pNMHDR, LRESULT* pResult); // 항목 변경 시 콜백
};
