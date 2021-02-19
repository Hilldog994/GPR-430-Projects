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

	main-server.c/.cpp
	Main source for console server application.
*/
/*
 Author:				Stephen Hill & Cameron Murphy
 Class:					GPR-430-02
 Assignment:			Project 2 Asynchronous Networked Architecture & Minigames
 Due Date:              2/26/21
 Purpose:               Handles server operations
*/

#include "gpro-net/gpro-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

const int MAX_CLIENTS = 10;
const int SERVER_PORT = 4024;

/*Base Setup for project/Raknet provided by Daniel Buckstein
http://www.jenkinssoftware.com/raknet/manual/tutorial.html tutorial used for RakNet, tutorial code samples were used
*/


int main(int const argc, char const* const argv[])
{
	bool loop = true;
	
	//output.clear(); //clear log to be only current log saved to file
	//first key is the address gotten from packet->systemAddress.ToString(), second is the nickname
	std::map<RakNet::SystemAddress, std::string> nicknameList;
	//iterator used for looking through map
	std::map<RakNet::SystemAddress, std::string>::iterator iter;

	RakNet::SystemAddress sa;

	int userNameSuffix = 1;

	std::string message;
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(SERVER_PORT,0);
	RakNet::Packet* packet;

	peer->Startup(MAX_CLIENTS, &sd, 1);

	peer->SetOccasionalPing(true);
	printf("Starting server...\n");

	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	RakNet::BitStream bsOut;
	while (loop)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])//checking first message_id
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				message = "Another client has disconnected.\n";
				printf(message.c_str());

				break;
			case ID_REMOTE_CONNECTION_LOST:
				message = "Another client has lost the connection.\n";
				printf(message.c_str());
	
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				message = "Another client has connected.\n";
				printf(message.c_str());


				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				message = "Our connection request has been accepted.\n";
				printf(message.c_str());


				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				//message = packet->systemAddress.ToString();
				sa = packet->systemAddress;
				printf("A connection is incoming.\n");


				//add user to list with name User1,User2, etc.
				nicknameList[sa] = "User" + std::to_string(userNameSuffix);
				printf("User: %s has joined \n", nicknameList[sa].c_str());


				/*
				//send id back to user
				bsOut.Write((RakNet::MessageID)ID_STORE_NAME);
				bsOut.Write(nicknameList[sa].c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				bsOut.Reset();
				*/
				userNameSuffix++;//increase suffix of user name

				break;
			}
			case ID_BS_ATTACK:
			{
				bs_Message msg;

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				bsIn >> msg;

				printf("I1: %c  I2: %s", msg.iIndex, std::to_string(msg.jIndex).c_str());
				break;
			}
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				message = "The server is full.\n";
				printf(message.c_str());

				break;
			case ID_DISCONNECTION_NOTIFICATION:
				//message = packet->systemAddress.ToString();
				sa = packet->systemAddress;
				printf("User: %s has disconnected \n", nicknameList[sa].c_str());

				//remove disconnected user from current user list
				iter = nicknameList.find(sa);
				if (iter != nicknameList.end())
				{
					nicknameList.erase(sa);
				}

				if (nicknameList.size() == 0)
				{
					loop = false;
				}
				break;
			case ID_CONNECTION_LOST:
				//message = packet->systemAddress.ToString();
				sa = packet->systemAddress;
				printf("A client %s lost connection.\n", nicknameList[sa].c_str());

				//remove disconnected user from current user list
				iter = nicknameList.find(sa);
				if (iter != nicknameList.end())
				{
					nicknameList.erase(sa);
				}

				if (nicknameList.size() == 0)
				{
					loop = false;
				}

				break;
			case ID_TIMESTAMP: //actual messages sent from clients will start with timestamp
			{
				//printf("timestamp \n");
				RakNet::RakString rs;
				RakNet::Time time;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(time); //read timestamp

				//https://github.com/facebookarchive/RakNet/blob/master/Samples/Timestamping/Timestamping.cpp line 144 on getting the time to display
				printf("[%s]: ", std::to_string(time).c_str()); //prints time stamp

				//move pointer of data[sizeof(Raknet::MessageID) + sizeof(Raknet::Time)](second part of packet), switch case for that like first one
				switch (packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)])
				{
					case ID_GAME_MESSAGE_1: //is used as the message sent from client joining
					{
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs); //read message
						printf("%s\n", rs.C_String());

						//send a message back to client welcoming them
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						std::string messageToSend = "Welcome to the server ";
						messageToSend += nicknameList[packet->systemAddress];

						bsOut.Write(messageToSend.c_str()); 
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						bsOut.Reset();
						break;
					}
					case ID_CHAT_MESSAGE_1: //message that was typed by a client
					{
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs); //read message
						printf("%s\n", rs.C_String());

						//send a message back to client
						bsOut.Write((RakNet::MessageID)ID_CHAT_MESSAGE_1);
						bsOut.Write(rs);
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						bsOut.Reset();

						//send a message back to client
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write("Message Sent");
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						bsOut.Reset();
						break;
					}
					default:
					{
						message = std::to_string(packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)]);
						printf("Second message with identifier %s has arrived.\n", message.c_str());
						break;
					}
				}
				break;
			}
			case ID_CLOSE_SERVER:
			{
				loop = false;
				break;
			}
			default:
				message = std::to_string(packet->data[0]);
				printf("Message with identifier %s has arrived.\n", message.c_str());
				break;
			}
		}
	}
	//http://www.raknet.net/raknet/manual/detailedimplementation.html for shutting down
	peer->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(peer);
	//output.close();
	system("pause");
}
