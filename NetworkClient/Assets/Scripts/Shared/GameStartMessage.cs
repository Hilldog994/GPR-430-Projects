using UnityEngine;

[System.Serializable]
public class GameStartMessage : NetworkMessage
{
    //set message type to correct one
    public GameStartMessage()
    {
        type = MsgType.STARTGAME;
    }

    public int cnnID { get; set; }
    public string name { get; set; }
}
