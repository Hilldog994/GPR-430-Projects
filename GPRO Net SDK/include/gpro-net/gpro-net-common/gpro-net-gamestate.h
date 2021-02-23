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

	gpro-net-gamestate.h
	Header for common networking mini-game states.
*/
#ifndef _GPRO_NET_GAMESTATE_H_
#define _GPRO_NET_GAMESTATE_H_

#include <iostream>

#define gpro_flag_raise(value, flag)	(value | flag)	// value with flag raised
#define gpro_flag_toggle(value, flag)	(value ^ flag)	// value with flag toggled
#define gpro_flag_check(value, flag)	(value & flag)	// value contains part of flag
#define gpro_flag_lower(value, flag)	(value & ~flag)	// value with flag lowered
#define gpro_flag_equals(value, flag)	(gpro_flag_check(value, flag) == flag)	// value contains entire flag


#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


typedef unsigned char
	gpro_battleship[10][10];	// battleship board (one player)


/*
* BS:
		[10]  1    2    3    4    5    6    7    8    9    10
			 __________________________________________________
		 A	|    |    |    |    |    |    |    |    |	 |    |                [10]
			|____|____|____|____|____|____|____|____|____|____|
		 B	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 C	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 D	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 E	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 F	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 G	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 H	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 I	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
		 J	|    |    |    |    |    |    |    |    |    |    |
			|____|____|____|____|____|____|____|____|____|____|
*/

typedef enum gpro_battleship_flag
{
	gpro_battleship_open,			// we have not attacked this space
	gpro_battleship_miss = 0x01,	// we have attacked this space and missed
	gpro_battleship_hit = 0x02,		// we have attacked this space and hit
	gpro_battleship_damage = 0x04,	// our ship is damaged; their attack hit
	gpro_battleship_ship_p2 = 0x08,	// space contains patrol boat (2 spaces)
	gpro_battleship_ship_s3 = 0x10,	// space contains submarine (3 spaces)
	gpro_battleship_ship_d3 = 0x20,	// space contains destroyer (3 spaces)
	gpro_battleship_ship_b4 = 0x40,	// space contains battleship (4 spaces)
	gpro_battleship_ship_c5 = 0x80,	// space contains carrier (5 spaces)
	gpro_battleship_ship = gpro_battleship_ship_p2 | gpro_battleship_ship_s3 |
		gpro_battleship_ship_d3 | gpro_battleship_ship_b4 | gpro_battleship_ship_c5, // our ships
	gpro_battleship_attack_rec = gpro_battleship_miss | gpro_battleship_hit,	// our records of attack
	gpro_battleship_defend_rec = gpro_battleship_damage | gpro_battleship_ship,	// our records of defense
} gpro_battleship_flag;

inline void gpro_battleship_reset(gpro_battleship gs)
{
	int* itr = (int*)gs, * const end = itr + sizeof(gpro_battleship) / sizeof(int);
	while (itr < end)
		*(itr++) = 0;
}

//displays battleship board, pass in board and whether or not the board to print is yours(show ships) or enemy's(no ships shown)
//Enemy board is on Top, player board is on bottom
inline void gpro_battleship_display_board(gpro_battleship board, bool yourBoard)
{
	gpro_consoleColor bg = gpro_consoleColor_a;

	if (!yourBoard)//print enemy board
	{
		bg = gpro_consoleColor_b;
		gpro_consoleSetColor(gpro_consoleColor_g, bg);
		printf("    ");//spacing for A-J spot
		for (int i = 1; i < 11; i++)//print 1-10
		{
			printf(" %i ", i);
		}
		printf("\n");
		for (int i = 0; i < 10; i++) //rows(A-J) 
		{
			//https://stackoverflow.com/questions/44999629/convert-letters-into-numbers-a-1-b-2-c 'A' + i just increments the character
			gpro_consoleSetColor(gpro_consoleColor_g, bg);
			printf(" %c |", 'A'+i);
			for (int j = 0; j < 10; j++) //columns(1-10)
			{
				if(gpro_flag_check(board[i][j],gpro_battleship_flag::gpro_battleship_open)) 
				{
					gpro_consoleSetColor(gpro_consoleColor_g, bg);
					printf("|_|");
						
				}
				else if(gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_hit))
				{
					if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_p2))//hit patrol ship
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|P|");
					}
					else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_d3))//hit destroyer
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|D|");
					}
					else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_s3))//hit submarine
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|S|");
					}
					else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_b4))//hit battleship
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|B|");
					}
					else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_c5))//hit carrier
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|C|");
					}
					else
					{
						gpro_consoleSetColor(gpro_consoleColor_r, bg);
						printf("|X|");
					}
					
						
				}
				else if(gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_miss))
				{
					gpro_consoleSetColor(gpro_consoleColor_white, bg);
					printf("|O|");
						
				}
				else //if board has ship or something else unique to player board, make it look like an open space
				{
					gpro_consoleSetColor(gpro_consoleColor_g, bg);
					printf("|_|");
						
				}
				
			}
			printf("\n");
		}
	}
	else //print your board with ships, etc.
	{
		bg = gpro_consoleColor_blue;
		gpro_consoleSetColor(gpro_consoleColor_g, bg);
		printf("    ");//spacing for A-J spot
		for (int i = 1; i < 11; i++)//print 1-10
		{
			printf(" %i ", i);
		}
		printf("\n");
		for (int i = 0; i < 10; i++) //rows(A-J) 
		{
			//https://stackoverflow.com/questions/44999629/convert-letters-into-numbers-a-1-b-2-c 'A' + i just increments the character
			gpro_consoleSetColor(gpro_consoleColor_g, bg);
			printf(" %c |", 'A' + i);
			for (int j = 0; j < 10; j++) //columns(1-10)
			{
				if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_open))
				{
					gpro_consoleSetColor(gpro_consoleColor_g, bg);
					printf("|_|");

				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_miss))
				{
					gpro_consoleSetColor(gpro_consoleColor_white, bg);
					printf("|O|");

				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_p2))//hit patrol ship
				{
					gpro_consoleSetColor(gpro_consoleColor_grey, bg);
					printf("|P|");
				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_d3))//hit patrol ship
				{
					gpro_consoleSetColor(gpro_consoleColor_grey, bg);
					printf("|D|");
				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_s3))//hit patrol ship
				{
					gpro_consoleSetColor(gpro_consoleColor_grey, bg);
					printf("|S|");
				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_b4))//hit patrol ship
				{
					gpro_consoleSetColor(gpro_consoleColor_grey, bg);
					printf("|B|");
				}
				else if (gpro_flag_check(board[i][j], gpro_battleship_flag::gpro_battleship_ship_c5))//hit patrol ship
				{
					gpro_consoleSetColor(gpro_consoleColor_grey, bg);
					printf("|C|");
				}
				else //if board has ship or something else unique to player board, make it look like an open space
				{
					gpro_consoleSetColor(gpro_consoleColor_g, bg);
					printf("|_|");

				}

			}
			printf("\n");
		}
	}

}


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !_GPRO_NET_GAMESTATE_H_