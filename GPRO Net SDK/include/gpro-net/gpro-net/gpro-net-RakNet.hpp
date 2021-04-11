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

	gpro-net-RakNet.hpp
	Header for RakNet common management.
*/

#ifndef _GPRO_NET_RAKNET_HPP_
#define _GPRO_NET_RAKNET_HPP_
#ifdef __cplusplus


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/GetTime.h"


namespace gproNet
{
	// eSettings
	//	Enumeration of common settings.
	enum eSettings
	{
		SET_GPRO_SERVER_PORT = 7777,
		SET_GPRO_MAX_CLIENTS = 10,
		SET_GPRO_MAX_POSES = 1000,
		SET_GPRO_MAX_POSES_PER_CLIENT = SET_GPRO_MAX_POSES / SET_GPRO_MAX_CLIENTS,
	};


	// eMessageCommon
	//	Enumeration of custom common message identifiers.
	enum eMessageCommon
	{
		ID_GPRO_MESSAGE_COMMON_BEGIN = ID_USER_PACKET_ENUM,

		ID_GPRO_MESSAGE_SENDPOSEDATA,

		ID_GPRO_MESSAGE_COMMON_END
	};

	// Description of spatial pose.
	struct sSpatialPose
	{
		short clientsID, poseID;
		float scale[3];     // non-uniform scale
		float rotate[4];    // orientation as uniform quaternion
		char compressedQuat[8];
		//x,y,z,w [-1,1] for each value
		//x^2 + y^2 + z^2 + w^2 = 1
		float translate[3]; // translation

		//float translateMatrix[4][4];
		/*
		x x x x
		x y x x
		x x z x
		x x x x
		*/

		//[-127,127]

		// read from stream
		RakNet::BitStream& Read(RakNet::BitStream& bitstream,const sSpatialPose& pose)
		{
			bitstream.Read(pose.scale[0]);
			bitstream.Read(pose.scale[1]);
			bitstream.Read(pose.scale[2]);
			//read in the compressed quaternion
			bitstream.Read(pose.compressedQuat);
			//pose.rotate = DecompressQuaternion(pose.compressedQuat); //decompress it back into normal quaternion

			bitstream.Read(pose.translate[0]);
			bitstream.Read(pose.translate[1]);
			bitstream.Read(pose.translate[2]);
			return bitstream;
		}

		// write to stream
		RakNet::BitStream& Write(RakNet::BitStream& bitstream, sSpatialPose& pose) const
		{
			bitstream.Write(pose.scale[0]);
			bitstream.Write(pose.scale[1]);
			bitstream.Write(pose.scale[2]);

			//compressed Quaternion
			//pose.compressedQuat = CompressQuaternion(pose.rotate);

			//write compressed quaternion to the bitstream
			bitstream.Write(pose.compressedQuat);

			//bitstream.Write(rotate[0]);
			//bitstream.Write(rotate[1]);
			//bitstream.Write(rotate[2]);
			bitstream.Write(pose.translate[0]);
			bitstream.Write(pose.translate[1]);
			bitstream.Write(pose.translate[2]);
			return bitstream;
		}

		char* CompressQuaternion(float quat[])
		{
			char compressed[3];
			//[-127,127]
			//starts [-1,1] for all 4 values of quat
			//a^2 + b^2 = 1
			//2a^2 = 1,  a^2 = 0.5, a=[sqrt(0.5)]
			//quat * const[1/[sqrt(0.5)]]

			//1/[sqrt(0.5)] = 1.4142

			//quat * 127

			return compressed;
		}

		float* DecompressQuaternion(char comp[])
		{
			//do the opposite of what was done in compressing

			float decompressed[4];
			return decompressed;
		}
	};

	// cRakNetManager
	//	Base class for RakNet peer management.
	class cRakNetManager abstract
	{
		// protected data
	protected:
		// peer
		//	Pointer to RakNet peer instance.
		RakNet::RakPeerInterface* peer;

		sSpatialPose poses[SET_GPRO_MAX_CLIENTS][SET_GPRO_MAX_POSES_PER_CLIENT];

		// protected methods
	protected:
		// cRakNetManager
		//	Default constructor.
		cRakNetManager();

		// ~cRakNetManager
		//	Destructor.
		virtual ~cRakNetManager();

		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);

		// WriteTimestamp
		//	Write timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& WriteTimestamp(RakNet::BitStream& bitstream);

		// ReadTimestamp
		//	Read timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTimestamp(RakNet::BitStream& bitstream, RakNet::Time& dtSendToReceive_out, RakNet::MessageID& msgID_out);

		// WriteTest
		//	Write test greeting message.
		//		param bitstream: packet data in bitstream
		//		param message: message string
		//		return: bitstream
		RakNet::BitStream& WriteTest(RakNet::BitStream& bitstream, char const message[]);

		RakNet::BitStream& WritePose(RakNet::BitStream& bitstream, sSpatialPose const pose);

		RakNet::BitStream& ReadPose(RakNet::BitStream& bitstream);

		// ReadTest
		//	Read test greeting message.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTest(RakNet::BitStream& bitstream);

		// public methods
	public:
		// MessageLoop
		//	Unpack and process packets.
		//		return: number of messages processed
		int MessageLoop();
	};

}


#endif	// __cplusplus
#endif	// !_GPRO_NET_RAKNET_HPP_