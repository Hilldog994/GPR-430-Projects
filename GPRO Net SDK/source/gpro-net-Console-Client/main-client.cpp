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

//#include "gpro-net/gpro-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "gpro-net/gpro-net-common/battleship.h"

/*Base Setup for project/Raknet provided by Daniel Buckstein
http://www.jenkinssoftware.com/raknet/manual/tutorial.html tutorial used for RakNet, tutorial code samples were used
*/

#define IP_ADDRESS = "172.16.2.56"; //dont work
const int SERVER_PORT = 4024;

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

	//Battleship battleship;

//setup battleship boards(placement of ships)
	SetupBoard(mBoard1, mBoard2, 1);
	//pause for player 2
	printf("Player 2's turn to place\n");
	system("pause");
//Player 2 setup
	SetupBoard(mBoard1, mBoard2, 2);
	//gpro_consoleClear();
	//gpro_consoleResetColor();
	
	//PrintBoards(mBoard1, mBoard2, 3);
	//gpro_consoleClear();
	//gpro_consoleResetColor();
	
//actual game
	bool loopGame = true;
	while (loopGame)
	{
		printf("Player 1's turn to attack\n");
		system("pause");
		AttackPhase(mBoard1, mBoard2, 1);

		printf("Player 2's turn to attack\n");
		system("pause");

		gpro_consoleClear();
		gpro_consoleResetColor();
		AttackPhase(mBoard1, mBoard2, 2);
	}


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
