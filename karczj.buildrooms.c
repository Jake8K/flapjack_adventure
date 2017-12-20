/************************************************************************************************
 * Author: 	Jacob Karcz
 * Date: 	02/13/2017
 * Course:	CS344-400: Operating Systems	Assignment: Program 2, part 1: build rooms
 * File: 	karczj.buildrooms.c
 * Usage:	Compile this file and run it to create a rooms directory that part 2, adventure!
 * 		(karczj.adventure.c) can use to run a game
 * Description: This program sets up the rooms files for a simple rpg game where the user starts 
 *		out in a room and wants to find the end room. This program creates the rooms for it.
 * 		This particualr game is modeled after "The Marvelous Misadventures of Flapjack."
 * 		Enjoy. (or not, that's ok too)
 *************************************************************************************************/
//headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

//constants
#define NUM_ROOMS 7
#define MAX_CONNS 6
#define MIN_CONNS 3

/***************************
 * room struct declaration
 **************************/
struct Room {
	char* name;
	char* roomType;
	int connections;
	struct Room* connectedRooms[MAX_CONNS];
};

/**************************************************************************************
 * Function: 		void Room* buildRoom(name)
 * Description:		A constructor function for struct room, that takes a char* as
 * 			a parameter (the room's name) and returns a pointer to a Room
 *			pointer to the new room
 * Parameters:		A char array
 * Pre-Conditions: 	a valid char* name and a Room struct to hold the return variable
 * Post-Conditions: 	a new room is allocated and initialized with the name given
***************************************************************************************/
struct Room* newRoom(char* name) {
	//malloc struct room
//	struct Room* thisRoom = (struct Room*) malloc(sizeof(struct Room));
	struct Room* thisRoom = malloc(sizeof(struct Room));
	assert(thisRoom);

	//assign it
	thisRoom->name = name;
	thisRoom->connections = 0;
	thisRoom->roomType = 0;
	int i;
	for (i = 0; i < MAX_CONNS; i++) {
		thisRoom->connectedRooms[i] = NULL;
	}
	return thisRoom;
}
	

/*********************************************************************************************
 * Function: 		main() function
 * Description:		This is a long, ugly and convoluted main function that I didn't
 * 			get the chance to break up into nice, simple, workable, and portable
 * 			smaller funcitons.
 * 			It is "broken up" into 3 main sections:
 * 				-Section 1 is short, watch for it. It  creates an array of
 * 				  NUM_ROOMS (7) Room structs with names from the name array.
 * 				-Section 2 is the long and convoulted loop that creates the
 * 				 connections between the rooms, making sure they all have a
 * 				 maximum connections of MAX_CONNS (6) and a minimum of 3 
 * 				 (MIN_CONNS) and that none repeat or connect back to the same
 * 				 room and that all connections are mirrored between connector 
 * 				 and connectee.
 * 				-section 3 is the next tiny section at the end, it takes the array
 * 				 of finalized and connected Room structs (rooms) and prints each
 * 				 one to their unique file in a directory marked with the current
 * 				 current process id.
 * Parameters:		none	
 * Pre-Conditions: 	none
 * Post-Conditions: 	a new directory (karczj.rooms.pid#) containing the 7 different room file
 *************************************************************************************************/
int main() {


	//seed randomizer
	srand(time(NULL));
	
	//variables
	int isUnique;
	FILE* outFile;										 
	int randomInt;
	int i, j, k, h;


	//struct Room* roomTry = newRoom(room0);					 		 
	//printf("room name: %s \n", roomTry->name);

	//create an array of 10 room names							
	char* room1 = "Candied Island";
	char* room2 = "Bubbie's Stomach";
	char* room3 = "The Candy Barrel";
	char* room4 = "Stormalong";
	char* room5 = "Eight-Armed Willy's Ship";
	char* room6 = "Doctor Barber's";
	char* room7 = "Bubbie's Mouth";
	char* room8 = "Sea Monster Lair";
	char* room9 = "Bubbie's Back";
	char* room10 = "Captain K'nuckle's Raft";
	char* roomNames[] = {room1, room2, room3, room4, room5, room6, room7, room8, room9, room10}; 

	//create the rooms
	struct Room* rooms[NUM_ROOMS];
	
	for (i = 0; i < NUM_ROOMS; i++) {
		rooms[i] = NULL;
	}

	for (i = 0; i < NUM_ROOMS; i++) {
		randomInt = rand() % 10; //select a room name	
		rooms[i] = newRoom(roomNames[randomInt]);
		for (j = 0; j < i; j++) {
			if (strcmp(rooms[i]->name, rooms[j]->name) == 0) {
				free(rooms[i]);
				rooms[i] = NULL;
				i--;
				j = i; //break out!	
			}
		}
	}	
	/*//print the room names	
	printf("\n");
	for(i = 0; i < NUM_ROOMS; i++) {
		printf("room name: %s \n", rooms[i]->name);

	}*/
	//printf("\nRooms Created... Setting Connections...\n\t------\n\n");

	//connections
//	randomInt = rand() % MIN_CONNS + (MAX_CONNS - MIN_CONNS) + 1;
	k = 0; //k will hold the connected rooms array index
	for(i = 0; i < NUM_ROOMS; i++) {
		for (j = 0; j < MIN_CONNS - 1; j++) {
			//printf("=====\ni = %d  | j = %d | working with room %s , setting address %p\n=====\n", 
			//		i, j, rooms[i]->name, (void*)&rooms[i]->connectedRooms[j]);
			//get a room at random
			randomInt = rand() % NUM_ROOMS;
			//make sure it's connecting to different rooms that haven't been maxed out
			while (randomInt == i && rooms[i]->connections < MAX_CONNS 
					&& rooms[randomInt]->connections < MAX_CONNS) {
				randomInt = rand() % NUM_ROOMS;
			}

			//if the room doesn't have any connections yet (uninitialized array)
			if (rooms[i]->connections == 0) {
				//if the other room has connections check if this is one of 'em
				if (rooms[randomInt]->connections > 0) {
					isUnique = 0;
					for (k = 0; k < rooms[randomInt]->connections; k++) {
						if (rooms[randomInt]->connectedRooms[rooms[randomInt]->connections] == rooms[i]) {
							isUnique++; // not unique
						}
					}
					//it was unique, connect them
					if(isUnique == 0) {
						//printf("\tIF statement of 0 conns loop adding room
						//\" %s \" to %s\'s connections\n",rooms[randomInt]->name, rooms[i]->name);
						rooms[i]->connectedRooms[rooms[i]->connections] = rooms[randomInt];
						rooms[i]->connections++;
						rooms[randomInt]->connectedRooms[rooms[randomInt]->connections] = rooms[i];
						rooms[randomInt]->connections++;
					}
					//else j--;?????
				}
				//else if rooms[randomInt]->connections == MAX_COUNT j-- & break??
				else { // the other room didn't have any connections either, connnect them
					//printf("\tELSE statement of 0 conns loop adding room \" %s \" to %s\'s connections\n",
					//	       rooms[randomInt]->name, rooms[i]->name);
					rooms[i]->connectedRooms[rooms[i]->connections] = rooms[randomInt];
					rooms[i]->connections++;
					rooms[randomInt]->connectedRooms[rooms[randomInt]->connections] = rooms[i];
					rooms[randomInt]->connections++;
			
				}
			}
			else{
				// if rooms[randomInt]->name is not already in rooms[i]->connectedRooms[index], then  
				// 	rooms[i]->connectedRooms[j] = rooms[randomInt];
				isUnique = 0;
				//printf("\tentering trouble loop, room %s has %d connections\n",
				// rooms[i]->name, rooms[i]->connections);
				for (k = 0; k < rooms[i]->connections; k++) {
					if (rooms[i]->connectedRooms[k] != NULL) {
						//printf("\t\tinside trouble loop, checking if room \"%s\"(%p) == \"%s\"(%p)\n", 
						//rooms[randomInt]->name, (void*)&rooms[randomInt], 
						//rooms[i]->connectedRooms[k]->name, (void*)&rooms[i]->connectedRooms[k]);
						if (strcmp(rooms[randomInt]->name, rooms[i]->connectedRooms[k]->name) == 0) {
							//printf("\t\t\tthe if statement executed\n");
							isUnique++;
							//printf("\t\t\tthey were equal, incrementing isEqual to %d\n", isUnique);
						}
						//printf("\t\t\t\tdone with that rubbish!\n\t");
					}
				}
				//the for loop above found that these rooms aren't connected yet, so go for it
				if (isUnique == 0) {
					//printf("\t\t+ adding room \" %s \" to %s\'s connections\n", 
					//		rooms[randomInt]->name, rooms[i]->name);
					rooms[i]->connectedRooms[rooms[i]->connections] = rooms[randomInt];
					rooms[i]->connections++;
					rooms[randomInt]->connectedRooms[rooms[randomInt]->connections] = rooms[i];
					rooms[randomInt]->connections++;
	
					//printf("\t\t\t* added room \" %s \" successfully\n", rooms[i]->connectedRooms[j]->name);
	
				}
				else { //connection failed, try again
					//printf("\t\t\t- decrementing j");
					j--;
				}

			}
		}
	}
	//some room(s) may only have 2 connections, fix it here
	for(i = 0; i < NUM_ROOMS; i++) {
		if (rooms[i]->connections < 3) {
			//if it has less than 3 connections, add a connection
			randomInt = rand() % NUM_ROOMS;
			//make sure it's connecting to different rooms that haven't been maxed out
			while (randomInt == i && rooms[i]->connections < MAX_CONNS 
					&& rooms[randomInt]->connections < MAX_CONNS) {
				randomInt = rand() % NUM_ROOMS;
			}

			isUnique = 0;
			for(j = 0; j < rooms[i]->connections; j++) {
				if (strcmp(rooms[randomInt]->name, rooms[i]->connectedRooms[j]->name) == 0) {
					isUnique++;
				}
			}
			if(isUnique == 0) {
				//printf("\t\t+ adding room \" %s \" to %s\'s connections\n", 
				//rooms[randomInt]->name, rooms[i]->name);
				rooms[i]->connectedRooms[rooms[i]->connections] = rooms[randomInt];
				rooms[i]->connections++;
				rooms[randomInt]->connectedRooms[rooms[randomInt]->connections] = rooms[i];
				rooms[randomInt]->connections++;
			}
			else i--;
		}
	}

	//set room types
	char* start = "START_ROOM";
	char* mid = "MID_ROOM";
	char* end = "END_ROOM";
	for (i = 0; i < NUM_ROOMS; i++) {
		rooms[i]->roomType = mid;
	}
	//rooms are generated randomly, so i min & max are already randomized -> make them start and end
	rooms[0]->roomType = start;
	rooms[NUM_ROOMS - 1]->roomType = end;

	/*Loop over everything to make sure it worked	
	*******************************************************
	printf("\nConnections Made... Printing Rooms ...\n\t\t-------\n\n");	
	for(i = 0; i < NUM_ROOMS; i++) {
		printf("room name: %s \n", rooms[i]->name);
		printf("type: %s \n", rooms[i]->roomType);
		printf("total connections: %d \n", rooms[i]->connections);
		for (j = 0; j < rooms[i]->connections; j++) {
			if(rooms[i]->connectedRooms[j] != NULL) {
				printf("\tConnection %d: %s \n", j, rooms[i]->connectedRooms[j]->name);
			}
		}
		printf("\n");	
	}
	*******************************************************/



	//file storage directory
	int pid = getpid(); //returns type pid_t
	char directoryName[] = "karczj.rooms.";
	char directoryPath[20]; 
	memset(directoryPath, '\0', (sizeof(char) *20)); 
	//concatenate directory name & pid into a string (directoryPath) and open the directory
	snprintf(directoryPath, sizeof(directoryPath), "%s%d", directoryName, pid);
	mkdir(directoryPath, 0755); //S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//printf("created path %s & process ID %d, concatenated to directory %s...\n", directoryName, pid, directoryPath);

	//save files
	char filePath[48];
	for(i = 0; i < NUM_ROOMS; i++) {
		memset(filePath, '0', 48);

		//create file name w concatenation & open for writing
		snprintf(filePath, sizeof(filePath), "%s/%s.room", directoryPath, rooms[i]->name);
		outFile = fopen(filePath, "w");

		//write to the file
		fprintf(outFile, "ROOM NAME: %s\n", rooms[i]->name);
		for (j = 0; j < rooms[i]->connections; j++) {
			fprintf(outFile, "CONNECTION %d: %s\n", j+1, rooms[i]->connectedRooms[j]->name);
		}
		fprintf(outFile, "ROOM TYPE: %s", rooms[i]->roomType);

		fclose(outFile);
	}

	
	for (i = 0; i < NUM_ROOMS; i++) {
		free(rooms[i]);
	}

	return 0;
}

//  karczj.buildrooms.c
//  cs344_Adventure!
//
//  Created by Jacob Karcz on 2/8/17.
//  Copyright © 2017 Jacob Karcz. All rights reserved.
//
