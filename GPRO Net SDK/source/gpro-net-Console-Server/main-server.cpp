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
	std::ofstream output;
	output.open("TextLog.txt");
	
	//output.clear(); //clear log to be only current log saved to file
	//first key is the address gotten from packet->systemAddress.ToString(), second is the nickname
	std::map<std::string, std::string> nicknameList;
	//iterator used for looking through map
	std::map<std::string, std::string>::iterator iter;

	int userNameSuffix = 1;;

	std::string message;
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(SERVER_PORT,0);
	RakNet::Packet* packet;

	peer->Startup(MAX_CLIENTS, &sd, 1);

	peer->SetOccasionalPing(true);
	printf("Starting server...\n");
	output << "Starting server...\n";
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	//output.close();
	//exit(0);
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
				output << message;
				break;
			case ID_REMOTE_CONNECTION_LOST:
				message = "Another client has lost the connection.\n";
				printf(message.c_str());
				output << message;
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				message = "Another client has connected.\n";
				printf(message.c_str());
				output << message;

				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				message = "Our connection request has been accepted.\n";
				printf(message.c_str());
				output << message;

				break;
			case ID_NEW_INCOMING_CONNECTION:
				message = packet->systemAddress.ToString();
				printf("A connection is incoming.\n");
				output << "A connection is incoming.\n";
				
				//add user to list with name User1,User2, etc.
				nicknameList[message] = "User" + std::to_string(userNameSuffix);
				printf("User: %s has joined \n" , nicknameList[message].c_str());
				output << "User: " << nicknameList[message].c_str() << " has joined \n";
				
				//send id back to user
				bsOut.Write((RakNet::MessageID)ID_STORE_NAME);
				bsOut.Write(nicknameList[message].c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				bsOut.Reset();
				
				userNameSuffix++;//increase suffix of user name

				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				message = "The server is full.\n";
				printf(message.c_str());
				output << message;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				message = packet->systemAddress.ToString();
				printf("User: %s has disconnected \n", nicknameList[message].c_str());
				output << "User: " << nicknameList[message].c_str() << " has disconnected \n";

				//remove disconnected user from current user list
				iter = nicknameList.find(message);
				if (iter != nicknameList.end())
				{
					nicknameList.erase(message);
				}

				//loop = false;
				output.close();
				break;
			case ID_CONNECTION_LOST:
				message = packet->systemAddress.ToString();
				printf("A client %s lost connection.\n", nicknameList[message].c_str());
				output << "A client " << nicknameList[message].c_str() << " lost connection.\n";

				//remove disconnected user from current user list
				iter = nicknameList.find(message);
				if (iter != nicknameList.end())
				{
					nicknameList.erase(message);
				}

				output.close();
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
				output << "[" << std::to_string(time)<< "]: ";

				//move pointer of data[sizeof(Raknet::MessageID) + sizeof(Raknet::Time)](second part of packet), switch case for that like first one
				switch (packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)])
				{
					case ID_GAME_MESSAGE_1: //is used as the message sent from client joining
					{
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs); //read message
						printf("%s\n", rs.C_String());
						output << rs.C_String() << "\n";

						//send a message back to client welcoming them
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						std::string messageToSend = "Welcome to the server ";
						messageToSend += nicknameList[packet->systemAddress.ToString()];

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
						output << rs.C_String() << "\n";

						//send a message back to client
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write("Message Sent");
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						bsOut.Reset();
						break;
					}
					case ID_PRIVATE_MESSAGE: //taking message sent by client and sending it to specified user
					{
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs); //read message

						std::string text = rs.C_String();

						std::string user;
						std::string message;
						bool onMessage = false;
						bool userExists = false;

					    //Seperates the users name and the message
						for (char& c : text)
						{
							if (onMessage)
							{
								message += c;
							}
							else if (c != '(')
							{
								if (c == ')')
								{
									onMessage = true;
								}
								else
								{
									user += c;
								}
							}
						}

						iter = nicknameList.find(user);

						if (iter != nicknameList.end()) //If users exists sends private message
						{
							text = nicknameList[packet->systemAddress.ToString()] + ": " + message + " (Private)\n";
							//Sends private message (stub)
						}
						else //If not sends as regular message
						{
							text = nicknameList[packet->systemAddress.ToString()] + ": " + text + " (Public)\n";
							printf(text.c_str());
							output << text << "\n";
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write("Private Message Sent");
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
							bsOut.Reset();
							
						}
						break;
					}
					default:
					{
						message = std::to_string(packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)]);
						printf("Second message with identifier %s has arrived.\n", message.c_str());
						output << "Second message with identifier " << message << " has arrived.\n";
						break;
					}
				}
				break;
			}
			case ID_NAMES_REQUEST:
			{
				std::string namesList="\n";

				//look through nickname list, add each name to the list to send
				for (iter = nicknameList.begin(); iter != nicknameList.end(); iter++)
				{
					namesList = namesList + iter->second.c_str() + "\n";
				}

				//send list of user names to client
				bsOut.Write((RakNet::MessageID)ID_NAMES_REQUEST);
				bsOut.Write(namesList.c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				bsOut.Reset();
				break;
			}
			case ID_GAME_MESSAGE_1: //was used to test sending back messages to client, not used anymore as packets now start with timestamp
			{
				printf("message \n");
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());

				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				bsOut.Write(" World");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				break;
			}
			case ID_CLOSE_SERVER:
			{
				loop = false;
				output.close();
				break;
			}
			default:
				message = std::to_string(packet->data[0]);
				printf("Message with identifier %s has arrived.\n", message.c_str());
				output << "Message with identifier " << message << " has arrived.\n";
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
