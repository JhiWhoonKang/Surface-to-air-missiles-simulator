//#pragma once
//#include <afxsock.h>
//#include "IReceiver.h"
//
//class CTcpClient
//{
//public:
//    CTcpClient();
//    ~CTcpClient();
//
//    BOOL Connect(const CString& ip, UINT port);
//    void SendData(const CString& data);
//    void SetReceiver(IReceiver* receiver);
//    void StartReceiving();
//    void Disconnect();
//
//private:
//    static UINT ReceiveThread(LPVOID pParam);
//
//private:
//    SOCKET m_socket;
//    CWinThread* m_pThread;
//    IReceiver* m_pReceiver;
//    BOOL m_bRunning;
//};
