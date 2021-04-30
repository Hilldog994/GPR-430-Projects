using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.Networking;
/*
 This playlist was used to help get the setup of the network transport API up and running for client and server
https://www.youtube.com/playlist?list=PLLH3mUGkfFCVSV6Q1UJE7Ge6UfhGzRAnH
  
 */


/*
 * TODO: Have room connected message, and game start message. Room connect lets client know its found a room, game start when both clients in a room are connected
 * (probably do the connection message as game start or something,
 */


public class ServerMain : MonoBehaviour
{
    const int MAX_USERS = 12;
    const int PORT = 7777;
    const int MAX_PACKET_SIZE = 1024;

    int hostID;
    byte TCPChannel, UDPChannel, ReliableOrderedChannel;//TCP = reliable, UDP = unreliable
    byte error;//error output


    bool serverStarted;

    public class GameRoom
    {
        //ids to be used to send messages to correct clients in a room, only send the position updates and such to the client that is playing with them
        public int clientID1 = -1;
        public int clientID2 = -1;

        public bool AddToRoom(Client clientToAdd)
        {
            if(clientID1 < 0)
            {
                clientID1 = clientToAdd.conID;
                Debug.Log("Space1 filled");
                return true;
            }
            else if (clientID2 < 0)
            {
                clientID2 = clientToAdd.conID;
                Debug.Log("Space2 filled");
                return true;
            }
            else
            {
                //room full
                return false;
            }    
        }

        public bool IsRoomFull()
        {
            if (clientID1 > 0 && clientID2 > 0)//if both client ids are valid
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void RemoveClientFromRoom(Client c)
        {
            if(clientID1 == c.conID)
            {
                clientID1 = -1;
            }
            else
            {
                clientID2 = -1;
            }
        }
    }

    public class Client//need class to be able to modify existing entries, struct doesnt let you modify
    {
        public int conID;
        public int roomNum;
        public string displayName;
        public Vector3 pos = Vector3.zero;
    }

    Dictionary<int,Client> clients = new Dictionary<int, Client>();
    List<GameRoom> gameRoomsList = new List<GameRoom>();

    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(gameObject); //dont get rid of server between scenes
        InitServer();
    }


    // Update is called once per frame
    void Update()
    {
        MessageLoop();
        /*if(Time.time - lastMoveUpdate > movementUpdateRate)
        {
            lastMoveUpdate = Time.time;
            PositionAskMessage pMsg = new PositionAskMessage();
            SendToAllClients(pMsg, UDPChannel);//send ask for position message to all clients
            

        }*/
    }

    private void InitServer()
    {
        //initializing server variables, adding tcp and udp channels, and adding the host
        NetworkTransport.Init();

        ConnectionConfig cc = new ConnectionConfig();
        TCPChannel = cc.AddChannel(QosType.Reliable);
        UDPChannel = cc.AddChannel(QosType.Unreliable);
        ReliableOrderedChannel = cc.AddChannel(QosType.ReliableSequenced);

        HostTopology topology = new HostTopology(cc, MAX_USERS);

        hostID = NetworkTransport.AddHost(topology, PORT, null);

        serverStarted = true;
        Debug.Log(string.Format("Server starting on port {0}", PORT));
    }

    private void MessageLoop()
    {
        if (!serverStarted)
            return; //dont look through messages if not started

        int recievedHostID; //is this web, standalone, etc. client
        int connectionID; // which user sent message
        int channelID; //which channel was message from(udp,tcp)

        byte[] recievedPacket = new byte[MAX_PACKET_SIZE];
        int dataSize;//how big packet data actually is

        //recieve message and store type
        NetworkEventType messageType = NetworkTransport.Receive(out recievedHostID, out connectionID, out channelID, recievedPacket, MAX_PACKET_SIZE, out dataSize, out error);

        //depending on message type do something
        switch (messageType)
        {
            case NetworkEventType.Nothing: //no message
                break;
            case NetworkEventType.ConnectEvent://connection
                Debug.Log(string.Format("User {0} has connected to the server", connectionID));
                SendConnectedClientInfo(connectionID);  
                break;
            case NetworkEventType.DisconnectEvent://disconnection
                Debug.Log(string.Format("User {0} has disconnected from the server", connectionID));
                SendDisconnectClientInfo(connectionID);
                break;
            case NetworkEventType.DataEvent://custom data
                //Debug.Log(recievedPacket[0]);
                BinaryFormatter formatter = new BinaryFormatter();
                MemoryStream ms = new MemoryStream(recievedPacket);
                //get serialized message and deserialize it back into a normal message
                NetworkMessage msg = (NetworkMessage)formatter.Deserialize(ms);

                DataMessageCheck(connectionID, channelID, recievedHostID, msg);
                break;
            default:
            case NetworkEventType.BroadcastEvent:
                Debug.Log("Unexpected message type");
                break;
        }

    }

    private void SendConnectedClientInfo(int connectionID)
    {
        //add new client to list
        Client c = new Client();
        c.conID = connectionID;
        c.displayName = (string.Format("Player{0}", connectionID));
        ConnectMessage cntMessage = new ConnectMessage();
        cntMessage.cnnID = connectionID;
        //probably allow for actually setting name when connecting id imagine
        cntMessage.name = c.displayName;

        //send these messages in order so that client updates its server connection ID before anything else
        SendMessageToClient(cntMessage,connectionID, ReliableOrderedChannel);
        clients.Add(connectionID, c); //adds client to dictionary through its connection ID

        /*****probably do stuff down here in game start instead, do the room only instead of all clients
        SendCurrentClientList(connectionID, ReliableOrderedChannel);//send list of current clients so they can update their player list to correct initial spot

        ConnectMessageOther coMsg = new ConnectMessageOther();
        coMsg.cnnID = connectionID;
        coMsg.name = c.displayName;
        SendToAllClients(coMsg, ReliableOrderedChannel); //send message that this client connected to everyone so they can update
        */
    }

    private void SendDisconnectClientInfo(int connectionID)
    {
        DisconnectMessage disMsg = new DisconnectMessage();
        disMsg.cnnID = connectionID;

        //SendToAllClients(disMsg, TCPChannel);
        SendToOtherClientInRoom(disMsg, connectionID, TCPChannel, clients[connectionID].roomNum);
        gameRoomsList[clients[connectionID].roomNum].RemoveClientFromRoom(clients[connectionID]);
        clients.Remove(connectionID);//removes corresponding client with id from the server's list
    }

    //sends the current clients status to the new connction so they can get their initial spots
    private void SendCurrentClientList(int connectionID, int roomID, byte channelID)
    {
        foreach(KeyValuePair<int,Client> pair in clients)
        {
            //initializes message contents with each connected clients info
            ConnectMessageOther cMsg = new ConnectMessageOther();
            cMsg.cnnID = pair.Key;
            cMsg.name = pair.Value.displayName;
            cMsg.initX = pair.Value.pos.x;
            cMsg.initY = pair.Value.pos.y;
            cMsg.initZ = pair.Value.pos.z;

            //send message of that client back
            SendMessageToClient(cMsg, connectionID, channelID);

        }
    }
    private void DataMessageCheck(int connectionID, int channelID, int recievedHostID, NetworkMessage msg)
    {
        //do different things depending on the message type, all our custom types
        switch (msg.type)
        {
            case MsgType.NONE:
                Debug.Log("Should not happen");
                break;
            
            case MsgType.POSITION:
                UpdateClientsPosition((PositionMessage)msg);
                break;
            case MsgType.STARTGAME:
            {
                 Client temp = clients[connectionID];
                 AddClientToARoom(ref temp); //add client to a room
                 break;
            }

        }
    }

    //update the position of the client that sent and send that position to others
    void UpdateClientsPosition(PositionMessage pMsg)
    {
        //Debug.Log(string.Format("xPos: {0}, yPos: {1}, zPos: {2}", pMsg.x, pMsg.y, pMsg.z));
        //set clients position to what its local value was
        clients[pMsg.cnnID].pos = new Vector3(pMsg.x, pMsg.y, pMsg.z);

        //send that updated position to the clients in the room(others dont need to know the information)
        SendToOtherClientInRoom(pMsg, pMsg.cnnID, UDPChannel, clients[pMsg.cnnID].roomNum);
    }

    void StartGameRoom(int roomIndex)
    {
        //send game start message to clients in room since room is full and ready to play
        GameStartMessage gsMsg = new GameStartMessage();
        Debug.Log(string.Format("Starting Room#{0}", roomIndex));
        //SendToAllClients(gsMsg, ReliableOrderedChannel);
        SendToAllClientsInRoom(gsMsg, ReliableOrderedChannel, roomIndex);

        //SendCurrentClientList(connectionID, ReliableOrderedChannel);//send list of current clients so they can update their player list to correct initial spot
        //gameRoomsList[roomIndex].clientID1
        //gameRoomsList[roomIndex].clientID2
        int id1 = gameRoomsList[roomIndex].clientID1;
        Debug.Log(string.Format("Slot1: ID={0}", id1));
        int id2 = gameRoomsList[roomIndex].clientID2;
        Debug.Log(string.Format("Slot2: ID={0}", id2));

        ConnectMessageOther coMsg = new ConnectMessageOther();
        coMsg.cnnID = id1;
        coMsg.name = clients[id1].displayName;
        coMsg.initX = clients[id1].pos.x == 0.0f ? -1.5f : clients[id1].pos.x; //if not initialized, give it a default spot
        coMsg.initY = clients[id1].pos.y;
        coMsg.initZ = clients[id1].pos.z;
        //SendToAllClients(coMsg, ReliableOrderedChannel);
        SendToAllClientsInRoom(coMsg, ReliableOrderedChannel,roomIndex); //send message that this client connected to everyone so they can update

        ConnectMessageOther coMsg2 = new ConnectMessageOther();
        coMsg2.cnnID = id2;
        coMsg2.name = clients[id2].displayName;
        coMsg2.initX = clients[id2].pos.x == 0 ? 1.5f : clients[id2].pos.x; //if not initialized, give it a default spot
        coMsg2.initY = clients[id2].pos.y;
        coMsg2.initZ = clients[id2].pos.z;
        //SendToAllClients(coMsg2, ReliableOrderedChannel);
        SendToAllClientsInRoom(coMsg2, ReliableOrderedChannel, roomIndex); //send message that this client connected to everyone so they can update

    }

    void AddClientToARoom(ref Client client)
    {
        //check if there is a room with open space
        for (int i = 0; i < gameRoomsList.Count; i++)
        {
            if(gameRoomsList[i].AddToRoom(client))
            {
                Debug.Log(string.Format("Added ID:{0} to Room#{1}!", client.conID, i));
                client.roomNum = i;
                if(gameRoomsList[i].IsRoomFull())
                {
                    StartGameRoom(i);               
                }
                return;
            }
        }
        //all current rooms are full, make new one
        gameRoomsList.Add(new GameRoom());
        gameRoomsList[gameRoomsList.Count - 1].AddToRoom(client);
        //add client to new room
        client.roomNum = gameRoomsList.Count - 1;
        Debug.Log(string.Format("Added ID:{0} to Room#{1}!", client.conID, gameRoomsList.Count - 1));
        //dont need to check if full since its a new room
    }

    /*
    int GetClientsRoomID(int clientConID)
    {
        return clients[clientConID].roomNum;
    }*/

    public void SendToAllClients(NetworkMessage msg, byte channelID)
    {
        //check every pair in the dictionary(every client that is connected to server
        foreach(KeyValuePair<int,Client> cEntry in clients)
        {
            //key is the connectionID of client so use that
            SendMessageToClient(msg, cEntry.Key, channelID);
        }
    }

    //send message to all clients in a room
    public void SendToAllClientsInRoom(NetworkMessage msg, byte channelID, int roomID)
    {
        if(roomID < gameRoomsList.Count)
        {
            SendMessageToClient(msg, gameRoomsList[roomID].clientID1, channelID);
            SendMessageToClient(msg, gameRoomsList[roomID].clientID2, channelID);
        }
    }

    //send message to other client that is in a room
    public void SendToOtherClientInRoom(NetworkMessage msg, int clientIDToIgnore, byte channelID, int roomID)
    {
        if (roomID < gameRoomsList.Count)
        {
            if (clientIDToIgnore == gameRoomsList[roomID].clientID1)
            {
                SendMessageToClient(msg, gameRoomsList[roomID].clientID2, channelID);
            }
            else
            {
                SendMessageToClient(msg, gameRoomsList[roomID].clientID1, channelID);
            }
        }
    }

    public void SendMessageToClient(NetworkMessage msg, int connectionID, byte channelID)
    {
        //byte array to hold message data
        byte[] buffer = new byte[MAX_PACKET_SIZE];

        //convert data into a byte array
        BinaryFormatter formatter = new BinaryFormatter();
        MemoryStream ms = new MemoryStream(buffer);

        formatter.Serialize(ms, msg);

        NetworkTransport.Send(hostID, connectionID, channelID, buffer, MAX_PACKET_SIZE, out error);
    }

    private void Shutdown()
    {
        NetworkTransport.Shutdown();
        serverStarted = false;
    }
}
