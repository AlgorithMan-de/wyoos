 

#include <net/tcp.h>

using namespace myos;
using namespace myos::common;
using namespace myos::net;



TransmissionControlProtocolHandler::TransmissionControlProtocolHandler()
{
}

TransmissionControlProtocolHandler::~TransmissionControlProtocolHandler()
{
}

void TransmissionControlProtocolHandler::HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size)
{
}





TransmissionControlProtocolSocket::TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend)
{
    this->backend = backend;
    handler = 0;
}

TransmissionControlProtocolSocket::~TransmissionControlProtocolSocket()
{
}

void TransmissionControlProtocolSocket::HandleTransmissionControlProtocolMessage(uint8_t* data, uint16_t size)
{
    if(handler != 0)
        handler->HandleTransmissionControlProtocolMessage(this, data, size);
}

void TransmissionControlProtocolSocket::Send(uint8_t* data, uint16_t size)
{
    backend->Send(this, data, size);
}

void TransmissionControlProtocolSocket::Disconnect()
{
    backend->Disconnect(this);
}





TransmissionControlProtocolProvider::TransmissionControlProtocolProvider(InternetProtocolProvider* backend)
: InternetProtocolHandler(backend, 0x06)
{
    for(int i = 0; i < 65535; i++)
        sockets[i] = 0;
    numSockets = 0;
    freePort = 1024;
}

TransmissionControlProtocolProvider::~TransmissionControlProtocolProvider()
{
}





bool TransmissionControlProtocolProvider::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE,
                                        uint8_t* internetprotocolPayload, uint32_t size)
{
  /*  
        for(uint16_t i = 0; i < numSockets && socket == 0; i++)
        if(sockets[i] == socket)
        {
            sockets[i] = sockets[--numSockets];
            MemoryManager::activeMemoryManager->free(socket);
            break;
        }
*/
    
    
    if(size < sizeof(TransmissionControlProtocolHeader))
        return false;
    
    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)internetprotocolPayload;
    uint16_t localPort = msg->dstPort;
    uint16_t remotePort = msg->srcPort;
    
    
    TransmissionControlProtocolSocket* socket = 0;
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
        socket->HandleTransmissionControlProtocolMessage(internetprotocolPayload + sizeof(TransmissionControlProtocolHeader),
                                                  size - sizeof(TransmissionControlProtocolHeader));
    
    return false;
}













// ------------------------------------------------------------------------------------------



void TransmissionControlProtocolProvider::Send(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size, uint16_t flags)
{
    uint16_t totalLength = size + sizeof(TransmissionControlProtocolHeader);
    uint8_t* buffer = (uint8_t*)MemoryManager::activeMemoryManager->malloc(totalLength);
    uint8_t* buffer2 = buffer + sizeof(TransmissionControlProtocolHeader);
    
    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)buffer;
    
    msg->srcPort = socket->localPort;
    msg->dstPort = socket->remotePort;
    msg->length = ((totalLength & 0x00FF) << 8) | ((totalLength & 0xFF00) >> 8);
    
    for(int i = 0; i < size; i++)
        buffer2[i] = data[i];
    
    msg -> checksum = 0;
    InternetProtocolHandler::Send(socket->remoteIP, buffer, totalLength);

    MemoryManager::activeMemoryManager->free(buffer);
}



TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Connect(uint32_t ip, uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));
    
    if(socket != 0)
    {
        new (socket) TransmissionControlProtocolSocket(this);
        
        socket -> remotePort = port;
        socket -> remoteIP = ip;
        socket -> localPort = freePort++;
        socket -> localIP = backend->GetIPAddress();
        
        socket -> remotePort = ((socket -> remotePort & 0xFF00)>>8) | ((socket -> remotePort & 0x00FF) << 8);
        socket -> localPort = ((socket -> localPort & 0xFF00)>>8) | ((socket -> localPort & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
        socket -> state = SYN_SENT;
        
        socket -> sequenceNumber = 0xbeefcafe;
        
        Send(socket, 0,0, SYN);
    }
    
    return socket;
}



void TransmissionControlProtocolProvider::Disconnect(TransmissionControlProtocolSocket* socket)
{
    socket->state = FIN_WAIT1;
    Send(socket, 0,0, FIN + ACK);
    socket->sequenceNumber++;
}


TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Listen(uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));
    
    if(socket != 0)
    {
        new (socket) TransmissionControlProtocolSocket(this);
        
        socket -> state = LISTENING;
        socket -> localIP = backend->GetIPAddress();
        socket -> localPort = ((port & 0xFF00)>>8) | ((port & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
    }
    
    return socket;
}
void TransmissionControlProtocolProvider::Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler)
{
    socket->handler = handler;
}





