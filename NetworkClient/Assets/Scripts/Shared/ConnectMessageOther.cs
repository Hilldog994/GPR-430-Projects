using UnityEngine;

[System.Serializable]
public class ConnectMessageOther : NetworkMessage
{
    //set message type to correct one
    public ConnectMessageOther()
    {
        type = MsgType.CLIENTCONNECTOTHER;
    }

    public int cnnID { get; set; }
    public string name { get; set; }

    public float initX { get; set; }
    public float initY { get; set; }
    public float initZ { get; set; }
}
