#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "MissileStatus.h"
#include "TargetStatus.h"
#include <vector>
#include <deque>

#define IDT_GRAPH_TIMER 1001

struct MockPoint {
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
    CMockTrack(CWnd* pParent = nullptr);
    virtual ~CMockTrack();
    void CMockTrack::UpdateFromMock(const MissileStatus& , const TargetStatus& , double );
    void CMockTrack::UpdateFromMissile(const MissileStatus& , double );
    void CMockTrack::UpdateFromTarget(const MissileStatus& , double );
    enum { IDD = IDD_MOCK_TRACK };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()

private:
    std::deque<MockPoint> m_mockData;
    double m_startTime = 0;
    double GetCurrentTimeSeconds() const;

    void AddMockData(double missileAltitude, double targetAltitude, bool missileHit, bool targetHit, double time);
    void AddMissileData(double time, double altitude);
    void AddTargetData(double time, double altitude);
    void ClearData();
    void DrawGrid(CDC& dc, const CRect& area);
    void DrawAxis(CDC& dc, const CRect& area);
    int maxAltitude = 250000;
};