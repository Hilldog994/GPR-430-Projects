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
    public Vector3 lastServerUpdateVel;

    public Vector3 positionToLerpTo;
    float distanceThreshold = 4f;
    // Start is called before the first frame update
    void Start()
    {
        positionToLerpTo = transform.position;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        LerpToServer();   
    }

    public void SetServerPosition(Vector3 pos, Vector3 vel)
    {
        lastServerUpdatePos = pos;
        lastServerUpdateVel = vel;
        //get current position through kinematics
        positionToLerpTo = transform.position + lastServerUpdateVel * Time.fixedDeltaTime;
        //compare with gotten position from server
        if (Vector3.Distance(transform.position,pos) > distanceThreshold)
        {
            //if its too far away from your predicted location, go directly there instead of prediction
            positionToLerpTo = lastServerUpdatePos;
            print("Too far");
        }
        else
        {
            positionToLerpTo = Vector3.Lerp(transform.position, lastServerUpdatePos, .7f); //lerp towards server position, 0 favors client, 1 favors server
        }
    }

    private void LerpToServer()
    {          
        //lerp towards the predicted position so its smooth
        transform.position = Vector3.Lerp(transform.position, positionToLerpTo, Time.deltaTime * 10.0f);
    }
}
