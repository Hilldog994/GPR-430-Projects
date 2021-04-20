using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClonedPlayer : MonoBehaviour
{
    public int conID;
    public GameObject obj;
    public string displayName;

    public Vector3 lastServerUpdatePos;
    float distanceThreshold = 6f;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        LerpToServer();   
    }

    public void SetServerPosition(Vector3 pos)
    {
        lastServerUpdatePos = pos;
        if (Vector3.Distance(transform.position,pos) > distanceThreshold)
        {
            transform.position = lastServerUpdatePos;
            print("Too far");
        }
    }

    private void LerpToServer()
    {
        //get current position
        //compare with gotten position
        //Vector3 lerped = Vector3.Lerp(currentPos, serverPos, .7f);
        //lerp it up
        //set position
        transform.position = Vector3.Lerp(transform.position, lastServerUpdatePos, .2f);
    }
}
