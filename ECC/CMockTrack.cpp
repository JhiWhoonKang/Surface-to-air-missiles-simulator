// CMockTrack.cpp: 구현 파일
//

#include "pch.h"
#include "afxdialogex.h"
#include "CMockTrack.h"
#include "resource.h"
#include <ctime>
#include <iostream>

// CMockTrack 대화 상자

IMPLEMENT_DYNAMIC(CMockTrack, CDialogEx)

CMockTrack::CMockTrack(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOCK_TRACK, pParent)
{

}

CMockTrack::~CMockTrack()
{
}

void CMockTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMockTrack, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMockTrack 메시지 처리기
BOOL CMockTrack::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_startTime = static_cast<double>(clock()) / CLOCKS_PER_SEC;
    SetTimer(IDT_GRAPH_TIMER, 100, nullptr); // 100ms 타이머
    return TRUE;
}

double CMockTrack::GetCurrentTimeSeconds() const
{
    return (static_cast<double>(clock()) / CLOCKS_PER_SEC) - m_startTime;
}

void CMockTrack::UpdateFromMock(const MissileStatus& missile, const TargetStatus& target, double currentTime)
{
    // 🔹 새로운 타겟이 들어오면 이전 곡선을 초기화
    if (target.id != m_currentTargetId)
    {
        ClearData();
        m_currentTargetId = target.id;
    }

    AddMockData(missile.position.z, target.position.z, missile.hit, target.hit, currentTime);
}


void CMockTrack::AddMockData(double missileAltitude, double targetAltitude, bool missileHit, bool targetHit, double time)
{
    m_mockData.push_back({ missileAltitude, targetAltitude, missileHit, targetHit, time });

    while (!m_mockData.empty() && time - m_mockData.front().time > 30.0)
    {
        m_mockData.pop_front();
    }
    Invalidate();
}

void CMockTrack::ClearData()
{
    m_mockData.clear();
    Invalidate();
}

void CMockTrack::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == IDT_GRAPH_TIMER)
    {
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CMockTrack::OnPaint()
{
    std::cout << "hi" << std::endl;
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    dc.FillSolidRect(rect, RGB(255, 255, 255));

    const int margin = 50;
    CRect graphArea = rect;
    graphArea.DeflateRect(margin, margin);

    DrawGrid(dc, graphArea);
    DrawAxis(dc, graphArea);

    if (m_mockData.empty())
    {
        std::cout << "[CMockTrack::OnPaint()] empty" << std::endl;
        return;
    }

    const MockPoint& latest = m_mockData.back();
    if (latest.missileHit || latest.targetHit)
    {
        std::cout << "[CMockTrack::OnPaint()] " << "missile hit? " << latest.missileHit << ", target hit? " << latest.targetHit << std::endl;
        return;
    }

    std::cout << "[CMockTrack::OnPaint()] ok" << std::endl;

    double timeStart = m_mockData.front().time;
    double timeEnd = m_mockData.back().time;
    double timeSpan = timeEnd - timeStart;
    if (timeSpan < 1.0)
    {
        timeSpan = 1.0;
    }

    // 최대 고도 계산 (둘 중 큰 값)
    maxAltitude = 0;
    for (const auto& pt : m_mockData)
    {
        if (pt.missileAltitude > maxAltitude)
        {
            maxAltitude = pt.missileAltitude;
        }
        if (pt.targetAltitude > maxAltitude)
        {
            maxAltitude = pt.targetAltitude;
        }
    }
    if (maxAltitude < 10000)
    {
        maxAltitude = 10000;
    }

    // ──────────────── 미사일 고도 곡선 ────────────────
    CPen penMissile(PS_SOLID, 2, RGB(0, 0, 255));  // 파란색
    CPen* pOldPen = dc.SelectObject(&penMissile);

    for (size_t i = 1; i < m_mockData.size(); ++i)
    {
        const auto& prev = m_mockData[i - 1];
        const auto& curr = m_mockData[i];

        double xRatio1 = log(1 + prev.time - timeStart) / log(1 + timeSpan);
        double xRatio2 = log(1 + curr.time - timeStart) / log(1 + timeSpan);

        int x1 = graphArea.left + static_cast<int>(xRatio1 * graphArea.Width());
        int y1 = graphArea.bottom - static_cast<int>((prev.missileAltitude / maxAltitude) * graphArea.Height());
        int x2 = graphArea.left + static_cast<int>(xRatio2 * graphArea.Width());
        int y2 = graphArea.bottom - static_cast<int>((curr.missileAltitude / maxAltitude) * graphArea.Height());

        dc.MoveTo(x1, y1);
        dc.LineTo(x2, y2);
    }

    dc.SelectObject(pOldPen);

    // ──────────────── 타겟 고도 곡선 ────────────────
    CPen penTarget(PS_SOLID, 2, RGB(255, 0, 0));  // 빨간색
    pOldPen = dc.SelectObject(&penTarget);

    for (size_t i = 1; i < m_mockData.size(); ++i)
    {
        const auto& prev = m_mockData[i - 1];
        const auto& curr = m_mockData[i];

        double xRatio1 = log(1 + prev.time - timeStart) / log(1 + timeSpan);
        double xRatio2 = log(1 + curr.time - timeStart) / log(1 + timeSpan);

        int x1 = graphArea.left + static_cast<int>(xRatio1 * graphArea.Width());
        int y1 = graphArea.bottom - static_cast<int>((prev.targetAltitude / maxAltitude) * graphArea.Height());
        int x2 = graphArea.left + static_cast<int>(xRatio2 * graphArea.Width());
        int y2 = graphArea.bottom - static_cast<int>((curr.targetAltitude / maxAltitude) * graphArea.Height());

        dc.MoveTo(x1, y1);
        dc.LineTo(x2, y2);
    }
    dc.SelectObject(pOldPen);
}


void CMockTrack::DrawGrid(CDC& dc, const CRect& area)
{
    CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
    CPen* pOld = dc.SelectObject(&gridPen);

    for (int i = 1; i < 10; ++i)
    {
        int y = area.top + i * area.Height() / 10;
        dc.MoveTo(area.left, y);
        dc.LineTo(area.right, y);
    }

    for (int i = 1; i < 10; ++i)
    {
        int x = area.left + i * area.Width() / 10;
        dc.MoveTo(x, area.top);
        dc.LineTo(x, area.bottom);
    }

    dc.SelectObject(pOld);
}

void CMockTrack::DrawAxis(CDC& dc, const CRect& area)
{
    CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
    CPen* pOld = dc.SelectObject(&axisPen);

    dc.MoveTo(area.left, area.top);
    dc.LineTo(area.left, area.bottom);

    dc.MoveTo(area.left, area.bottom);
    dc.LineTo(area.right, area.bottom);

    dc.SelectObject(pOld);

    // Y축 눈금 텍스트
    CFont font;
    font.CreatePointFont(80, _T("Arial"));
    CFont* pOldFont = dc.SelectObject(&font);
    dc.SetTextColor(RGB(0, 0, 0));
    dc.SetBkMode(TRANSPARENT);

    const int labelCount = 10;
    for (int i = 0; i <= labelCount; ++i)
    {
        int y = area.bottom - i * area.Height() / labelCount;
        int altitudeValue = static_cast<int>((maxAltitude * i) / labelCount);
        CString str;
        str.Format(_T("%d"), altitudeValue);
        dc.TextOutW(area.left - 45, y - 8, str);
    }

    dc.SelectObject(pOldFont);
}