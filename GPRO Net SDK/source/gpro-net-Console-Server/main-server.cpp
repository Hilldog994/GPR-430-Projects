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
#include <string.h>


#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/GetTime.h"
#include "RakNet/Kbhit.h"
#include "RakNet/Gets.h"

const int MAX_CLIENTS = 10;
const int SERVER_PORT = 4024;

/*Base Setup for project/Raknet provided by Daniel Buckstein
http://www.jenkinssoftware.com/raknet/manual/tutorial.html tutorial used for RakNet, tutorial code samples were used
*/

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_CHAT_MESSAGE_1
};

int main(int const argc, char const* const argv[])
{
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(SERVER_PORT,0);
	RakNet::Packet* packet;

	peer->Startup(MAX_CLIENTS, &sd, 1);

	peer->SetOccasionalPing(true);
	printf("Starting server...\n");
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	RakNet::BitStream bsOut;
	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])//checking first message_id
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
				printf("Our connection request has been accepted.\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection with address %s is incoming.\n", packet->systemAddress.ToString());
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("A client with address %s has disconnected.\n", packet->systemAddress.ToString());
				break;
			case ID_CONNECTION_LOST:
				printf("A client with address %s lost connection.\n", packet->systemAddress.ToString());
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
				printf("%" PRINTF_64_BIT_MODIFIER "u ", time); //prints time stamp

				//move pointer of data[sizeof(Raknet::MessageID) + sizeof(Raknet::Time)](second part of packet), switch case for that like first one
				switch (packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)])
				{
					case ID_GAME_MESSAGE_1: //is used as the message sent from client joining
					{
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs); //read message
						printf("%s\n", rs.C_String());

						//send a message back to client
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write("Welcome to Server"); 
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
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write("Message Sent");
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						bsOut.Reset();
						break;
					}
					default:
					{
						printf("Second message with identifier %i has arrived.\n", packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)]);
						break;
					}
				}
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
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}
	RakNet::RakPeerInterface::DestroyInstance(peer);
	system("pause");
}
