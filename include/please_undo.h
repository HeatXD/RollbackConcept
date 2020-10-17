//Oh ye btw *X*X means not implemented or not done yet : Jamie/heatxd
//-----------------------------------------------------------------------------
#ifndef PLEASE_UNDO_H
#define PLEASE_UNDO_H
#define ENET_IMPLEMENTATION
#include "enet.h"
#include "cvector.h"
#include <stdio.h>
//PLEASE_UNDO DEBUG
#define SHOW_DEBUG 1// Show Debug messages
//Declaration Constants
#define MAX_ROLLBACK_FRAMES 10 // Specifies the maximum number of frames that can be resimulated
#define FRAME_ADVANTAGE_LIMIT 4 // Only allow the local client to get so far ahead of remote
#define INITIAL_FRAME 0 //Specifies the initial frame the game starts in. Cannot rollback before this frame
#define ENET_CHANNELS 2// ch1 for gameplay and ch2 for text messages
#define DEFAULT_PORT 9090
#define ENET_CHANNELS 2
#define MAX_PEERS 1
#define INPUT_RESERVSE_SPACE MAX_ROLLBACK_FRAMES*60
//Declaration Structs and enums
typedef enum PU_PLAYER_TYPE{
  PLAYER_HOST = 1,
  PLAYER_CLIENT = 2,
  PLAYER_SPECTATOR = 3// not implemented yet X*X*
}PU_PLAYER_TYPE;

typedef struct PU_INPUT_STORAGE{
  uint16_t* input_vector; // expecting gameinput to be a 16bit bitfield for now makes it easier to compare for now.
}PU_INPUT_STORAGE;

typedef struct PU_INPUT_PACKET{
  int frame_num;
  uint16_t input;
}PU_INPUT_PACKET;

typedef struct PU_SESSION{
  int local_frame;// Tracks the latest update frame.
  int remote_frame;// Tracks the latest frame received from the remote client
  int sync_frame;// Tracks the last frame where we synchronized the game state with the remote client. Never rollback before this frame
  int remote_frame_advantage;// Latest frame advantage received from the remote client
  //----------------------------------------------------------------------------
  PU_PLAYER_TYPE local_player_type;
  PU_INPUT_STORAGE player_input[3]; // 0 == local_player, 1 == remote_player, 2 == remote_player_predicted
  //----------------------------------------------------------------------------
  ENetHost* local_player_host;
  ENetEvent local_client_event;
  ENetPeer* host_peer;
  //----------------------------------------------------------------------------
  int has_started;
}PU_SESSION;
// Declaration Funcs
void pu_disconnect_from_host(PU_SESSION *session, ENetHost* client);
void pu_destroy_client(ENetHost* client);
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address);
ENetHost* pu_create_client(PU_SESSION *session);
void pu_update_network(PU_SESSION *session, ENetHost* player);
ENetHost* pu_create_host(PU_SESSION *session);
void pu_deinitialize(PU_SESSION *session);
int pu_initialize(PU_SESSION *session);
void pu_log(const char* message);
void pu_destroy_host(ENetHost* host, PU_SESSION *session);
void pu_send_input(PU_SESSION *session, ENetHost *player, uint16_t input);// send player input over the wire with its associated frame number
void pu_add_local_input(PU_SESSION *session, ENetHost *player, uint16_t input); //Add local input and send it
void pu_add_remote_input(PU_SESSION *session, uint16_t input);// Add Recieved input to input vector
void pu_predict_remote_input(PU_SESSION *session);//predict remote input if not yet available simply use the previous input.
void pu_determine_sync_frame(PU_SESSION *session);// Finds the last frame where the inputs were matched
int pu_rollback_condition(PU_SESSION *session); // No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_timesynced_condition(PU_SESSION *session);// Function for syncing both players making the other wait
void pu_handle_rollbacks(PU_SESSION *session)//X*X*
//-----------------------------------------------------------------------------
// Implementation
#ifdef PLEASE_UNDO_IMPL_H
// Please undo functions
//rollback sequence
void pu_handle_rollbacks(PU_SESSION *session){
//restore gamestate to sync frame
  for (int i = session->sync_frame + 1; i <= session->local_frame; i++) {
    //update input
    //advance gamestate
    //store gamestate
  }
}
//predict remote input if not yet available simply use the previous input.
void pu_predict_remote_input(PU_SESSION *session){
  if (session->player_input[1].input_vector[session->local_frame-1] == NULL) {
    session->player_input[2].input_vector[session->local_frame-1] = session->player_input[2].input_vector[session->local_frame-2];
    session->player_input[1].input_vector[session->local_frame-1] = session->player_input[2].input_vector[session->local_frame-2];
  }
};
// Add Recieved input to input vector
void pu_add_remote_input(PU_SESSION *session, uint16_t input){
  session->player_input[1].input_vector[session->remote_frame-1] = input;
  session->player_input[2].input_vector[session->remote_frame-1] = input;
};
// Add local input and send it
void pu_add_local_input(PU_SESSION *session, ENetHost *player, uint16_t input){
  if (vector_capacity(session->player_input[0].input_vector) < session->local_frame ) {
    vector_reserve(session->player_input[0].input_vector, vector_capacity(session->player_input[0].input_vector) + INPUT_RESERVSE_SPACE);
    vector_reserve(session->player_input[1].input_vector, vector_capacity(session->player_input[1].input_vector) + INPUT_RESERVSE_SPACE);
    vector_reserve(session->player_input[2].input_vector, vector_capacity(session->player_input[2].input_vector) + INPUT_RESERVSE_SPACE);
  }
  session->player_input[0].input_vector[session->local_frame-1] = input;
  //printf("input[%d] = %u\n", session->local_frame-1, session->player_input[0].input_vector[session->local_frame-1]);
  pu_send_input(session, player, input);
}
// send player input over the wire with its associated frame number
void pu_send_input(PU_SESSION *session, ENetHost *player, uint16_t input){
  PU_INPUT_PACKET data;

  data.frame_num = session->local_frame;
  data.input = input;

  ENetPacket* packet = enet_packet_create(&data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);

  if (session->local_player_type == PLAYER_HOST) {
    enet_host_broadcast(player, 0, packet);
  }else{
    enet_peer_send(session->host_peer, 0, packet);
  }
}
// X*X*
void pu_determine_sync_frame(PU_SESSION *session){
  int final_frame = session->remote_frame; // We will only check inputs until the remote_frame, since we don't have inputs after.
  int found_frame = INITIAL_FRAME - 1;

  if (session->remote_frame > session->local_frame) {
    final_frame = session->remote_frame;  // Incase the remote client is ahead of local, don't check past the local frame.
  }

  for (int i = session->sync_frame + 1; i <= final_frame; i++) {
    if (session->player_input[1].input_vector[i-1] != session->player_input[2].input_vector[i-1] ) {
        pu_log("Found wrong prediction!\n");
        found_frame = i;
        break;
    }
  }

  if (found_frame != INITIAL_FRAME-1) {
    session->sync_frame = found_frame - 1;
  }else{
    session->sync_frame = final_frame;
  }
}
// Function for syncing both players making the frame longer / slowing down
int pu_timesynced_condition(PU_SESSION *session){
  int local_frame_advantage = session->local_frame - session->remote_frame;// How far the client is ahead of the last reported frame by the remote client
  int frame_advantage_difference = local_frame_advantage - session->remote_frame_advantage;// How different is the frame advantage reported by the remote client and this one

  if (local_frame_advantage < MAX_ROLLBACK_FRAMES && frame_advantage_difference <= FRAME_ADVANTAGE_LIMIT) {// Only allow the local client to get so far ahead of remote
    return true;
  }else{
    return false;
  }
}
// No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_rollback_condition(PU_SESSION *session){
  if (session->local_frame > session->sync_frame && session->remote_frame > session->sync_frame) {
    return true;
  } else {
    return false;
  }
}
// Network functions
void pu_update_network(PU_SESSION *session, ENetHost* player){
  session->remote_frame_advantage = (session->local_frame - session->remote_frame);

  ENetEvent event;

  switch (session->local_player_type) {
    case PLAYER_HOST:
      while (enet_host_service(player, &event, 0) > 0) {
        switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT:
            if (SHOW_DEBUG) {
              printf("A new client connected from %s:%u.\n", event.peer->address.host, event.peer->address.port);
            }
            if (!session->has_started) {
              session->has_started = true;
              event.peer->data = NULL;
            }
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            if (SHOW_DEBUG) {
              printf ("%u disconnected.\n", event.peer->connectID);
            }
            event.peer->data = NULL;
            break;
          case ENET_EVENT_TYPE_RECEIVE:
            if (SHOW_DEBUG) {
              printf ("A packet of length %u was received from %x:%u on channel %u.\n",
                event.packet->dataLength,
                event.peer->address.host,
                event.peer->address.port,
                event.channelID);
              printf("Recieved Frame: %d - ", ((PU_INPUT_PACKET*)event.packet->data)->frame_num);
              printf("Recieved Input: %u \n", ((PU_INPUT_PACKET*)event.packet->data)->input);
            }
            session->remote_frame = ((PU_INPUT_PACKET*)event.packet->data)->frame_num;
            enet_packet_destroy(event.packet);
            break;
        }
      }
      break;
    case PLAYER_CLIENT:
      while (enet_host_service(player ,&session->local_client_event, 0) > 0) {
        switch (session->local_client_event.type) {
          case ENET_EVENT_TYPE_RECEIVE:
            if (SHOW_DEBUG) {
              printf ("A packet of length %u was received from %x:%u on channel %u.\n",
                session->local_client_event.packet->dataLength,
                session->local_client_event.peer->address.host,
                session->local_client_event.peer->address.port,
                session->local_client_event.channelID);
              printf("Recieved Frame: %d - ", ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->frame_num);
              printf("Recieved Input: %u \n", ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->input);
            }
            if (!session->has_started) {
              session->has_started = true;
            }
            session->remote_frame = ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->frame_num;
            enet_packet_destroy(session->local_client_event.packet);
            break;
        }
      }
      break;
  }
}
//cleanup enethost client
void pu_destroy_client(ENetHost* client){
  enet_host_destroy(client);
  pu_log("Client destroyed!\n");
}
// disconnect client from host
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
// connect client to host
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address){
  ENetAddress address = {0};
  enet_address_set_host(&address, ip_address);
  address.port = DEFAULT_PORT;
  session->host_peer = enet_host_connect(client, &address, ENET_CHANNELS, 0);

  if (session->host_peer == NULL) {
    pu_log("No available peers for initiating an ENet connection!\n");
    return false;
  }
  if (enet_host_service(client, &session->local_client_event, 5000) > 0 && session->local_client_event.type == ENET_EVENT_TYPE_CONNECT) {
    pu_log("Connection succesfull\n");
    return true;
  }else{
    enet_peer_reset(session->host_peer);
    pu_log("Connection failed\n");
    return false;
  }
}
// create a client
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
// cleanup enethost host
void pu_destroy_host(ENetHost* host, PU_SESSION *session){
  session->local_player_host = NULL;
  enet_host_destroy(host);
  pu_log("Host destroyed!\n");
}
// create host
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
// simple logging function
void pu_log(const char* message){
  if (SHOW_DEBUG) {
    fprintf(stderr, message);
  }
}
// initialize Please undo session and ENet
int pu_initialize(PU_SESSION *session){
  session->local_frame = INITIAL_FRAME;
  session->remote_frame = INITIAL_FRAME;
  session->sync_frame = INITIAL_FRAME;
  session->remote_frame_advantage = 0;
  session->has_started = false;

  session->player_input[0].input_vector = vector_init(uint16_t, INPUT_RESERVSE_SPACE);
  session->player_input[1].input_vector = vector_init(uint16_t, INPUT_RESERVSE_SPACE);
  session->player_input[2].input_vector = vector_init(uint16_t, INPUT_RESERVSE_SPACE);

  if (enet_initialize() != 0) {
    pu_log("An error occurred while initializing ENet.\n");
    return true;
  }
  pu_log("Initialized Enet and Please Undo.\n");
  return false;
}
// Deinitialized Please undo and ENet
void pu_deinitialize(PU_SESSION *session){

  vector_free(session->player_input[0].input_vector);
  vector_free(session->player_input[1].input_vector);
  vector_free(session->player_input[2].input_vector);

  enet_deinitialize();
  pu_log("Enet and Please Undo deinitialized.\n");
}
#endif //PLEASE_UNDO_IMPL_H

#endif //PLEASE_UNDO_H
