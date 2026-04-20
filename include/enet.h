#ifndef ENET_H
#define ENET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char enet_uint8;
typedef unsigned short enet_uint16;
typedef unsigned int enet_uint32;

typedef struct _ENetAddress
{
    enet_uint32 host;
    enet_uint16 port;
} ENetAddress;

typedef enum _ENetEventType
{
    ENET_EVENT_TYPE_NONE,
    ENET_EVENT_TYPE_CONNECT,
    ENET_EVENT_TYPE_DISCONNECT,
    ENET_EVENT_TYPE_RECEIVE
} ENetEventType;

typedef struct _ENetPeer ENetPeer;
typedef struct _ENetHost ENetHost;

typedef struct _ENetPacket
{
    size_t dataLength;
    enet_uint8 data[1];
} ENetPacket;

typedef struct _ENetEvent
{
    ENetEventType type;
    ENetPeer* peer;
    enet_uint32 data;
    ENetPacket* packet;
} ENetEvent;

#define ENET_HOST_ANY               0
#define ENET_PACKET_FLAG_RELIABLE   1

int enet_initialize(void);
void enet_deinitialize(void);

ENetHost* enet_host_create(const ENetAddress* address, size_t peerCount, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth);
void enet_host_destroy(ENetHost* host);
int enet_host_service(ENetHost* host, ENetEvent* event, enet_uint32 timeout);
void enet_host_broadcast(ENetHost* host, enet_uint8 channelID, ENetPacket* packet);

ENetPeer* enet_host_connect(ENetHost* host, const ENetAddress* address, size_t channelCount, enet_uint32 data);

ENetPacket* enet_packet_create(const void* data, size_t dataLength, enet_uint32 flags);
void enet_packet_destroy(ENetPacket* packet);

int enet_peer_send(ENetPeer* peer, enet_uint8 channelID, ENetPacket* packet);
void enet_address_set_host(ENetAddress* address, const char* hostName);

#ifdef __cplusplus
}
#endif

#endif