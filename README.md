Robots
======

Robots is a simple tank programming game. The communications are done
through TCP sockets (allowing playing on a network). The game is coded
in C and an example is given along with useful functions to write a bot
in C. However, a bot can be written in any programming language.

Compiling
---------

You'll need the following packages:

opengl1-mesa-dev openglu1-mesa-dev freeglut3-dev libsoil-dev libalut-dev libvorbis-dev

Then, go into the 'src/' directory and type:

    $ make

You're done !


Executing
---------

Binaries should be put in the 'bin/' directory. First launch the server by typing:

    $ bin/server

The display must be launched from the directory where 'img/' and 'music/' belong:

    $ bin/display

Then, you can connect clients to the server. For example:

    $ bin/client

For each executable, you can specify the interface and the port to use
(both IPv4 and IPv6 are suported). For more information, simply use the
`--help` flag.

The simulation starts when the display and all the clients are connected
to the server.


Making bots
-----------

You can make a bot in any programming language. If the language does
not support networking, you can redirect its input and output through
netcat for example. To know how to interact with the server, refer to
the [PROTOCOL.md](PROTOCOL.md) file.
