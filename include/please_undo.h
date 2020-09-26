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
#define INPUT_DELAY 1 // Should always be above 0
#define INITIAL_FRAME 0 //Specifies the initial frame the game starts in. Cannot rollback before this frame
#define DEFAULT_HOST_PORT 6767 // Default port for when the host creates a game
#define MAX_PLAYERS 3 //host it self and 3 peers
#define ENET_CHANNELS 2// 1 for gameplay and 2 for text messages
//Declaration Structs and enums
typedef enum PU_PLAYER_TYPE{
  PLAYER_HOST = 1,
  PLAYER_CLIENT = 2,
  PLAYER_SPECTATOR = 3
}PU_PLAYER_TYPE;

typedef struct PU_SESSION{
}PU_SESSION;
//Declaration Funcs
//Please Undo Session Functions
//-----------------------------------------------------------------------------
// Implementation
#ifdef PLEASE_UNDO_IMPL_H
#endif //PLEASE_UNDO_IMPL_H

#endif //PLEASE_UNDO_H
