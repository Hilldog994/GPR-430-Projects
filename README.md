# GPR-430-Final
Final project for networking. Consists of a server and clients who connect to the server and can move around and see others move around. This was made using the transport layer(LLAPI) in Unity 3D.

There are 2 seperate unity projects for the server and client to allow for easier debugging between the two through looking at debug messages in editor. There is a build for the client so that multiple clients can be connected easily to the server. Clients connect to game rooms that hold 2 clients including themself. Once both spots of a room are filled with a client the "game" starts and each client can move around and see the other client in their room move as well. The server creates new rooms as needed, and removes any disconnected clients from rooms so others can join in their place if they try to join a room

## How to use
1. Open the server project in unity and press play to start up the server, this project doesnt need to be interacted with until you are done or want to look at debug messages
2. Either open the client build for as many clients or open the client project in unity to connect clients to the open server.(This can be done any time after the server project has been opened)
3. Click the button on any client build to connect to a room on the server, if a different client has already joined a room as well you will see both clients represented by a cube with a nametag indicating their connectionID on the server
4. Once a room has been filled the clients within that room will now see each other in their build and can move themselves using either WASD/Arrow keys to move and Space to jump
