
// SAMtestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "SAMtest.h"
#include "SAMtestDlg.h"
#include "afxdialogex.h"
#include "SendMessageTypes.h"
#include "Serializer.h"
#include "ReceiveMessageTypes.h"
#include "Deserializer.h"
#include "PacketParser.h"
#include "Config.h"

#include <iostream>
#include <variant>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSAMtestDlg 대화 상자



CSAMtestDlg::CSAMtestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SAMTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSAMtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSAMtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSAMtestDlg 메시지 처리기

BOOL CSAMtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	const int margin = 10;

	// 사용자 정의 창 크기 설정
	const int desiredWidth = 2000;
	const int desiredHeight = 900;

	// 화면 중앙에 배치
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	int posX = (screenWidth - desiredWidth) / 2;
	int posY = (screenHeight - desiredHeight) / 2;

	this->SetWindowPos(
		NULL,
		posX, posY,
		desiredWidth, desiredHeight,
		SWP_NOZORDER | SWP_SHOWWINDOW
	);

	// 다이얼로그 스타일에서 최대화, 테두리 제거 안함

	// 각 영역 크기 재계산
	int leftWidth = desiredWidth * 4 / 14;
	int centerWidth = desiredWidth * 6 / 14;
	int rightWidth = desiredWidth * 4 / 14;
	int halfHeight = desiredHeight / 2;

	/*m_lcControlDlg.Create(IDD_LC_INFO_DLG, this);
	m_lcControlDlg.ShowWindow(SW_SHOW);*/

	// 왼쪽 위
	m_leftTop.Create(IDD_LEFT_TOP_DLG, this);
	m_leftTop.MoveWindow(0, 0, leftWidth, halfHeight);
	m_leftTop.ShowWindow(SW_SHOW);

	// 왼쪽 아래
	m_leftBottom.Create(IDD_LEFT_BOTTOM_DLG, this);
	m_leftBottom.MoveWindow(0, halfHeight, leftWidth, halfHeight);
	m_leftBottom.ShowWindow(SW_SHOW);

	// 중앙 위
	m_rightPane.Create(IDD_RIGHT_PANE_DLG, this);
	m_rightPane.MoveWindow(leftWidth, 0, centerWidth, halfHeight);
	m_rightPane.ShowWindow(SW_SHOW);

	// 중앙 아래
	m_mockTrack.Create(IDD_MOCK_TRACK, this);
	m_mockTrack.MoveWindow(leftWidth, halfHeight, centerWidth, halfHeight);
	m_mockTrack.ShowWindow(SW_SHOW);

	// 오른쪽 전체
	m_targetListDlg.Create(IDD_TARGET_LIST_DLG, this);
	m_targetListDlg.MoveWindow(leftWidth + centerWidth, 0, rightWidth, desiredHeight);
	m_targetListDlg.ShowWindow(SW_SHOW);

	// 자식 다이얼로그에 부모 전달
	m_leftTop.SetParentDlg(this);
	m_leftBottom.SetParentDlg(this);

	// 설정 파일 로드
	ConfigCommon config;
	if (!loadConfig("ECCconfig.ini", config))
	{
		AfxMessageBox(_T("설정 파일을 읽어오는 데 실패했습니다."));
		return FALSE;
	}

	m_tcp = std::make_unique<ECC_TCP>();
	if (!m_tcp->connect(config.LCSendIP.c_str(), config.LCSendPort))
	{
		CString msg;
		msg.Format(_T("서버 연결 실패\nIP: %S\nPort: %d"),
			config.LCSendIP.c_str(), config.LCSendPort);
		AfxMessageBox(msg);
		return FALSE;
	}

	m_tcp->registerReceiver(this);
	m_tcp->startReceiving();

	SetTimer(TIMER_ID_REQUEST, 100, nullptr); // 100ms 주기

	return TRUE;
}

void CSAMtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSAMtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSAMtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSAMtestDlg::setGoalTargetId(int id)
{
	std::cout << "[setGoalTargetId] goal id: " << goalTargetId << ", " << id << std::endl;
	goalTargetId = id;
}

void CSAMtestDlg::receive(int len, const char* packet)
{
	if (len <= 0 || packet == nullptr)		
	{
		return;
	}

	try
	{
		ParsedPacket parsed = PacketParser::Parse(packet, static_cast<size_t>(len));

		std::visit([&](auto&& msg) {
			using T = std::decay_t<decltype(msg)>;

			if constexpr (std::is_same_v<T, ParsedStatusResponse>)
			{
				static bool printed = false;
				if (!printed)
				{
					printed = true;
				}

				// UI 반영
				m_leftTop.SetRadarList(msg.radarList);
				m_leftTop.SetTargetList(msg.targetList);

				m_leftBottom.SetLSList(msg.lsList);
				m_leftBottom.SetTargetList(msg.targetList);

				if (!msg.lcList.empty())
				{
					m_rightPane.SetLCStatus(msg.lcList[0]);
				}

				m_rightPane.SetMissileStatus(msg.missileList);
				m_rightPane.SetTargetList(msg.targetList);
				
				m_targetListDlg.SetTargetList(msg.targetList);

				if (!msg.missileList.empty() && goalTargetId != -1)
				{
					const auto& missile = msg.missileList[0];

					auto it = std::find_if(msg.targetList.begin(), msg.targetList.end(),
						[=](const TargetStatus& t) { return t.id == goalTargetId; });
					
					if (it != msg.targetList.end())
					{
						const auto& target = *it;
						auto now = std::chrono::steady_clock::now().time_since_epoch();
						double timeInSeconds = std::chrono::duration<double>(now).count();
						m_mockTrack.UpdateFromMock(missile, target, timeInSeconds);
					}
				}
			}

			else 
			{
				//std::cout << "[수신] ACK or 기타 패킷 수신됨\n";
			}
			}, parsed);
	}
	catch (const std::exception& ex)
	{
		//std::cerr << "[에러] 패킷 파싱 실패: " << ex.what() << "\n";
	}
}

void CSAMtestDlg::sendStatusRequest() {
	StatusRequest msg{};
	auto data = SerializeStatusRequest(msg);
	//std::cout << "[클라이언트] STATUS_REQUEST 전송 (0x01)\n";
	m_tcp->send(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}

void CSAMtestDlg::sendRadarModeChange(unsigned int radar_id, uint8_t mode, uint8_t priority_select, unsigned int target_id) {
	RadarModeChange msg{ CommandType::RADAR_MODE_CHANGE, radar_id, mode, priority_select, target_id };
	auto data = SerializeRadarModeChange(msg);
	m_tcp->send(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}

void CSAMtestDlg::sendLSModeChange(unsigned int ls_id, uint8_t mode) {
	LSModeChange msg{ CommandType::LS_MODE_CHANGE, ls_id, mode };
	auto data = SerializeLSModeChange(msg);
	//std::cout << "[클라이언트] LS_MODE_CHANGE 전송 (0x03)\n";
	m_tcp->send(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}

void CSAMtestDlg::sendMissileLaunch(unsigned int ls_id, unsigned int target_id) {
	MissileLaunch msg{ CommandType::MISSILE_LAUNCH, ls_id, target_id };
	auto data = SerializeMissileLaunch(msg);
	m_tcp->send(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}
void CSAMtestDlg::sendLSMove(unsigned int ls_id, Pos2D pos) {
	LSMove msg{ CommandType::LS_MOVE, ls_id, pos };
	auto data = SerializeLSMove(msg);
	m_tcp->send(reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()));
}


void CSAMtestDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_REQUEST) {
		sendStatusRequest();  // 주기적 상태 요청
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CSAMtestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(TIMER_ID_REQUEST);  // 타이머 해제
	if (m_tcp) {
		m_tcp->stop();  // 수신 스레드 종료
	}
}
