using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

/*
 This playlist was used to help get the setup of the network transport API up and running for client and server
https://www.youtube.com/playlist?list=PLLH3mUGkfFCVSV6Q1UJE7Ge6UfhGzRAnH
  
 */

public class ClientMain : MonoBehaviour
{
    public static ClientMain instance;

    public GameObject playerPrefab;

    const int MAX_USERS = 12;
    const int PORT = 7777;
    const string SERVER_IP = "127.0.0.1";//localhost, server is on this computer
    const int MAX_PACKET_SIZE = 1024;

    private int myClientID;
    private int hostID, connectionID;
    byte TCPChannel, UDPChannel, ReliableOrderedChannel;
    byte error;//error output, https://docs.unity3d.com/ScriptReference/Networking.NetworkError.html

    bool connected;
    bool started;

    public class Player //need class to be able to modify existing entries, struct doesnt let you modify
    {
        public int conID;
        public GameObject obj;
        public string displayName;
    }

    Dictionary<int, ClonedPlayer> players = new Dictionary<int, ClonedPlayer>();


    float lastMoveUpdate;
    float movementUpdateRate = 0.1f;
    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(gameObject); //dont get rid of client between scenes
        if(!instance)
        {
            instance = this;
        }
        InitServer();
    }


    // Update is called once per frame
    void Update()
    {
        MessageLoop();

        if (started && Time.time - lastMoveUpdate > movementUpdateRate)
        {
            lastMoveUpdate = Time.time;
            SendOurPosition();
        }
    }

    private void InitServer()
    {
        //initializing network variables, adding tcp and udp channels, and connecting to the server
        NetworkTransport.Init();

        ConnectionConfig cc = new ConnectionConfig();
        TCPChannel = cc.AddChannel(QosType.Reliable);
        UDPChannel = cc.AddChannel(QosType.Unreliable);
        ReliableOrderedChannel = cc.AddChannel(QosType.ReliableSequenced);

        HostTopology topology = new HostTopology(cc, MAX_USERS);

        hostID = NetworkTransport.AddHost(topology, 0);

        //connect to the server
        connectionID = NetworkTransport.Connect(hostID, SERVER_IP, PORT, 0, out error);

        Debug.Log(string.Format("Attempting connection to IP {0}", SERVER_IP));
        connected = true;
    }

    private void MessageLoop()
    {
        if (!connected)
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
                Debug.Log("Connected to the server");
                break;
            case NetworkEventType.DisconnectEvent://disconnection
                Debug.Log("Disconnected from the server");
                break;
            case NetworkEventType.DataEvent://custom data
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

    private void DataMessageCheck(int connectionID, int channelID, int recievedHostID, NetworkMessage msg)
    {
        //do different things depending on the message type
        switch (msg.type)
        {
            case MsgType.NONE:
                Debug.Log("Should not happen");
                break;
            case MsgType.POSITION:
                SetClientPos((PositionMessage)msg);
                break;
            case MsgType.CLIENTCONNECTSELF:
                ConnectMessage cm = (ConnectMessage)msg;
                Debug.Log(string.Format("Our ID is {0}", cm.cnnID));
                myClientID = cm.cnnID; //initialize our server ID so we know what it is
                break;
            case MsgType.CLIENTCONNECTOTHER:
                SpawnPlayer((ConnectMessageOther)msg);
                break;
            case MsgType.CLIENTDISCONNECT:
                RemovePlayer((DisconnectMessage)msg);
                break;
            case MsgType.STARTGAME:
                Debug.Log("Game started");
                GameObject.Find("Canvas").SetActive(false);
                //remove whatever name setup canvas is there/load different scene
                break;

        }
    }

    private void SetClientPos(PositionMessage msg)
    {
        if(msg.cnnID != myClientID)//only set if not ours
        {
            Vector3 serverPos = new Vector3(msg.x, msg.y, msg.z);

            players[msg.cnnID].obj.GetComponent<ClonedPlayer>().SetServerPosition(serverPos);
        }
    }

    //When player is connected to the server, will be sent a message and need to now spawn that client in our local game
    private void SpawnPlayer(ConnectMessageOther msg) //msg has name, starting position, id too prob
    {
        if (!players.ContainsKey(msg.cnnID)) //only spawn player if its not already in the list(prevents dupes) from someone new joining a room
        {
            Debug.Log(string.Format("Spawning Player with ID: {0}, name: {1}", msg.cnnID, msg.name));
            Vector3 pos = new Vector3(msg.initX, msg.initY, msg.initZ);
            GameObject go = Instantiate(playerPrefab, pos, Quaternion.identity);

            if (msg.cnnID == myClientID)
            {
                //Give player control component to this object since it is ours so we can move it
                go.AddComponent<PlayerMovement>();
                started = true;
                Debug.Log("spawned player");
            }
            else
            {
                go.AddComponent<ClonedPlayer>();
                Debug.Log("spawned other");
            }

            ClonedPlayer p = new ClonedPlayer();
            p.conID = msg.cnnID;
            p.obj = go;
            p.displayName = msg.name;
            p.obj.GetComponentInChildren<TextMesh>().text = msg.name;

            players.Add(msg.cnnID, p);//add player to player list
        }

    }

    //When player is disconnected from server, will be sent a message and do this in response
    private void RemovePlayer(DisconnectMessage msg)
    {
        Destroy(players[msg.cnnID].obj);//remove local copy of the game object
        players.Remove(msg.cnnID);//remove player from players list
    }

    //Sends the position data of our player to the server
    public void SendOurPosition()
    {
        PositionMessage pMsg = new PositionMessage();
        //gets the position of our game object from the player list
        Vector3 pos = players[myClientID].obj.transform.position;
        pMsg.cnnID = myClientID;
        pMsg.x = pos.x;
        pMsg.y = pos.y;
        pMsg.z = pos.z;
        SendMessageToServer(pMsg,UDPChannel);
    }

    public void SendRoomJoinRequest()
    {
        GameStartMessage gsMsg = new GameStartMessage();
        gsMsg.cnnID = myClientID;
        SendMessageToServer(gsMsg, TCPChannel);
        GameObject.Find("RoomFindText").GetComponent<Text>().text = "Waiting for room to be full";
    }

    public void SendMessageToServer(NetworkMessage msg,byte channelID)
    {
        //byte array to hold message data
        byte[] buffer = new byte[MAX_PACKET_SIZE];

        BinaryFormatter formatter = new BinaryFormatter();
        MemoryStream ms = new MemoryStream(buffer);

        //serializes the message before sending it over
        formatter.Serialize(ms, msg);

        NetworkTransport.Send(hostID, connectionID, channelID, buffer, MAX_PACKET_SIZE, out error);
    }

    private void Shutdown()
    {
        NetworkTransport.Shutdown();
        connected = false;
    }
}
