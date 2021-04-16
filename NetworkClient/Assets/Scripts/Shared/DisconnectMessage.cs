using UnityEngine;

[System.Serializable]
public class DisconnectMessage : NetworkMessage
{
    //set message type to correct one
    public DisconnectMessage()
    {
        type = MsgType.CLIENTDISCONNECT;
    }

    public int cnnID { get; set; }
}
