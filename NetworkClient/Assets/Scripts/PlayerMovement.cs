using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    private CharacterController cont;
    float moveSpeed = 4.0f;
    float jumpSpeed = 20.0f;
    float gravity = 60f;

    float vertVelocity;

    // Start is called before the first frame update
    void Start()
    {
        cont = GetComponent<CharacterController>();
    }

    // Update is called once per frame
    void Update()
    {
        CheckInput();
    }

    private void CheckInput()
    {
        Vector3 move = Vector2.zero;
        move.x = Input.GetAxis("Horizontal");
        move.z = Input.GetAxis("Vertical");

        move *= moveSpeed;

        if(cont.isGrounded && Input.GetButtonDown("Jump"))
        {
            vertVelocity = jumpSpeed;
        }
        else
        {
            vertVelocity -= gravity * Time.deltaTime;
        }

        move.y = vertVelocity;

        cont.Move(move * Time.deltaTime);
    }
}
