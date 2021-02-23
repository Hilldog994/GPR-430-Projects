/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-client.c/.cpp
	Main source for console client application.
*/

/*
 Author:				Stephen Hill & Cameron Murphy
 Class:					GPR-430-02
 Assignment:			Project 2 Asynchronous Networked Architecture & Minigames
 Due Date:              2/26/21
 Purpose:               Handles client operations
*/

#include "gpro-net/gpro-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>



/*Base Setup for project/Raknet provided by Daniel Buckstein
http://www.jenkinssoftware.com/raknet/manual/tutorial.html tutorial used for RakNet, tutorial code samples were used
*/

#define IP_ADDRESS = "172.16.2.56"; //dont work
const int SERVER_PORT = 4024;

void PrintBoards(gpro_battleship board1, gpro_battleship board2, int playerIndex)
{
	if (playerIndex == 1)
	{
		gpro_consoleResetColor();
		printf("Player2 Board(enemy)\n");
		gpro_battleship_display_board(board2, false);
		gpro_consoleResetColor();
		printf("Player1 Board(yours)\n");
		gpro_battleship_display_board(board1, true);
	}
	else if (playerIndex == 2)
	{
		gpro_consoleResetColor();
		printf("Player1 Board(enemy)\n");
		gpro_battleship_display_board(board1, false);
		gpro_consoleResetColor();
		printf("Player2 Board(yours)\n");
		gpro_battleship_display_board(board2, true);
	}
	else if (playerIndex == 3)//spectator view, both sides with ship available
	{
		gpro_consoleResetColor();
		printf("Player1 Board\n");
		gpro_battleship_display_board(board1, true);
		gpro_consoleResetColor();
		printf("Player2 Board\n");
		gpro_battleship_display_board(board2, true);
	}
}

//takes in input(something like B4) and translates into the correct spot in gpro_battleship, input will be in correct format
Vec2 ProcessInput(std::string input)
{
	char first = input[0];//A-J
	//stoi converts to int, substr gets 2nd index of string onwards(goes 2 incase of 10)
	int second = std::stoi(input.substr(1, 2));//1-10
	//if A, 'A' - A will be 0, 'B' gives 1, etc. number will be one more than index
	return Vec2(first - 'A', second - 1);
}

//check if input is within board bounds
bool IsInputValid(std::string input)
{
	//check if input is in correct format, and is within range of board
	if (input[0] >= 'A' && input[0] <= 'J')
	{
		if (std::stoi(input.substr(1, 3)) >= 1 && std::stoi(input.substr(1, 3)) <= 10) //check 3 spaces incase 100 or more is put
		{
			return true;
		}
	}
	printf("Invalid Entry, try again\n");
	return false;
}


bool isSpaceOpen(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length)
{
	if (direction == 0)
	{
		if (startLocation.y - length >= -1) //make sure ship would fit if open
		{
			for (int i = 1; i < length; i++)
			{
				if (gpro_flag_check(boardToUse[startLocation.x][startLocation.y - i], gpro_battleship_flag::gpro_battleship_ship))//check if ship already in spot
				{
					printf("Ship will not fit there, try again\n");
					return false;
				}
			}
			return true;
		}
		else
		{
			printf("Not enough space for ship, try again\n");
			return false;
		}
	}
	else if (direction == 1)
	{
		if (startLocation.x - length >= -1) //make sure ship would fit if open
		{
			for (int i = 1; i < length; i++)
			{
				if (gpro_flag_check(boardToUse[startLocation.x - i][startLocation.y], gpro_battleship_flag::gpro_battleship_ship))//check if ship already in spot
				{
					printf("Ship will not fit there, try again\n");
					return false;
				}
			}
			return true;
		}
		else
		{
			printf("Not enough space for ship, try again\n");
			return false;
		}
	}
	else if (direction == 2)
	{
		if (startLocation.y + length <= 10) //make sure ship would fit if open
		{
			for (int i = 1; i < length; i++)
			{
				if (gpro_flag_check(boardToUse[startLocation.x][startLocation.y + i], gpro_battleship_flag::gpro_battleship_ship))//check if ship already in spot
				{
					printf("Ship will not fit there, try again\n");
					return false;
				}
			}
			return true;
		}
		else
		{
			printf("Not enough space for ship, try again\n");
			return false;
		}
	}
	else if (direction == 3)
	{
		if (startLocation.x + length <= 10) //make sure ship would fit if open
		{
			for (int i = 1; i < length; i++)
			{
				if (gpro_flag_check(boardToUse[startLocation.x+i][startLocation.y], gpro_battleship_flag::gpro_battleship_ship))//check if ship already in spot
				{
					printf("Ship will not fit there, try again\n");
					return false;
				}
			}
			return true;
		}
		else
		{
			printf("Not enough space for ship, try again\n");
			return false;
		}
	}
	return false;
}

void PopulateBoardWithShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length, gpro_battleship_flag shipType)
{
	if (direction == 0)
	{
		for (int i = 1; i < length; i++)
		{
			boardToUse[startLocation.x][startLocation.y - i] += shipType;
		}
	}
	else if (direction == 1)
	{
		for (int i = 1; i < length; i++)
		{
			boardToUse[startLocation.x - i][startLocation.y] += shipType;
		}
	}
	else if (direction == 2)
	{
		for (int i = 1; i < length; i++)
		{
			boardToUse[startLocation.x][startLocation.y + i] += shipType;
		}
	}
	else if (direction == 3)
	{
		for (int i = 1; i < length; i++)
		{
			boardToUse[startLocation.x + i][startLocation.y] += shipType;
		}
	}
}

//0 is left, 1 is up, 2 is right, 3 is down
bool CheckAndCreateShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation)
{
	switch (boardToUse[startLocation.x][startLocation.y])
	{
		case gpro_battleship_flag::gpro_battleship_ship_p2:
		{
			if (isSpaceOpen(boardToUse, direction, startLocation, 2))//if space is open for ship
			{
				PopulateBoardWithShip(boardToUse, direction, startLocation, 2, gpro_battleship_flag::gpro_battleship_ship_p2);
				return true;
			}
			else
			{
				return false;
			}

			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_d3:
		{
			if (isSpaceOpen(boardToUse, direction, startLocation, 3))//if space is open for ship
			{
				PopulateBoardWithShip(boardToUse, direction, startLocation, 3, gpro_battleship_flag::gpro_battleship_ship_d3);
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_s3:
		{
			if (isSpaceOpen(boardToUse, direction, startLocation, 3))//if space is open for ship
			{
				PopulateBoardWithShip(boardToUse, direction, startLocation, 3, gpro_battleship_flag::gpro_battleship_ship_s3);
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_b4:
		{
			if (isSpaceOpen(boardToUse, direction, startLocation, 4))//if space is open for ship
			{
				PopulateBoardWithShip(boardToUse, direction, startLocation, 4, gpro_battleship_flag::gpro_battleship_ship_b4);
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_c5:
		{
			if (isSpaceOpen(boardToUse, direction, startLocation, 5))//if space is open for ship
			{
				PopulateBoardWithShip(boardToUse, direction, startLocation, 5, gpro_battleship_flag::gpro_battleship_ship_c5);
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
		default:
			return false;
			break;
	}
	return false;
}
//sets up a single ship, rerun with all types of ships
void SetupShip(gpro_battleship& board1, gpro_battleship& board2, int playerIndex, gpro_battleship_flag shipType)
{
	bool inputValid = false;
	std::string input;
	Vec2 loc(0,0);

	while (!inputValid)
	{
		std::getline(std::cin, input); //get input
		inputValid = IsInputValid(input.c_str());
		if (playerIndex == 1 && inputValid)//check if board spot is already filled
		{
			loc = ProcessInput(input.c_str());
			inputValid = !gpro_flag_check(board1[loc.x][loc.y], gpro_battleship_ship);
		}
		else if (playerIndex == 2 && inputValid)
		{
			loc = ProcessInput(input.c_str());
			inputValid = !gpro_flag_check(board2[loc.x][loc.y], gpro_battleship_ship);
		}
	}
	inputValid = false;
	loc = ProcessInput(input.c_str());
	
	
	switch (shipType)
	{
		case gpro_battleship_flag::gpro_battleship_ship_p2:
		{
			if (playerIndex == 1)
			{
				board1[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_p2;
			}
			else if (playerIndex == 2)
			{
				board2[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_p2;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_d3:
		{
			if (playerIndex == 1)
			{
				board1[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_d3;
			}
			else if (playerIndex == 2)
			{
				board2[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_d3;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_s3:
		{
			if (playerIndex == 1)
			{
				board1[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_s3;
			}
			else if (playerIndex == 2)
			{
				board2[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_s3;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_b4:
		{
			if (playerIndex == 1)
			{
				board1[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_b4;
			}
			else if (playerIndex == 2)
			{
				board2[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_b4;
			}
			break;
		}
		case gpro_battleship_flag::gpro_battleship_ship_c5:
		{
			if (playerIndex == 1)
			{
				board1[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_c5;
			}
			else if (playerIndex == 2)
			{
				board2[loc.x][loc.y] += gpro_battleship_flag::gpro_battleship_ship_c5;
			}
			break;
		}
	}
	while (!inputValid) //check for direction to put ship, check if valid
	{
		printf("Which direction for the rest of the ship? (0=left, 1=up, 2=right, 3=down)\n");
		std::getline(std::cin, input);
		if (input.length() == 1)
		{
			if (std::stoi(input) >= 0 && std::stoi(input) <= 3) //input is between 0 and 3
			{
				if (playerIndex == 1)
				{
					inputValid = CheckAndCreateShip(board1, std::stoi(input), loc);
				}
				else if (playerIndex == 2)
				{
					inputValid = CheckAndCreateShip(board2, std::stoi(input), loc);
				}
			}
			else
			{
				printf("Enter a valid direction\n");
			}
		}
	}
	inputValid = false;
	gpro_consoleClear();
	gpro_consoleResetColor();
	PrintBoards(board1, board2, playerIndex); //print board again to show ship placed
	gpro_consoleResetColor();
}

void SetupBoard(gpro_battleship& board1, gpro_battleship& board2, int playerIndex)
{
	std::string input;
	bool inputValid = false;

	gpro_consoleClear();
	gpro_consoleResetColor();
	PrintBoards(board1, board2, playerIndex);

	gpro_consoleResetColor();
	printf("Setup Ships Player %i. \n", playerIndex);
//Ship 1
	printf("Enter starting position of patrol ship(size 2)\n");
	SetupShip(board1, board2, playerIndex, gpro_battleship_ship_p2);
 //Ship2
	printf("Enter starting position of submarine ship(size 3)\n");
	SetupShip(board1, board2, playerIndex, gpro_battleship_ship_s3);
//Ship 3
	printf("Enter starting position of destroyer ship(size 3)\n");
	SetupShip(board1, board2, playerIndex, gpro_battleship_ship_d3);
//Ship4
	printf("Enter starting position of battleship ship(size 4)\n");
	SetupShip(board1, board2, playerIndex, gpro_battleship_ship_b4);
//Ship5
	printf("Enter starting position of carrier ship(size 5)\n");
	SetupShip(board1, board2, playerIndex, gpro_battleship_ship_c5);

}

int main(int const argc, char const* const argv[])
{
	//std::string test;
	//std::string displayName;
	//RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	//RakNet::SocketDescriptor sd;
	//RakNet::Packet* packet;

	//peer->Startup(1, &sd, 1);

	//printf("Insert display name.\n");
	//std::getline(std::cin, displayName);

	printf("Starting client... \n");
//SP battleship stuff
//----------------------------------------------------------------------------------------------
//Setup Game
	gpro_consoleClear();
	gpro_consoleToggleCursor(true);
	//create player boards and reset them
	gpro_battleship mBoard1;//player 1 board
	gpro_battleship mBoard2;//player 2 board
	gpro_battleship_reset(mBoard1);
	gpro_battleship_reset(mBoard2);

	//setup battleship boards(placement of ships)
	SetupBoard(mBoard1, mBoard2, 1);
	//pause for player 2
	printf("Player 2's turn to place\n");
	system("pause");
	//Player 2 setup
	SetupBoard(mBoard1, mBoard2, 2);
	gpro_consoleClear();
	gpro_consoleResetColor();
	
	//PrintBoards(mBoard1, mBoard2, 3);
	//gpro_consoleClear();
	//gpro_consoleResetColor();
	

	//attacking will only have to check if it is gpro_battleship_ship

//---------------------------------------------------------------------------------------------
//end of SP battleship stuff

	/*peer->Connect("172.16.2.59", SERVER_PORT, 0, 0);

	RakNet::BitStream bsOut;
	//RakNet::Time time;
	bool loop = true;


	while (loop)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0]) //checking first message_id
			{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					printf("Another client has disconnected.\n");
					break;
				case ID_REMOTE_CONNECTION_LOST:
					printf("Another client has lost the connection.\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					printf("Another client has connected.\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					bs_Message msg;
					printf("Our connection request has been accepted.\n");
					msg.iIndex = 'A';
					msg.jIndex = 1;
					bsOut << msg;
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					/*bsOut.Write((RakNet::MessageID)ID_TIMESTAMP);
					time = RakNet::GetTime();
					bsOut.Write(time);
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write("Hello world");
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					bsOut.Reset();

					break;
				}

				case ID_NEW_INCOMING_CONNECTION:
					printf("A connection is incoming.\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("We have been disconnected.\n");
					break;
				case ID_CONNECTION_LOST:
					printf("Connection lost.\n");
					break;
				case ID_GAME_MESSAGE_1:
				{
					//get message sent from server
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("Recieved: %s\n\n", rs.C_String());
					/*
					printf("Type your message(type /quit to exit | /names to get a list of connected users | put a users name in paranthesis to privately message them\n");

					std::getline(std::cin, test); //get input

					//Determines if text is message or command
					if (test == "/quit") //If quitting cancels the loop
					{
						loop = false;
					}
					else if (test == "/names") //sends message to get names from server
					{
						bsOut.Write((RakNet::MessageID)ID_NAMES_REQUEST);
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						bsOut.Reset();
					}
					else if (test.front() == '(' && test.find(')') != std::string::npos) //Determines if message starts with an opening paranthesis and closes at some point (private message)
					{
						//write timestamp and typed message and send to server
						bsOut.Write((RakNet::MessageID)ID_TIMESTAMP);
						time = RakNet::GetTime();
						bsOut.Write(time);

						bsOut.Write((RakNet::MessageID)ID_PRIVATE_MESSAGE);
						bsOut.Write(test.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						bsOut.Reset();
					}
					else //If none of those commands is a regular message
					{
						test = displayName + ": " + test + " (Public)";

						//write timestamp and typed message and send to server
						bsOut.Write((RakNet::MessageID)ID_TIMESTAMP);
						time = RakNet::GetTime();
						bsOut.Write(time);
						
						bsOut.Write((RakNet::MessageID)ID_CHAT_MESSAGE_1);
						bsOut.Write(test.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						bsOut.Reset();
					}
					break;
				}
				case ID_CHAT_MESSAGE_1: //Receives chat message
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					printf("%s\n", rs.C_String());
					break;
				}
				case ID_STORE_NAME: //stores users name in clients script
				{
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					displayName = rs;
					break;
				}
				default:
				{
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
				}
			}
		}
	}*/
	//http://www.raknet.net/raknet/manual/detailedimplementation.html for shutting down
	//peer->Shutdown(300);
	//RakNet::RakPeerInterface::DestroyInstance(peer);
	system("pause");
}
