using UnityEngine;

//Different message types
public static class MsgType
{
    public const byte NONE = 0;
    public const byte POSITION = 1;
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
