#include "cl_socket_base.h"
#include <cerrno>
#include <cstdio>

#ifndef _WIN32
#   include <unistd.h>
#   include <sys/select.h>
#   include <string.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif

clSocketBase::clSocketBase(int sockfd)
    : m_socket(sockfd)
    , m_closeOnExit(true)
{
}

clSocketBase::~clSocketBase()
{
    DestroySocket();
}

void clSocketBase::Initialize()
{
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

int clSocketBase::Read(char* buffer, size_t bufferSize, size_t& bytesRead, long timeout) throw (clSocketException)
{
    if ( SelectRead(timeout) == kTimeout ) {
        return kTimeout;
    }
    memset(buffer, 0, bufferSize);
    bytesRead = recv(m_socket, buffer, bufferSize, 0);
    return kSuccess;
}

int clSocketBase::SelectRead(long seconds) throw (clSocketException)
{
    if ( seconds == -1 ) {
        return kSuccess;
    }

    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }

    struct timeval tv = {seconds, 0};

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_socket, &readfds);
    int rc = select(m_socket+1, &readfds, NULL, NULL, &tv);
    if ( rc == 0 ) {
        // timeout
        return kTimeout;

    } else if ( rc < 0 ) {
        // an error occured
        throw clSocketException( "SelectRead failed: " + error() );

    } else {
        // we got something to read
        return kSuccess;
    }
}

void clSocketBase::Send(const std::string& msg) throw (clSocketException)
{
    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }
    ::send(m_socket, msg.c_str(), msg.length(), 0);
}

std::string clSocketBase::error() const
{
    std::string err;
#ifdef _WIN32
    char _buf[256];
    memset(_buf, 0, sizeof(_buf));
    sprintf(_buf, "WSAGetLastError returned: %d", WSAGetLastError());
    err = _buf;
#else
    err = strerror(errno);
#endif
    return err;
}

void clSocketBase::DestroySocket()
{
    if (IsCloseOnExit()) {
        if ( m_socket != INVALID_SOCKET ) {
#ifdef _WIN32
            ::closesocket( m_socket );
#else
            ::close(m_socket);
#endif
            ::shutdown(m_socket, 2);
        }
    }
    m_socket = INVALID_SOCKET;
}

int clSocketBase::ReadMessage(wxString& message, int timeout) throw (clSocketException)
{
    size_t message_len(0);
    size_t bytesRead(0);
    int rc = Read( (char*)&message_len, sizeof(message_len), bytesRead, timeout);

    if ( rc != kSuccess ) {
        return rc;
    }

    bytesRead = 0;
    char *buff = new char[message_len+1];
    memset(buff, 0, message_len+1);
    rc = Read(buff, message_len, bytesRead, timeout);
    if ( rc != kSuccess ) {
        wxDELETEA( buff );
        return rc;
    }

    if ( bytesRead != message_len ) {
        wxDELETEA( buff );
        throw clSocketException("Wrong message length received");
    }

    buff[message_len] = '\0';
    message = buff;
    return kSuccess;
}

void clSocketBase::WriteMessage(const wxString& message) throw (clSocketException)
{
    if ( m_socket == INVALID_SOCKET ) {
        throw clSocketException("Invalid socket!");
    }

    // Write the message length
    std::string c_str = message.mb_str(wxConvUTF8).data();
    size_t len = c_str.length();

    ::send(m_socket, (const char*)&len, sizeof(len), 0);

    // now send the actual data
    Send(c_str);
}

socket_t clSocketBase::Release()
{
    int fd = m_socket;
    m_socket = INVALID_SOCKET;
    return fd;
}
