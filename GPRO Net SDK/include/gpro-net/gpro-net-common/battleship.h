#pragma once

#include "gpro-net/gpro-net.h"
#include "gpro-net/gpro-net-common/gpro-net-console.h"
#include "gpro-net/gpro-net-common/gpro-net-gamestate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

		
void PrintBoards(gpro_battleship board1, gpro_battleship board2, int playerIndex);
		
//takes in input(something like B4) and translates into the correct spot in gpro_battleship, input will be in correct format
Vec2 ProcessInput(std::string input);
		

//check if input is within board bounds
bool IsInputValid(std::string input);
		

//Checks if there is space open in the given direction(within board bounds and not going through a ship)
bool isSpaceOpen(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length);
		
//Fills board with the specified ship in the direction
void PopulateBoardWithShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation, int length, gpro_battleship_flag shipType);
		

//0 is left, 1 is up, 2 is right, 3 is down
bool CheckAndCreateShip(gpro_battleship& boardToUse, int direction, Vec2 startLocation);
		
//sets up a single ship, rerun with all types of ships
void SetupShip(gpro_battleship& board1, gpro_battleship& board2, int playerIndex, gpro_battleship_flag shipType);
		
//Do the setup phase for the player
void SetupBoard(gpro_battleship& board1, gpro_battleship& board2, int playerIndex);


//Do the attack phase for the player
void AttackPhase(gpro_battleship& board1, gpro_battleship& board2, int playerIndex);
		
		

