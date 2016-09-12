
#include <net/udp.h>

using namespace myos;
using namespace myos::common;
using namespace myos::net;



UserDatagramProtocolHandler::UserDatagramProtocolHandler()
{
}

UserDatagramProtocolHandler::~UserDatagramProtocolHandler()
{
}

void UserDatagramProtocolHandler::HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, uint8_t* data, uint16_t size)
{
}





UserDatagramProtocolSocket::UserDatagramProtocolSocket(UserDatagramProtocolProvider* backend)
{
    this->backend = backend;
    handler = 0;
    listening = false;
}

UserDatagramProtocolSocket::~UserDatagramProtocolSocket()
{
}

void UserDatagramProtocolSocket::HandleUserDatagramProtocolMessage(uint8_t* data, uint16_t size)
{
    if(handler != 0)
        handler->HandleUserDatagramProtocolMessage(this, data, size);
}

void UserDatagramProtocolSocket::Send(uint8_t* data, uint16_t size)
{
    backend->Send(this, data, size);
}

void UserDatagramProtocolSocket::Disconnect()
{
    backend->Disconnect(this);
}





UserDatagramProtocolProvider::UserDatagramProtocolProvider(InternetProtocolProvider* backend)
: InternetProtocolHandler(backend, 0x11)
{
    for(int i = 0; i < 65535; i++)
        sockets[i] = 0;
    numSockets = 0;
    freePort = 1024;
}

UserDatagramProtocolProvider::~UserDatagramProtocolProvider()
{
}

bool UserDatagramProtocolProvider::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE,
                                        uint8_t* internetprotocolPayload, uint32_t size)
{
    if(size < sizeof(UserDatagramProtocolHeader))
        return false;
    
    UserDatagramProtocolHeader* msg = (UserDatagramProtocolHeader*)internetprotocolPayload;
    uint16_t localPort = msg->dstPort;
    uint16_t remotePort = msg->srcPort;
    
    
    UserDatagramProtocolSocket* socket = 0;
    for(uint16_t i = 0; i < numSockets && socket == 0; i++)
    {
        if( sockets[i]->localPort == msg->dstPort
        &&  sockets[i]->localIP == dstIP_BE
        &&  sockets[i]->listening)
        {
            socket = sockets[i];
            socket->listening = false;
            socket->remotePort = msg->srcPort;
            socket->remoteIP = srcIP_BE;
        }
        
        else if( sockets[i]->localPort == msg->dstPort
        &&  sockets[i]->localIP == dstIP_BE
        &&  sockets[i]->remotePort == msg->srcPort
        &&  sockets[i]->remoteIP == srcIP_BE)
            socket = sockets[i];
    }
    
    if(socket != 0)
        socket->HandleUserDatagramProtocolMessage(internetprotocolPayload + sizeof(UserDatagramProtocolHeader),
                                                  size - sizeof(UserDatagramProtocolHeader));
    
    return false;
}


UserDatagramProtocolSocket* UserDatagramProtocolProvider::Connect(uint32_t ip, uint16_t port)
{
    UserDatagramProtocolSocket* socket = (UserDatagramProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(UserDatagramProtocolSocket));
    
    if(socket != 0)
    {
        new (socket) UserDatagramProtocolSocket(this);
        
        socket -> remotePort = port;
        socket -> remoteIP = ip;
        socket -> localPort = freePort++;
        socket -> localIP = backend->GetIPAddress();
        
        socket -> remotePort = ((socket -> remotePort & 0xFF00)>>8) | ((socket -> remotePort & 0x00FF) << 8);
        socket -> localPort = ((socket -> localPort & 0xFF00)>>8) | ((socket -> localPort & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
    }
    
    return socket;
}



UserDatagramProtocolSocket* UserDatagramProtocolProvider::Listen(uint16_t port)
{
    UserDatagramProtocolSocket* socket = (UserDatagramProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(UserDatagramProtocolSocket));
    
    if(socket != 0)
    {
        new (socket) UserDatagramProtocolSocket(this);
        
        socket -> listening = true;
        socket -> localPort = port;
        socket -> localIP = backend->GetIPAddress();
        
        socket -> localPort = ((socket -> localPort & 0xFF00)>>8) | ((socket -> localPort & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
    }
    
    return socket;
}


void UserDatagramProtocolProvider::Disconnect(UserDatagramProtocolSocket* socket)
{
    for(uint16_t i = 0; i < numSockets && socket == 0; i++)
        if(sockets[i] == socket)
        {
            sockets[i] = sockets[--numSockets];
            MemoryManager::activeMemoryManager->free(socket);
            break;
        }
}

void UserDatagramProtocolProvider::Send(UserDatagramProtocolSocket* socket, uint8_t* data, uint16_t size)
{
    uint16_t totalLength = size + sizeof(UserDatagramProtocolHeader);
    uint8_t* buffer = (uint8_t*)MemoryManager::activeMemoryManager->malloc(totalLength);
    uint8_t* buffer2 = buffer + sizeof(UserDatagramProtocolHeader);
    
    UserDatagramProtocolHeader* msg = (UserDatagramProtocolHeader*)buffer;
    
    msg->srcPort = socket->localPort;
    msg->dstPort = socket->remotePort;
    msg->length = ((totalLength & 0x00FF) << 8) | ((totalLength & 0xFF00) >> 8);
    
    for(int i = 0; i < size; i++)
        buffer2[i] = data[i];
    
    msg -> checksum = 0;
    InternetProtocolHandler::Send(socket->remoteIP, buffer, totalLength);

    MemoryManager::activeMemoryManager->free(buffer);
}

void UserDatagramProtocolProvider::Bind(UserDatagramProtocolSocket* socket, UserDatagramProtocolHandler* handler)
{
    socket->handler = handler;
}





