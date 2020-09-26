//Oh ye btw *X*X means not implemented or not done yet : Jamie/heatxd
//-----------------------------------------------------------------------------
#ifndef PLEASE_UNDO_H
#define PLEASE_UNDO_H
#define ENET_IMPLEMENTATION
#include "enet.h"
#include <stdio.h>
//PLEASE_UNDO DEBUG
#define SHOW_DEBUG 1 // Show Debug messages
//Declaration Constants
#define MAX_ROLLBACK_FRAMES 10 // Specifies the maximum number of frames that can be resimulated
#define FRAME_ADVANTAGE_LIMIT 2 // Only allow the local client to get so far ahead of remote
#define INITIAL_FRAME 0 //Specifies the initial frame the game starts in. Cannot rollback before this frame
#define ENET_CHANNELS 2// 1 for gameplay and 2 for text messages
#define DEFAULT_PORT 9090
#define ENET_CHANNELS 2
#define MAX_PEERS 10
//Declaration Structs and enums
typedef enum PU_PLAYER_TYPE{
  PLAYER_HOST = 1,
  PLAYER_CLIENT = 2,
  PLAYER_SPECTATOR = 3
}PU_PLAYER_TYPE;

typedef struct PU_SESSION{
  PU_PLAYER_TYPE local_player_type;
  ENetHost* local_player_host;
  ENetEvent local_client_event;
  ENetPeer* host_peer;
}PU_SESSION;
//Declaration Funcs
void pu_disconnect_from_host(PU_SESSION *session, ENetHost* client);
void pu_destroy_client(ENetHost* client);
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address);
ENetHost* pu_create_client(PU_SESSION *session);
void pu_update_network(PU_SESSION *session, ENetHost* player);
ENetHost* pu_create_host(PU_SESSION *session);
void pu_deinitialize();
int pu_initialize();
void pu_log(const char* message);
void pu_destroy_host(ENetHost* host, PU_SESSION *session);
//-----------------------------------------------------------------------------
// Implementation
#ifdef PLEASE_UNDO_IMPL_H
void pu_update_network(PU_SESSION *session, ENetHost* player){
  ENetEvent event;
  switch (session->local_player_type) {
    case PLAYER_HOST:
      while (enet_host_service(player, &event, 0) > 0) {
        switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", event.peer -> data);
            event.peer->data = NULL;
            break;
        }
      }
      break;
    case PLAYER_CLIENT:
      while (enet_host_service(player ,&session->local_client_event, 0) > 0) {
        switch (session->local_client_event.type) {
          case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
            session->local_client_event.packet->dataLength,
            session->local_client_event.packet->data,
            session->local_client_event.peer->address.host,
            session->local_client_event.peer->address.port,
            session->local_client_event.channelID);
            break;
        }
      }
      break;
  }
}
void pu_destroy_client(ENetHost* client){
  enet_host_destroy(client);
  pu_log("Client destroyed!\n");
}
void pu_disconnect_from_host(PU_SESSION *session, ENetHost* client){
  enet_peer_disconnect(session->host_peer, 0);
  while (enet_host_service(client, &session->local_client_event, 3000) > 0) {
    switch (session->local_client_event.type) {
      case ENET_EVENT_TYPE_RECEIVE:
        enet_packet_destroy(session->local_client_event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        pu_log("Disconnection succeeded.\n");
        break;
    }
  }
}
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address){
  ENetAddress address = {0};
  enet_address_set_host(&address, ip_address);
  address.port = DEFAULT_PORT;
  session->host_peer = enet_host_connect(client, &address, ENET_CHANNELS, 0);
  if (session->host_peer == NULL) {
    pu_log("No available peers for initiating an ENet connection!\n");
    return 0;
  }
  if (enet_host_service(client, &session->local_client_event, 5000) > 0 && session->local_client_event.type == ENET_EVENT_TYPE_CONNECT) {
    pu_log("Connection succesfull\n");
    return 1;
  }else{
    enet_peer_reset(session->host_peer);
    pu_log("Connection failed\n");
    return 0;
  }
}
ENetHost* pu_create_client(PU_SESSION *session){
  ENetHost* client;
  session->local_player_type = PLAYER_CLIENT;
  client = enet_host_create(NULL, 1, ENET_CHANNELS, 0, 0);
  if (client == NULL) {
    pu_log("An error occurred while trying to create an ENet client!\n");
    return client;
  }
  pu_log("Client created!\n");
  return client;
}
void pu_destroy_host(ENetHost* host, PU_SESSION *session){
  session->local_player_host = NULL;
  enet_host_destroy(host);
  pu_log("Host destroyed!\n");
}
ENetHost* pu_create_host(PU_SESSION *session){
  ENetEvent event;
  ENetHost* host;
  ENetAddress address = {0};
  address.host = ENET_HOST_ANY;
  address.port = DEFAULT_PORT;
  host = enet_host_create(&address, MAX_PEERS, ENET_CHANNELS, 0, 0);
  if (host == NULL) {
    pu_log("An error occurred while trying to create an ENet server host.\n");
    return host;
  }
  session->local_player_type = PLAYER_HOST;
  session->local_player_host = host;
  pu_log("Host created!\n");
  return host;
}
void pu_log(const char* message){
  if (SHOW_DEBUG) {
    fprintf (stderr, message);
  }
}
int pu_initialize(){
  if (enet_initialize() != 0) {
    pu_log("An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }
  atexit(pu_deinitialize);
  pu_log("Initialized Enet and Please Undo.\n");
}
void pu_deinitialize(){
  enet_deinitialize();
  pu_log("Enet and Please Undo deinitialized.\n");
}
#endif //PLEASE_UNDO_IMPL_H

#endif //PLEASE_UNDO_H
