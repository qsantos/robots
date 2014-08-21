Protocol
========

This file defines the protocol used in the tank programming game
'robots' for communications between the display and the robots with the
server. Hexadecimal token represent individual bytes sent on the network
while integer and floating point values are represented on four bytes
using C types uin32_t and float.

Before the game
---------------

### Connection and waiting phase

When the client connects, he first sends the magic word (0x42) and protocol version (0x01).

    Client> 0x42 0x01

The server then sends back the same two bytes followed by information on the game:

    Server> 0x42 0x01
    Server> Game (see Data structure transfer)

At this point, the server considers that the client is fully connected
and waiting for a game. Every time the number of currently connected
clients (n_clients) changes the server sends its new value to **every**
connected client:

    Server> n_clients (uint32)

For instance, when a first client A connects, the server could return:

    Server>A 0x42 0x01 1024 768 2 1

This is because there is currently one connected client (A). Then, if a
second client B connects, the server sends him similar information and
notifies A:

    Server>B 0x42 0x01 1024 768 2 2
    Server>A 2

In other words, the server send the new number of currently connected clients to both A and B.



### Initialization phase

When all clients are connected (`n_clients` == `n_slots`), the game is
ready to begin and the server sends the initial state of
its robot to each client:

    Server> Robot (see Data structure transfer)

Then, the server sends the start message (0x42) to every client:

    Server> 0x42

From now on, the servers only sends events concerning the client, and
clients only sends orders to their robots.


Orders
------

An order can be sent by a client to the server at any time and is executed
immediately by the client's robot. Currently, any order takes exactly
one 4-byte (float) parameter.

    Advance   0x01
    Turn      0x02
    Turngun   0x03
    Fire      0x04
    Velocity  0x05
    Turnspeed 0x06
    Gunspeed  0x07

Movement is not instantaneous: if a robot is currently executing movement
orders and receive a new one, the old one is forgotten.

### Advance (0x01)

Move forward or backward (negative value) for a given distance (pixels).

    Client> 0x01
    Client> distance (float)

### Turn (0x02)

Rotate for the given angle (radians).

    Client> 0x02
    Client> angle (float)

### Turngun (0x03)

Rotate the gun by the given angle (radians).

    Client> 0x03
    Client> angle (float)

### Fire (0x04)

Fire a bullet with the given energy.

    Client> 0x04
    Client> energy (float)

### Velocity (0x05)

Sets the velocity at which the robot moves (pixels per second).

    Client> 0x05
    Client> velocity (float)


### Turnspeed (0x06)

Sets the rotational speed of the robot (radians per second).

    Client> 0x06
    Client> speed (float)

### Gunspeed (0x07)

Sets the rotational speed of the gun (radians per second).

    Client> 0x07
    Client> speed (float)


Events
------

Events inform the clients on what is happening. They can happen at any
time during the game.

    Tick     0x01
    Dump     0x02
    Robot    0x03
    Bullet   0x04
    Hit      0x05
    Hitby    0x06
    Hitrobot 0x07
    Hitwall  0x08
    Kaboum   0x09

In the case of the display, the event code is followed by the identifier
of the robot which produced the event (except for TICK, DUMP and KABOUM):

    Server> event_code  (uint8)
    Server> robot_id    (uint8)
    Server> event_param (?)

The intended destination for an event is given in the title.

### Tick (0x01) (all)

A tick is the atomic unit of time in a game. At every tick, the server broadcasts a tick event:

    Server> 0x01

### Dump (0x02) (display)

The server regularly sends information on the current state to the display:

    Server> 0x02
    Server> Game (see Data structure transfer)
    Server> n_robots   (uint32)
    Server> Robot      (n_robots items) (see Robot information)
    Server> RobotOrder (n_robots items) (see RobotOrder information)
    Server> n_bullets  (uint32)
    Server> Bullet     (n_bullet items) (see Bullet information)

where `n_robots` (respectively `n_bullets`) is the total number of active
robots (bullets) still in the game and the items of the arrays of `Robot`
and of `RobotOrder` should be sorted the same way.

### Dump (0x02) (robots)

The server can decide to remind information on their robots to clients:

    Server> 0x02
    Server> Robot      (see Robot information)
    Server> RobotOrder (see RobotOrder information)

### Robot (0x03) (display, watcher)

Whenever another robot is in sight, the client gets:

    Server> 0x03
    Server> Robot (see Robot information)


### Bullet (0x04) (display, watcher)

Whenever a bullet is in sight (regardless of direction), the client gets:

    Server> 0x04
    Server> Bullet (see Bullet information)


### Hit (0x05) (display, shooter)

Whenever a bullet fired by the robot hits a target, the folowing event occurs:

    Server> 0x05
    Server> Bullet (see Bullet information)
    Server> robot_id (uint32)

where `robot_id` is the identifier of the robot which was hit.


### Hitby (0x06) (display, shooted)

Whenever the robot is hit by a bullet, it gets:

    Server> 0x06
    Server> Bullet (see Bullet information)


### Hitrobot (0x07) (display, collided robots)

Whenever two robots collide, one gets the following information:

    Server> 0x07
    Server> robot_id (uint32)

where `robot_id` is the identifier of the other robot involved.


### Hitwall (0x08) (display, clumsy robot)

Whenever a robot hits the border, the client gets:

    Server> 0x08

### Kaboum (0x09) (display, all)

Whenever a robot is killed, all robots feel a disturbance in the Force:

    Server> 0x09
    Server> Robot (see Robot information)


Data structure transfers
------------------------

### Game

    width            (float)
    height           (float)
    max_velocity     (float)
    max_turnSpeed    (float)
    max_turnGunSpeed (float)
    max_fireEnergy   (float)
    n_slots          (uint32)
    n_clients        (uint32)

### Robot

    id           (uint32)
    x            (float)
    y            (float)
    width        (float)
    height       (float)
    angle        (float)
    gunAngle     (float)
    energy       (float)
    velocity     (float)
    turnSpeed    (float)
    turnGunSpeed (float)

`id` is an uniq identifier

### RobotOrder

    advance (float)
    turn    (float)
    turnGun (float)

Each value is the remaining action to be executed.

### Bullet

    from   (uint32)
    x      (float)
    y      (float)
    angle  (float)
    energy (float)

`from` is the shooter's identifier
