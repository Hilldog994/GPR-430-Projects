using System;
using System.Collections;
using System.Collections.Generic;
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
    byte TCPChannel, UDPChannel;
    byte error;//error output


    bool serverStarted;

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
    }

    private void InitServer()
    {
        //initializing server variables, adding tcp and udp channels, and adding the host
        NetworkTransport.Init();

        ConnectionConfig cc = new ConnectionConfig();
        TCPChannel = cc.AddChannel(QosType.Reliable);
        UDPChannel = cc.AddChannel(QosType.Unreliable);

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
                break;
            case NetworkEventType.DisconnectEvent://disconnection
                Debug.Log(string.Format("User {0} has disconnected from the server", connectionID));
                break;
            case NetworkEventType.DataEvent://custom data
                Debug.Log("Custom data");
                break;
            default:
            case NetworkEventType.BroadcastEvent:
                Debug.Log("Unexpected message type");
                break;
        }

    }

    private void Shutdown()
    {
        NetworkTransport.Shutdown();
        serverStarted = false;
    }
}
