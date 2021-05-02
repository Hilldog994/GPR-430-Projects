using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    private CharacterController cont;
    public float moveSpeed = 4.5f;
    public float jumpSpeed = 20.0f;
    public float gravity = 60f;
    Vector3 move = Vector2.zero;

    float vertVelocity;

    // Start is called before the first frame update
    void Start()
    {
        cont = GetComponent<CharacterController>();
    }

    private void Update()
    {
        CheckInput();
    }
    // Update is called once per frame
    void FixedUpdate()
    {
        Move();
    }

    public Vector3 GetVelocity()
    {
        if (cont)
            return cont.velocity;
        else
            return Vector3.zero;
    }
    private void Move()
    {
        if (!cont.isGrounded)
        {
            vertVelocity -= gravity * Time.fixedDeltaTime;
        }
        move.y = vertVelocity;
        cont.Move(move * Time.fixedDeltaTime);
    }

    private void CheckInput()
    {
        move.x = Input.GetAxis("Horizontal");
        move.z = Input.GetAxis("Vertical");

        move *= moveSpeed;

        if(cont.isGrounded && Input.GetButtonDown("Jump"))
        {
            vertVelocity = jumpSpeed;
        }

    }
}
