using UnityEngine;

//Different message types
public static class MsgType
{
    public const int NONE = 0;
    public const int POSITION = 1;
    public const int CLIENTCONNECTSELF = 2;
    public const int CLIENTCONNECTOTHER = 3;
    public const int CLIENTDISCONNECT = 4;
    public const int POSITIONASK = 5;
};



//base network message class
[System.Serializable]
public abstract class NetworkMessage
{
    public byte type { set; get; }

    public NetworkMessage()
    {
        type = MsgType.NONE;
    }
}
