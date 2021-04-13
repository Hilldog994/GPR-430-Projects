using UnityEngine;

public static class MsgType
{
    public const int NONE = 0;
    public const int POSITION = 1;
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
