#pragma once

#include "gpro-net/gpro-net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

class Battleship
{
	public:

		Battleship(){}

		~Battleship(){}

		/*
		void PrintBoards(gpro_battleship board1, gpro_battleship board2, int playerIndex);

	//takes in input(something like B4) and translates into the correct spot in gpro_battleship, input will be in correct format
		Vec2 ProcessInput(std::string input);

	//check if input is within board bounds
		bool IsInputValid(std::string input);

		bool isSpaceOpen(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length);

		void PopulateBoardWithShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length, gpro_battleship_flag shipType);

	//0 is left, 1 is up, 2 is right, 3 is down
		bool CheckAndCreateShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation);
	//sets up a single ship, rerun with all types of ships
		void SetupShip(gpro_battleship& board1, gpro_battleship& board2, int playerIndex, gpro_battleship_flag shipType);

		void SetupBoard(gpro_battleship& board1, gpro_battleship& board2, int playerIndex);
		*/

		
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
						if (gpro_flag_check(boardToUse[startLocation.x + i][startLocation.y], gpro_battleship_flag::gpro_battleship_ship))//check if ship already in spot
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
			Vec2 loc(0, 0);

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
};
