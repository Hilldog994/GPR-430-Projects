using UnityEngine;

[System.Serializable]
public class ConnectMessage : NetworkMessage
{
    //set message type to correct one
    public ConnectMessage()
    {
        type = MsgType.CLIENTCONNECTSELF;
    }

    public int cnnID { get; set; }
    public string name { get; set; }
}
