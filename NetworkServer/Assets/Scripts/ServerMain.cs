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



public class ServerMain : MonoBehaviour
{
    const int MAX_USERS = 12;
    const int PORT = 7777;
    const int MAX_PACKET_SIZE = 1024;

    int hostID;
    byte TCPChannel, UDPChannel, ReliableOrderedChannel;
    byte error;//error output


    bool serverStarted;

    public class Client//need class to be able to modify existing entries, struct doesnt let you modify
    {
        public int conID;
        public string displayName;
        public Vector3 pos;
    }

    Dictionary<int,Client> clients = new Dictionary<int, Client>();
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
        cntMessage.name = c.displayName;

        //send these messages in order so that client updates its server connection ID before anything else
        SendMessageToClient(cntMessage,connectionID, ReliableOrderedChannel);
        SendCurrentClientList(connectionID, ReliableOrderedChannel);//send list of current clients so they can update their player list to correct initial spot
        clients.Add(connectionID, c); //adds client to dictionary through its connection ID
        ConnectMessageOther coMsg = new ConnectMessageOther();
        coMsg.cnnID = connectionID;
        coMsg.name = c.displayName;
        SendToAllClients(coMsg, ReliableOrderedChannel); //send message that this client connected to everyone so they can update

    }

    private void SendDisconnectClientInfo(int connectionID)
    {
        clients.Remove(connectionID);//removes corresponding client with id from the server's list

        DisconnectMessage disMsg = new DisconnectMessage();
        disMsg.cnnID = connectionID;

        SendToAllClients(disMsg, TCPChannel);
    }

    private void SendCurrentClientList(int connectionID,byte channelID)
    {
        foreach(KeyValuePair<int,Client> pair in clients)
        {
            ConnectMessageOther cMsg = new ConnectMessageOther();
            cMsg.cnnID = pair.Key;
            cMsg.name = pair.Value.displayName;
            cMsg.initX = pair.Value.pos.x;
            cMsg.initY = pair.Value.pos.y;
            cMsg.initZ = pair.Value.pos.z;

            SendMessageToClient(cMsg, connectionID, channelID);

        }
    }
    private void DataMessageCheck(int connectionID, int channelID, int recievedHostID, NetworkMessage msg)
    {
        //do different things depending on the message type
        switch (msg.type)
        {
            case MsgType.NONE:
                Debug.Log("Should not happen");
                break;
            
            case MsgType.POSITION:
                UpdateClientsPosition((PositionMessage)msg);
                break;
            
        }
    }

    void UpdateClientsPosition(PositionMessage pMsg)
    {
        Debug.Log(string.Format("xPos: {0}, yPos: {1}, zPos: {2}", pMsg.x, pMsg.y, pMsg.z));
        clients[pMsg.cnnID].pos = new Vector3(pMsg.x, pMsg.y, pMsg.z);
        SendToAllClients(pMsg, UDPChannel);
    }

    public void SendToAllClients(NetworkMessage msg, byte channelID)
    {
        //check every pair in the dictionary
        foreach(KeyValuePair<int,Client> cEntry in clients)
        {
            //key is the connectionID of client so use that
            SendMessageToClient(msg, cEntry.Key, channelID);
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
