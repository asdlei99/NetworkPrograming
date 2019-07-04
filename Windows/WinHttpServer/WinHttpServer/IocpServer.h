#ifndef __IOCP_SERVER_H__
#define __IOCP_SERVER_H__

#include <list>
#include <vector>

struct IoContext;
struct ClientContext;

class IocpServer
{
public:

    IocpServer(short listenPort);
    IocpServer(const IocpServer&) = delete;
    IocpServer& operator=(const IocpServer&) = delete;
    ~IocpServer();

    bool start();
    bool stop();
    //bool init();
    //bool unInit();

protected:
    //必须要static _beginthreadex才能访问
    static unsigned WINAPI IocpWorkerThread(LPVOID arg);

    HANDLE associateWithCompletionPort(ClientContext* pClient);
    bool getAcceptExPtr();
    bool getAcceptExSockaddrs();
    bool setKeepAlive(SOCKET s, IoContext* pIoCtx, int time = 30, int interval = 10);

    bool createListenClient(short listenPort);
    bool createIocpWorker();
    bool exitIocpWorker();

    bool postAccept(IoContext* pIoCtx);
    bool postRecv(IoContext* pIoCtx);
    bool postSend(IoContext* pIoCtx);
    bool postParse(ClientContext* pConnClient, IoContext* pIoCtx);

    bool handleAccept(ClientContext* pListenClient, IoContext* pIoCtx);
    bool handleRecv(ClientContext* pConnClient, IoContext* pIoCtx);
    bool handleSend(ClientContext* pConnClient, IoContext* pIoCtx);
    bool handleParse(ClientContext* pConnClient, IoContext* pIoCtx);
    bool handleClose(ClientContext* pConnClient, IoContext* pIoCtx);

    //线程安全
    void addClient(ClientContext* pConnClient);
    void removeClient(ClientContext* pConnClient);
    void removeClients();

    bool decodePacket();

    void echo(ClientContext* pConnClient);


private:
    short                       m_listenPort;
    HANDLE                      m_hComPort;              //完成端口
    HANDLE                      m_hExitEvent;           //退出线程事件
    std::vector<HANDLE>         m_hWorkerThreads;       //工作线程句柄列表
    ClientContext*              m_pListenClient;
    std::list<ClientContext*>   m_connList;             //已连接客户端列表
    CRITICAL_SECTION            m_csConnList;           //保护连接列表

    void*                       m_lpfnAcceptEx;		    //acceptEx函数指针
    void*                       m_lpfnGetAcceptExAddr;  //GetAcceptExSockaddrs函数指针

    int                         m_nWorkerCnt;           //io工作线程数量
};

#endif // !__IOCP_SERVER_H__

