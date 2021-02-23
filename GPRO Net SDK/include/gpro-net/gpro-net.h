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

	gpro-net.h
	Main include for framework.
*/

/*
 Author:				Stephen Hill & Cameron Murphy
 Class:					GPR-430-02
 Assignment:			Project 1 Server/Client Chat Application
 Due Date:              2/11/21
 Purpose:               Holds enum for game message types
*/

#ifndef _GPRO_NET_H_
#define _GPRO_NET_H_

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/GetTime.h"

#include "gpro-net/gpro-net.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"
#include "gpro-net/gpro-net-common/gpro-net-gamestate.h"

#include <string>
#include <iostream>

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_CHAT_MESSAGE_1,
	ID_NAMES_REQUEST,
	ID_CLOSE_SERVER,
	ID_PRIVATE_MESSAGE,
	ID_STORE_NAME,
	ID_BS_ATTACK
};

struct bs_Message
{
	int iIndex; // iIndex is A-J
	int jIndex; // jIndex 1-10
};

struct Vec2
{
	Vec2(int xi, int yi) :x(xi), y(yi) {};
	int x;
	int y;
};




namespace RakNet
{
	RakNet::BitStream& operator << (RakNet::BitStream& out, bs_Message& in)
	{
		out.Write((RakNet::MessageID)GameMessages::ID_BS_ATTACK);
		out.Write(in.iIndex);
		out.Write(in.jIndex);
		//maybe also write converted index(1,2 writing B3)
		return out;
	}

	RakNet::BitStream& operator >> (RakNet::BitStream& in, bs_Message& out)
	{
		in.IgnoreBytes(sizeof(RakNet::MessageID));
		in.Read(out.iIndex);
		in.Read(out.jIndex);
		//if writing converted read in
		return in;
	}
}

#endif	// !_GPRO_NET_H_