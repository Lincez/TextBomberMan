#ifndef LSOCKET_H
#define LSOCKET_H
#include <winsock.h>
#include "../Vector/LString.h"
#include "../Exception/LException.h"
#include "../Stream/LStream.h"

int winsockSend(SOCKET sock, const char* s, int i ,int j);
int winsockConnect(SOCKET sock, const struct sockaddr* addr, int i);
int winsockAccept(SOCKET sock, struct sockaddr* addr, int* i);
int winsockListen(SOCKET sock, int i);
int winsockBind(SOCKET sock, const struct sockaddr* addr, int i);

class SocketException : public LException
{
    public:
        SocketException() : LException("Socket couldn't be started")
        {
        }
        SocketException(char*s) : LException(s)
        {
        }
};

class BindException : public SocketException
{
    public:
        BindException()
        {
            msg = "Server couldn't bind";
            details = "bind() failed";
        }
        BindException(SOCKET sock, struct sockaddr* addr_, int i) : SocketException("Server couldn't bind")
        {
            details = "bind(";
            details += (int)sock;
            details += ", addr, ";
            details += (int)i;
            details += ") failed";
            this->addr = addr_;
        }
    struct sockaddr* addr;
};

class ListenException : public SocketException
{
    public:
        ListenException() : SocketException("Server couldn't start listening")
        {
            details = "listen() failed";
        }
};

class SendException : public SocketException
{
    public:
        SendException() : SocketException("Socket sended a different number of bytes")
        {
        }
};

class ReceiveException : public SocketException
{
    public:
        ReceiveException() : SocketException("Socket couldn't receive data")
        {
        }
};

class ConnectException : public SocketException
{
    public:
        ConnectException() : SocketException("Socket couldn't connect")
        {
        }
};

class WSAStartupException : public SocketException
{
    public:
        WSAStartupException() : SocketException("WSAStartup() failed")
        {

        }
};

class AcceptException : public SocketException
{
    public:
        AcceptException() : SocketException("Server couldn't accept client")
        {
        }
};

class LBasicSocket
{
    public:
        LBasicSocket()
        {
            WinStartup();
            this->sock = 0;
        }
        void close()
        {
            if (sock != 0)
                closesocket(sock);
            sock = 0;
        }
        void setSock(int s) {this->sock = s;}
        struct sockaddr_in* getAddr() {return &this->addr;}
        char* getIp() {return inet_ntoa(getAddr()->sin_addr);}
        int getPort() {return this->port;}
        int getSock() {return this->sock;}
        virtual ~LBasicSocket()
        {
            this->close();
            WSACleanup();   /* Cleanup Winsock */
        }

        int getLastError()
        {
            return WSAGetLastError();
        }
    protected:
        void createSocket()
        {
            /* Create A reliable, stream socket using TCP */
            if ((this->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            {
                throw new SocketException;
//                dieWithError("socket() failed");
            }
        }

        WSADATA wsaData;                    /* Structure for WinSock setup communication */
        void WinStartup()
        {
            if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            {
                throw new WSAStartupException;
            }
        }

        int sock;                           /* Socket descriptor */

        struct sockaddr_in addr;        /** Local Addr */
        unsigned short port;            /* Server port */
};

class LSocket : public LBasicSocket, public LTextInputStream, public LTextOutputStream
{
    public:
        /// Input
        void* receive(int length)
        {
            char* s = (char*) calloc(sizeof(char), length + 1);
            memset(s, 0, sizeof(char) * (length + 1));
            if ((recv(this->sock, s, length, 0)) < 0)
            {
                throw new ReceiveException;
            }
            return (void*) s;
        }
        int getChar()
        {
            char* s = (char*)receive(1);
            int rt = s[0];
            free(s);
            return rt;
        }
        char* read(size_t len)
        {
            return (char*) receive(len);
        }

        /// Output
        void send(const void*data, int len)
        {
            if (this->sock == 0) throw new SocketException("Socket not prepared to send");

            /* Send the string to the server, including the null terminator */
            if (winsockSend(sock, (char*)data, len, 0) != len)
            {
                throw new SendException;
            }
        }
        void write(const char* data)
        {
            send((const char*)data, strlen(data));
        }
        void write(const void* data, size_t len)
        {
            send(data, len);
        }
        void append(const char* data)
        {
            write(data);
        }

};

class LClientSocket : public LSocket
{
    public:
        LClientSocket()
        {
            this->clientLen = sizeof(addr);
        }
        LClientSocket(const char*serverIp, int port)
        {
            this->clientLen = sizeof(addr);
            this->connect(serverIp, port);
        }
        LClientSocket(LString*serverIp, int port)
        {
            this->clientLen = sizeof(addr);
            this->connect(serverIp, port);
        }

        void connect(const char*serverIp, int port)
        {
            this->setServerIp(serverIp);
            this->port = port;
            this->createSocket();
            this->makeServerAddress();
            if (winsockConnect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
            {
                throw new ConnectException;
//                dieWithError("connect() failed");
            }
        }
        void connect(LString* serverIp, int port)
        {
            this->connect(serverIp->c_str(), port);
        }

        int noThrowConnect(const char* serverIp, int port)
        {
            setServerIp(serverIp);
            this->port = port;
            this->createSocket();
            this->makeServerAddress();
            int rt = winsockConnect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr));
            return rt;
        }

        unsigned int* getClientLen() {return &this->clientLen;}
    protected:
        void setServerIp(const char* ip_)
        {
            this->servIp = ip_;
            if (this->servIp.equalsI("localhost"))
            {
                this->servIp = "127.0.0.1";
            }
        }
        void makeServerAddress()
        {
            /* Construct the server address structure */
            memset(&servAddr, 0, sizeof(servAddr));
            servAddr.sin_family = AF_INET;
            servAddr.sin_addr.s_addr = inet_addr(servIp.c_str());
            servAddr.sin_port = htons(this->port);
        }
        struct sockaddr_in servAddr;        /* Server address */
        LString servIp;                     /* Server Ip address (dotted quad)*/

        unsigned int clientLen;         /** Local Addr len */
};

class LServerSocket : public LBasicSocket
{
    public:
        LServerSocket()
        {
            this->maxPending = 5;
        }
        LServerSocket(int port_)
        {
            this->maxPending = 5;
            this->open(port_);
        }
        void open(int port_)
        {
            this->port = port_;
            this->createSocket();
            this->makeLocalAddress();
            this->bind();
        }

        void listen()
        {
            if (this->sock == 0) throw new SocketException("Socket not prepared to listen");

            if (winsockListen(this->sock, this->maxPending) < 0)
            {
                throw new ListenException;
            }
        }
        LSocket* accept()
        {
            LClientSocket* aSock = new LClientSocket;
            int clntSock;
            /* wait for a client to connect*/
            if ((clntSock = winsockAccept(this->sock, (struct sockaddr *) aSock->getAddr(), (int*)aSock->getClientLen())) < 0)
            {
                throw new AcceptException;
            }
            aSock->setSock(clntSock);
            return aSock;
        }
        void setMaxPending(int i) {this->maxPending = i;}
    protected:
        void makeLocalAddress()
        {
            /* Construct local address structure */
            memset(&addr, 0, sizeof(addr));     /* Zero out  structure */
            addr.sin_family = AF_INET;                  /* Internet address family */
            addr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any incoming interface */
            addr.sin_port = htons(this->port);        /* Local port */
        }
        void bind()
        {
            /* Bind to the local address */
            if (winsockBind(this->sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            {
                BindException* e = new BindException(this->sock, (struct sockaddr *)&addr, sizeof(addr));
                throw e;
            }
        }
        int maxPending;
};

#endif
