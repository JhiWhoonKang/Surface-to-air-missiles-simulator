//#pragma once
//#include "afxdialogex.h"
//#include "IReceiver.h"
//#include "CommonMessage.h"
//#include "MessageDeserializer.h"
//#include "resource.h"
//
//class CTestReceiverDlg : public CDialogEx, public IReceiver
//{
//	DECLARE_DYNAMIC(CTestReceiverDlg)
//
//public:
//	CTestReceiverDlg(CWnd* pParent = nullptr);
//	virtual ~CTestReceiverDlg();
//
//	// IReceiver 구현
//	virtual void receive(int len, const char* packet) override;
//
//	enum { IDD = IDD_TEST_RECEIVER_DLG };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);
//	DECLARE_MESSAGE_MAP()
//
//private:
//	void HandleParsedMessage(const CommonMessage& msg);
//	void AppendLog(const CString& line);
//
//private:
//	CEdit m_editLog;
//};