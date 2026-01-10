#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "MissileStatus.h"
#include "TargetStatus.h"
#include <vector>
#include <deque>

#define IDT_GRAPH_TIMER 1001

struct MockPoint
{
	double missileAltitude;     // m
	double targetAltitude;      // m
	bool missileHit;
	bool targetHit;
	double time;                // s    
};

// CMockTrack 대화 상자

class CMockTrack : public CDialogEx
{
	DECLARE_DYNAMIC(CMockTrack)

public:
	CMockTrack(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMockTrack();
	void CMockTrack::UpdateFromMock(const MissileStatus&, const TargetStatus&, double);
	enum { IDD = IDD_MOCK_TRACK };

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOCK_TRACK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
private:
	std::deque<MockPoint> m_mockData;
	double m_startTime = 0;
	int m_currentTargetId = -1;  // 현재 출력 중인 타겟 ID
	double GetCurrentTimeSeconds() const;

	void AddMockData(double missileAltitude, double targetAltitude, bool missileHit, bool targetHit, double time);
	void ClearData();
	void DrawGrid(CDC& dc, const CRect& area);
	void DrawAxis(CDC& dc, const CRect& area);
	int maxAltitude = 250000;
};
