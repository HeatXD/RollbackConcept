/*
MIT License

Copyright (c) 2020 HeatXD

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
//Oh ye btw *X*X means not implemented or not done yet : Jamie/heatxd
//-----------------------------------------------------------------------------
#ifndef PLEASE_UNDO_H
#define PLEASE_UNDO_H
#define ENET_IMPLEMENTATION
#include "enet.h"
#include "cvector.h"
#include <stdio.h>
#include <stdbool.h>
//PLEASE_UNDO DEBUG
#define SHOW_DEBUG true// Show Debug messages
//Declaration Constants
#define MAX_TIMEOUT_TRESHOLD 60 //time until a disconnect in frames
#define MAX_ROLLBACK_FRAMES 10 // Specifies the maximum number of frames that can be resimulated
#define FRAME_ADVANTAGE_LIMIT 6 // Only allow the local clie nt to get so far ahead of remote
#define LOCAL_FRAME_DELAY 2 // amount of artificial local delay added for smoother gameplay should never be below 1.
#define INITIAL_FRAME 0 //Specifies the initial frame the game starts in. Cannot rollback before this frame
#define ENET_CHANNELS 2// ch1 for gameplay and ch2 for text messages
#define DEFAULT_PORT 9090 //Default port used by an Enet HOST and to connect to an Enet HOST
#define MAX_PEERS_CLIENT 1//Amount of clients who can connect to this client. should always be above 0 because he has to connect to a host
#define MAX_PEERS_HOST 2//Amount of clients who can connect to this host. should always be above 0 otherwise noone can connect
#define INPUT_RESERVSE_SPACE 90 //Input space to reserve when the input vector is close to capacity
// Macros
#define PU_SET_BIT(BF, N) BF |= ((uint16_t) 0x0001 << N)
#define PU_CLR_BIT(BF, N) BF &= ~((uint16_t) 0x0001 << N)
#define PU_IS_BIT_SET(BF, N) ((BF >> N) & 0x1)
// this is a part with will differ per program. the programmer may have to look to change these callback functions to make it compatible with his use case
typedef void (*PU_SESSION_CALLBACK)(int frame, void* a, void* b);
typedef struct PU_SESSION_CALLBACKS{
  PU_SESSION_CALLBACK save_game_state;
  PU_SESSION_CALLBACK advance_game_state;
  PU_SESSION_CALLBACK restore_game_state;
}PU_SESSION_CALLBACKS;
// Player tags to specify the right job in the pu_update_network function
typedef enum PU_PLAYER_TYPE{
  PLAYER_HOST = 1,
  PLAYER_CLIENT = 2,
  PLAYER_SPECTATOR = 3//not implemented yet *X*X
}PU_PLAYER_TYPE;
// Input tags to specify which type of input is added to the input_vector
typedef enum PU_INPUT_STATUS{
  INPUT_UNDEFINED = 1,
  INPUT_CONFIRMED = 2,
  INPUT_PREDICTED = 3
}PU_INPUT_STATUS;
// Game input used in the input vectors
typedef struct PU_GAME_INPUT{
  uint16_t input; // expecting gameinput to be a 32bit bitfield for now makes it easier to compare for now.
  PU_INPUT_STATUS input_status;
}PU_GAME_INPUT;
//primary way to store input, used in various functions
typedef struct PU_INPUT_STORAGE{
  PU_GAME_INPUT* input_vector;
}PU_INPUT_STORAGE;
// Used to send input over the wire with its associated frame
typedef struct PU_INPUT_PACKET{
  int frame_num;
  uint16_t input;
}PU_INPUT_PACKET;

// Main session struct used in almost all major functions contains much needed info for implementing rollback
typedef struct PU_SESSION{
  int local_frame;// Tracks the latest update frame.
  int remote_frame;// Tracks the latest frame received from the remote client
  int sync_frame;// Tracks the last frame where we synchronized the game state with the remote client. Never rollback before this frame
  int remote_frame_advantage;// Latest frame advantage received from the remote client
  //----------------------------------------------------------------------------
  PU_PLAYER_TYPE local_player_type;
  PU_INPUT_STORAGE player_input[3]; // 0 == local_player, 1 == remote_player_confirmed, 2 == remote_player_predicted // could prolly be done better if you want to support more than 2 players
  //----------------------------------------------------------------------------
  ENetHost* local_player_host;
  ENetEvent local_client_event;
  ENetPeer* remote_peer;
  //----------------------------------------------------------------------------
  int has_started;
  int remote_timeout_count; //X*X* not inplemented fully yet
}PU_SESSION;

// Declaration Funcs
void pu_disconnect_from_host(PU_SESSION *session, ENetHost* client);// disconnect from enethost
void pu_destroy_client(PU_SESSION *session, ENetHost* client);// cleanup enet client
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address);// function to connect to a host
ENetHost* pu_create_client(PU_SESSION *session);// create an Enet+PU client to connect to a host
void pu_update_network(PU_SESSION *session, ENetHost* player);// updates remote inputs and events
ENetHost* pu_create_host(PU_SESSION *session); // create an Enet+PU Host if you want people to connect to you
void pu_deinitialize(PU_SESSION *session);// de-init PU and Enet
int pu_initialize(PU_SESSION *session);// init PU and Enet functions needed to properly run
void pu_log(const char* message);//console message function
void pu_destroy_host(PU_SESSION *session, ENetHost* host);// cleans up ENet and PU host vars
void pu_send_input(PU_SESSION *session, ENetHost *player, const uint16_t input);// send player input over the wire with its associated frame number
void pu_add_local_input(PU_SESSION *session, ENetHost *player, const uint16_t input); //Add local input and send it
void pu_add_remote_input(PU_SESSION *session, const uint16_t input);// Add Recieved input to input vector
void pu_predict_remote_input(PU_SESSION *session, int frame);//predict remote input if not yet available simply use the previous input.
void pu_determine_sync_frame(PU_SESSION *session);// Finds the last frame where the inputs were matched
int pu_rollback_condition(PU_SESSION *session); // No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_timesynced_condition(PU_SESSION *session);// Function for syncing both players making the other wait
void pu_update_predicted_input(PU_INPUT_STORAGE* inputs, int frame);//update predicted input to confirmed input when corrected in the rollback update
void pu_start_session(PU_SESSION *session);// Please undo function to start send input
void pu_check_disconnect_treshold(PU_SESSION *session);// function to check wheter to disconnect or not
void pu_process_rollbacks(PU_SESSION *session, PU_SESSION_CALLBACKS *cb, void* gamestate, void* gamestate_vector); // internal rollback squence
//-----------------------------------------------------------------------------
// Implementation
#ifdef PLEASE_UNDO_IMPL_H
// Internal rollback squence
void pu_process_rollbacks(PU_SESSION *session, PU_SESSION_CALLBACKS *cb, void* gamestate, void* gamestate_vector){
  pu_determine_sync_frame(session);
  if (pu_rollback_condition(session)){
    //restore gamestate to sync frame
    cb->restore_game_state(session->sync_frame, gamestate_vector, gamestate);
    //use all the inputs since the last sync frame until the current frame to simulate
    for (int i = session->sync_frame + 1; i <= session->local_frame; i++) {
      ///update without rendering and local input since we already have that
      //update input to be used in the games
      pu_update_predicted_input(session->player_input, i);
      //advance gamestate
      cb->advance_game_state(i, gamestate, session->player_input);
      //save gamestate
      cb->save_game_state(i, gamestate_vector, gamestate);
    }
  }
}
// Check for how long the remote client isnt synced if its above the treshold we will disconnect
void pu_check_disconnect_treshold(PU_SESSION *session){
  if (session->remote_timeout_count > MAX_TIMEOUT_TRESHOLD) {
    pu_log("SHOULD DISCONNECT!\n");
    if (session->local_player_type == PLAYER_HOST) {
      pu_destroy_host(session, session->local_player_host);
    }else if (session->local_player_type == PLAYER_CLIENT) {
      pu_disconnect_from_host(session, session->local_player_host);
      pu_destroy_client(session, session->local_player_host);
    }
  }
}
// Please undo function to start the session
void pu_start_session(PU_SESSION *session){
  session->has_started = true;
}
//update predicted input to confirmed input when corrected in the rollback update
void pu_update_predicted_input(PU_INPUT_STORAGE* inputs, int frame){
  inputs[2].input_vector[frame-1].input = inputs[1].input_vector[frame-1].input;
}
//predict remote input if not yet available simply use the previous input.
void pu_predict_remote_input(PU_SESSION *session, int frame){
  if (SHOW_DEBUG) {
      printf("SYNC FRAME = %d\n", session->sync_frame);
  }
  if (session->player_input[1].input_vector[frame-1].input == 0 && session->player_input[1].input_vector[frame-2].input == 0) {
    PU_GAME_INPUT game_input = {.input = 0, .input_status = INPUT_PREDICTED};
    session->player_input[1].input_vector[frame-1] = game_input;
    session->player_input[2].input_vector[frame-1] = game_input;

  }else if (session->player_input[1].input_vector[frame-1].input == 0 && session->player_input[1].input_vector[frame-2].input != 0) {
    PU_GAME_INPUT game_input = {.input = session->player_input[1].input_vector[frame-2].input, .input_status = INPUT_PREDICTED};
    session->player_input[1].input_vector[frame-1] = game_input;
    session->player_input[2].input_vector[frame-1] = game_input;
  }
};
// Add Recieved input to input vector
void pu_add_remote_input(PU_SESSION *session, const uint16_t input){
  if (session->remote_frame >= vector_capacity(session->player_input[1].input_vector)) {
    vector_reserve(session->player_input[1].input_vector, vector_capacity(session->player_input[1].input_vector) + INPUT_RESERVSE_SPACE);
    vector_reserve(session->player_input[2].input_vector, vector_capacity(session->player_input[2].input_vector) + INPUT_RESERVSE_SPACE);
  }
  PU_GAME_INPUT game_input = {.input = input, .input_status = INPUT_CONFIRMED};
  session->player_input[1].input_vector[session->remote_frame-1] = game_input;
};
// Add local input and send it
void pu_add_local_input(PU_SESSION *session, ENetHost *player, const uint16_t input){
  if (vector_capacity(session->player_input[0].input_vector) < session->local_frame + (MAX_ROLLBACK_FRAMES + LOCAL_FRAME_DELAY)) {
    vector_reserve(session->player_input[0].input_vector, vector_capacity(session->player_input[0].input_vector) + INPUT_RESERVSE_SPACE);
    vector_reserve(session->player_input[1].input_vector, vector_capacity(session->player_input[1].input_vector) + INPUT_RESERVSE_SPACE);
    vector_reserve(session->player_input[2].input_vector, vector_capacity(session->player_input[2].input_vector) + INPUT_RESERVSE_SPACE);
  }
  //adding some predicted inputs for the first few frames at the start of a session when the local players has no input
  if(session->local_frame <= LOCAL_FRAME_DELAY){
    PU_GAME_INPUT game_input = {.input = 0, .input_status = INPUT_PREDICTED};
    session->player_input[1].input_vector[(session->local_frame-1)] = game_input;
    session->player_input[2].input_vector[(session->local_frame-1)] = game_input;
  }

  PU_GAME_INPUT game_input = {.input = input, .input_status = INPUT_CONFIRMED};
  //place the local input at localframe + local_delay
  session->player_input[0].input_vector[(session->local_frame-1) + LOCAL_FRAME_DELAY] = game_input;

  if (SHOW_DEBUG) {
    printf("SHOW CURRENT INPUT\nP1-INPUT[%d] = %u , %d\nP2-INPUT[%d] = %u , %d\n",
    session->local_frame, session->player_input[0].input_vector[session->local_frame-1].input,
    session->player_input[0].input_vector[session->local_frame-1].input_status,
    session->local_frame, session->player_input[1].input_vector[session->local_frame-1].input,
    session->player_input[1].input_vector[session->local_frame-1].input_status);
  }
  //send input to remote player
  pu_send_input(session, player, input);
}
// send player input over the wire with its associated frame number
void pu_send_input(PU_SESSION *session, ENetHost *player, const uint16_t input){
  PU_INPUT_PACKET data;

  data.frame_num = session->local_frame + LOCAL_FRAME_DELAY;
  data.input = input;

  ENetPacket* packet = enet_packet_create(&data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);

  enet_peer_send(session->remote_peer, 0, packet);
}
// Finds the last frame where the inputs were matched
void pu_determine_sync_frame(PU_SESSION *session){
  int final_frame = session->remote_frame; // We will only check inputs until the remote_frame, since we don't have inputs after.
  int found_frame = INITIAL_FRAME - 1;

  if (session->remote_frame > session->local_frame) {
    final_frame = session->local_frame;  // Incase the remote client is ahead of local, don't check past the local frame.
  }

  for (int i = session->sync_frame + 1; i <= final_frame; i++) {
    if (session->player_input[1].input_vector[i-1].input != session->player_input[2].input_vector[i-1].input) {
      if(session->player_input[2].input_vector[i-1].input_status == INPUT_PREDICTED){
        if (SHOW_DEBUG) {
          printf("FOUND WRONG PREDICTION!\nFOUND INPUT: %u  BUT SHOULD BE %u AT FRAME: %d\n",
          session->player_input[2].input_vector[i-1].input, session->player_input[1].input_vector[i-1].input, i);
        }
        found_frame = i;
        break;
      }
    }else{
      pu_log("NO WRONG PREDICTION FOUND!\n");
      if (session->player_input[1].input_vector[i-1].input_status == INPUT_PREDICTED || session->player_input[1].input_vector[i-1].input_status == INPUT_CONFIRMED){
        session->player_input[1].input_vector[i-1].input_status = INPUT_CONFIRMED;
        session->player_input[2].input_vector[i-1].input_status = INPUT_CONFIRMED;
      }
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
  // Only allow the local client to get so far ahead of remote
  if (local_frame_advantage < MAX_ROLLBACK_FRAMES && frame_advantage_difference <= FRAME_ADVANTAGE_LIMIT) {
    pu_log("TIME IS SYNCED!\n");
    if (session->remote_timeout_count != 0) {
      session->remote_timeout_count = 0;
    }
    return true;
  }else{
    pu_log("TIME IS NOT SYNCED!\n");
    session->remote_timeout_count++;
    pu_check_disconnect_treshold(session);
  }
  return false;
}
// No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_rollback_condition(PU_SESSION *session){
  if (session->local_frame > session->sync_frame && session->remote_frame > session->sync_frame) {
    pu_log("SHOULD ROLLBACK!\n");
    return true;
  } else {
    pu_log("SHOULDN'T ROLLBACK!\n");
    return false;
  }
}
// Network functions
void pu_update_network(PU_SESSION *session, ENetHost* player){
  ENetEvent event;

  switch (session->local_player_type) {
    case PLAYER_HOST:
      while (enet_host_service(player, &event, 0) > 0) {
        switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT:
            if (SHOW_DEBUG) {
              printf("A NEW CLIENT CONNECTED FROM %s:%u. \n", event.peer->address.host, event.peer->address.port);
            }
            if (!session->has_started) {
              pu_start_session(session);
              session->remote_peer = event.peer;
            }
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            if (SHOW_DEBUG) {
              printf ("%u DISCONNECTED.\n", event.peer->connectID);
            }
            event.peer->data = NULL;
            break;
          case ENET_EVENT_TYPE_RECEIVE:
            if (SHOW_DEBUG) {
              printf ("A PACKET OF LENGTH %u WAS RECIEVED FROM %x:%u ON CHANNEL %u.\n",
                event.packet->dataLength,
                event.peer->address.host,
                event.peer->address.port,
                event.channelID);
              printf("RECIEVED FRAME: %d - ", ((PU_INPUT_PACKET*)event.packet->data)->frame_num);
              printf("RECIEVED INPUT: %u \n", ((PU_INPUT_PACKET*)event.packet->data)->input);
            }
            session->remote_frame = ((PU_INPUT_PACKET*)event.packet->data)->frame_num;
            pu_add_remote_input(session, ((PU_INPUT_PACKET*)event.packet->data)->input);
            enet_packet_destroy(event.packet);
            break;
        }
      }
      break;
    case PLAYER_CLIENT:
      while (enet_host_service(player, &session->local_client_event, 0) > 0) {
        switch (session->local_client_event.type) {
          case ENET_EVENT_TYPE_RECEIVE:
            if (!session->has_started) {
              pu_start_session(session);
            }
            if (SHOW_DEBUG) {
              printf ("A PACKET OF LENGTH %u WAS RECIEVED FROM %x:%u ON CHANNEL %u.\n",
                session->local_client_event.packet->dataLength,
                session->local_client_event.peer->address.host,
                session->local_client_event.peer->address.port,
                session->local_client_event.channelID);
              printf("RECIEVED FRAME: %d - ", ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->frame_num);
              printf("RECIEVED INPUT: %u \n", ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->input);
            }
            session->remote_frame = ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->frame_num;
            pu_add_remote_input(session, ((PU_INPUT_PACKET*)session->local_client_event.packet->data)->input);
            enet_packet_destroy(session->local_client_event.packet);
            break;
        }
      }
      break;
  }
  session->remote_frame_advantage = (session->local_frame - session->remote_frame);
}
//cleanup enethost client
void pu_destroy_client(PU_SESSION *session, ENetHost* client){
  session->local_player_host = NULL;
  enet_host_destroy(client);
  pu_log("CLIENT DESTROYED!\n");
}
// disconnect client from host
void pu_disconnect_from_host(PU_SESSION *session, ENetHost* client){
  enet_peer_disconnect(session->remote_peer, 0);
  while (enet_host_service(client, &session->local_client_event, 3000) > 0) {
    switch (session->local_client_event.type) {
      case ENET_EVENT_TYPE_RECEIVE:
        enet_packet_destroy(session->local_client_event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        pu_log("DISCONNECTION SUCCEEDED.\n");
        break;
    }
  }
}
// connect client to host
int pu_connect_to_host(ENetHost* client, PU_SESSION *session, char* ip_address){
  ENetAddress address = {0};
  enet_address_set_host(&address, ip_address);
  address.port = DEFAULT_PORT;
  session->remote_peer = enet_host_connect(client, &address, ENET_CHANNELS, 0);

  if (session->remote_peer == NULL) {
    pu_log("NO AVAILABLE PEERS FOR INITIATING AN ENET CONNECTION!\n");
    return false;
  }
  if (enet_host_service(client, &session->local_client_event, 5000) > 0 && session->local_client_event.type == ENET_EVENT_TYPE_CONNECT) {
    pu_log("CONNECTION SUCCESSFUL\n");
    return true;
  }else{
    enet_peer_reset(session->remote_peer);
    pu_log("CONNECTION FAILED\n");
    return false;
  }
}
// create a client
ENetHost* pu_create_client(PU_SESSION *session){
  ENetHost* client;
  session->local_player_type = PLAYER_CLIENT;
  client = enet_host_create(NULL, MAX_PEERS_CLIENT, ENET_CHANNELS, 0, 0);

  if (client == NULL) {
    pu_log("AN ERROR OCCURRED WHILE TRYING TO CREATE AN ENET CLIENT!\n");
    return client;
  }
  session->local_player_host = client;
  pu_log("CLIENT CREATED!\n");
  return client;
}
// cleanup enethost host
void pu_destroy_host(PU_SESSION *session, ENetHost* host){
  session->local_player_host = NULL;
  enet_host_destroy(host);
  pu_log("HOST DESTROYED!\n");
}
// create host
ENetHost* pu_create_host(PU_SESSION *session){
  ENetHost* host;
  ENetAddress address = {0};

  address.host = ENET_HOST_ANY;
  address.port = DEFAULT_PORT;
  host = enet_host_create(&address, MAX_PEERS_HOST, ENET_CHANNELS, 0, 0);

  if (host == NULL) {
    pu_log("AN ERROR OCCURRED WHILE TRYING TO CREATE AN ENET SERVER HOST.\n");
    return NULL;
  }
  session->local_player_type = PLAYER_HOST;
  session->local_player_host = host;
  pu_log("ENET HOST CREATED!\n");
  return host;
}
// simple console logging function
void pu_log(const char* message){
  if (SHOW_DEBUG) {
    printf(message);
  }
}
// initialize Please undo session and ENet
int pu_initialize(PU_SESSION *session){
  session->local_frame = INITIAL_FRAME;
  session->remote_frame = INITIAL_FRAME;
  session->sync_frame = INITIAL_FRAME;
  session->remote_frame_advantage = 0;
  session->has_started =  false;

  size_t n = sizeof(session->player_input)/sizeof(session->player_input[0]);

  for (size_t i = 0; i < n; i++) {
    session->player_input[i].input_vector = vector_init(PU_GAME_INPUT, INPUT_RESERVSE_SPACE);
  }

  if (enet_initialize() != 0) {
    pu_log("AN ERROR OCCURRED WHILE INITIALIZING ENET.\n");
    return true;
  }
  pu_log("INITIALIZED ENET AND PLEASEUNDO.\n");
  return false;
}
// Deinitialized Please undo and ENet
void pu_deinitialize(PU_SESSION *session){

  size_t n = sizeof(session->player_input)/sizeof(session->player_input[0]);

  for (size_t i = 0; i < n; i++) {
      vector_free(session->player_input[i].input_vector);
  }

  enet_deinitialize();
  pu_log("ENET AND PLEASEUNDO DEINITIALIZED.\n");
}
#endif //PLEASE_UNDO_IMPL_H

#endif //PLEASE_UNDO_H
