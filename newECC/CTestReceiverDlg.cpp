//#include "pch.h"
//#include "CTestReceiverDlg.h"
//#include "afxdialogex.h"
//
//IMPLEMENT_DYNAMIC(CTestReceiverDlg, CDialogEx)
//
//CTestReceiverDlg::CTestReceiverDlg(CWnd* pParent)
//	: CDialogEx(IDD_TEST_RECEIVER_DLG, pParent)
//{
//}
//
//CTestReceiverDlg::~CTestReceiverDlg()
//{
//}
//
//void CTestReceiverDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
//}
//
//BEGIN_MESSAGE_MAP(CTestReceiverDlg, CDialogEx)
//END_MESSAGE_MAP()
//
//void CTestReceiverDlg::receive(int len, const char* packet)
//{
//	CommonMessage msg = DeserializeCommonMessage((const uint8_t*)packet, len);
//	HandleParsedMessage(msg);
//}
//
//void CTestReceiverDlg::HandleParsedMessage(const CommonMessage& msg)
//{
//	for (const auto& item : msg.payload)
//	{
//		std::visit([&](auto&& typedMsg) {
//			using T = std::decay_t<decltype(typedMsg)>;
//
//			CString line;
//			if constexpr (std::is_same_v<T, LS::Message>) {
//				line.Format(_T("[LS] ID=%u Pos=(%lld,%lld) Angle=%.1f Mode=%d Speed=%d"),
//					typedMsg.data.id, typedMsg.data.position.x, typedMsg.data.position.y,
//					typedMsg.data.angle, static_cast<int>(typedMsg.data.mode), typedMsg.data.speed);
//			}
//			else if constexpr (std::is_same_v<T, MFR::Message>) {
//				line.Format(_T("[MFR] ID=%u Pos=(%lld,%lld) Angle=%.1f Mode=%d"),
//					typedMsg.data.id, typedMsg.data.position.x, typedMsg.data.position.y,
//					typedMsg.data.angle, static_cast<int>(typedMsg.data.mode));
//			}
//			else if constexpr (std::is_same_v<T, LC::Message>) {
//				line.Format(_T("[LC] ID=%u Pos=(%lld,%lld)"),
//					typedMsg.data.id, typedMsg.data.position.x, typedMsg.data.position.y);
//			}
//			else if constexpr (std::is_same_v<T, Missile::Message>) {
//				line.Format(_T("[Missile] ID=%u Pos=(%lld,%lld) Z=%lld Speed=%lld Angle=%.1f Time=%d"),
//					typedMsg.data.id, typedMsg.data.position.x, typedMsg.data.position.y,
//					typedMsg.data.z, typedMsg.data.speed, typedMsg.data.angle, typedMsg.data.shoot_down_time);
//			}
//			else if constexpr (std::is_same_v<T, Target::Message>) {
//				for (const auto& t : typedMsg.data) {
//					line.Format(_T("[Target] ID=%u Pos=(%lld,%lld) Z=%lld Speed=%lld Angle=%.1f Priority=%d"),
//						t.id, t.position.x, t.position.y, t.z, t.speed, t.angle, t.shoot_down_time);
//					AppendLog(line);
//				}
//				return;
//			}
//			AppendLog(line);
//			}, item);
//	}
//}
//
//void CTestReceiverDlg::AppendLog(const CString& line)
//{
//	m_editLog.SetSel(-1, -1);
//	m_editLog.ReplaceSel(line + _T("\r\n"));
//}
