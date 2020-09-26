#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main (int argc, char ** argv){
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    ENetEvent event;
    ENetAddress address = {0};
    ENetHost* server;

    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY; // This allows
    /* Bind the server to port 7777. */
    address.port = 7777;

    server = enet_host_create(&address	/* the address to bind the server host to */,
    				32	/* allow up to 32 clients and/or outgoing connections */,
    				1	/* allow up to 1 channel to be used, 0. */,
    				0	/* assume any amount of incoming bandwidth */,
    				0	/* assume any amount of outgoing bandwidth */);

    if (server == NULL)
    {
    	printf("An error occurred while trying to create an ENet server host.");
    	return 1;
    }

    // gameloop
    while(true)
    {
    	ENetEvent event;
    	/* Wait up to 1000 milliseconds for an event. */
    	while (enet_host_service (server, & event, 1000) > 0)
    	{
    		switch (event.type)
    		{
    			case ENET_EVENT_TYPE_CONNECT:
    				printf ("A new client connected from %x:%u.\n",
    		  		event.peer -> address.host,
    				event.peer -> address.port);
    			break;

    			case ENET_EVENT_TYPE_RECEIVE:
    				printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
    					event.packet -> dataLength,
    					event.packet -> data,
    					event.peer -> data,
    					event.channelID);
    					/* Clean up the packet now that we're done using it. */
    					enet_packet_destroy (event.packet);
    			break;

    			case ENET_EVENT_TYPE_DISCONNECT:
    				printf ("%s disconnected.\n", event.peer -> data);
    				/* Reset the peer's client information. */
    				event.peer -> data = NULL;
    		}
    	}
    }

    enet_host_destroy(server);

    return 0;
}
