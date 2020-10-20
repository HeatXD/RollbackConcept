# PleaseUndo

Rollback networking library built on top of Enet written in c99.

You need to link to -lws2_32 and -lwinmm on Windows just like normal Enet.
I haven't tested this on Linux yet. You're welcome to help me with that!

It's still very WIP lots of things are still missing but it has been a fun covid-project so far.
For now I can connect 1 client to a host.

I need to implement a checksum to verify that the correct data has been recieved.

I still need to implement a Spectator job in pu_update_network so people can watch games.

I still need to come up with a proper event system and I need to come up with ways to detect disconnects.

I should create an example game. Coming soonTM.

printf spits out wierd numbers. i should probably make a log to file function.

## Resources

https://gist.github.com/rcmagic/f8d76bca32b5609e85ab156db38387e9 and the GGPO discord.

## Dependencies

ENET and C_Vector both included with PleaseUndo in the include folder which are both MIT licensed.

ENET >> https://github.com/zpl-c/enet

C_Vector >> https://github.com/NickHackman/C_Vector
