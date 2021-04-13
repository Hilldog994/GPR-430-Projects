﻿using UnityEngine;

[System.Serializable]
public class PositionMessage : NetworkMessage
{
    //set message type to correct one
    public PositionMessage()
    {
        type = MsgType.POSITION;
    }

    //position data, floats instead of vector3 because it has to be serializable 
    public float x { get; set; }
    public float y { get; set; }
    public float z { get; set; }
}
