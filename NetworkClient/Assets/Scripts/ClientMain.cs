﻿using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

/*
 This playlist was used to help get the setup of the network transport API up and running for client and server
https://www.youtube.com/playlist?list=PLLH3mUGkfFCVSV6Q1UJE7Ge6UfhGzRAnH
  
 */

public class ClientMain : MonoBehaviour
{
    const int MAX_USERS = 12;
    const int PORT = 7777;
    const string SERVER_IP = "127.0.0.1";//localhost, server is on this computer
    const int MAX_PACKET_SIZE = 1024;


    int hostID;
    byte TCPChannel, UDPChannel;
    byte error;//error output, https://docs.unity3d.com/ScriptReference/Networking.NetworkError.html

    bool connected;

    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(gameObject); //dont get rid of client between scenes
        InitServer();
    }


    // Update is called once per frame
    void Update()
    {
        MessageLoop();
    }

    private void InitServer()
    {
        //initializing network variables, adding tcp and udp channels, and connecting to the server
        NetworkTransport.Init();

        ConnectionConfig cc = new ConnectionConfig();
        TCPChannel = cc.AddChannel(QosType.Reliable);
        UDPChannel = cc.AddChannel(QosType.Unreliable);

        HostTopology topology = new HostTopology(cc, MAX_USERS);

        hostID = NetworkTransport.AddHost(topology, 0);

        //connect to the server
        NetworkTransport.Connect(hostID, SERVER_IP, PORT, 0, out error);

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
        connected = false;
    }
}
