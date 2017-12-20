/************************************************************************************************
 * Author: 	Jacob Karcz
 * Date: 	02/13/2017
 * Course:	CS344-400: Operating Systems	Assignment: Program 2, part 2: Adventure!
 * File: 	karczj.adventure.c 
 * Usage:	First compile karczj.buildrooms.c and Run karczj.buildrooms to
 * 		create the necessary game files
 * Description: This program is a simple game simulator that recreates randomly arranged
 * 		and connected "rooms." Once the array of room structs is re-created from
 * 		from the files (created by *buildrooms) the player starts in one location
 * 		and is given options of visiting connected "rooms." alternatively, the
 * 		player may enter "time" to get a print out of the current time and date, 
 * 		as well as a timestamped currentTime.txt file in the working rooms directory
 * 		courtesy of an evil thread. The user makes their way from the start-room, 
 * 		through the mid-room(s) and onto the end-room. Once the user reaches the end, 
 * 		the number of steps taken as well as the list of rooms visited at each step
 * 		are displayed on the screen.
 * 		This particualr game is modeled after "The Marvelous Misadventures of Flapjack."
 * 		Enjoy. (or not, that's ok too)
 *************************************************************************************************/
//headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <fcntl.h>

#define NUM_ROOMS 7
#define MAX_CONNS 6


/***************************
 * room struct declaration
 **************************/
struct Room {
	char* name;
	char* roomType;
	int connections;
	//struct Room* connectedRooms[MAX_CONNS];
	char* connectedRooms[MAX_CONNS];
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
struct Room* buildRoom(char* name) {
	//malloc struct room
	struct Room* thisRoom = malloc(sizeof(struct Room));
	assert(thisRoom);

	//assign its member vars
	int i;
	thisRoom->name = name;
	thisRoom->connections = 0;
	thisRoom->roomType = NULL;
	for (i = 0; i < MAX_CONNS; i++) {
		thisRoom->connectedRooms[i] = NULL;
	}
	return thisRoom;
}

/**************************************************************************************
 * Function: 		void* timeKeeper(void* directoryPath)
 * Description:		A function that is called in a thread and prints the current local
 * 			time to the screen and to a currentTime.txt file
 * Parameters:		a directory path	
 * Pre-Conditions: 	valid directory
 * Post-Conditions: 	a time stamped file, currentTime.txt
 ***************************************************************************************/
void *timeKeeper() { // (void* directoryPath){ //dir path == filePath bellow
	//vars
	time_t curTime = time(0); //= time(&curTime);
	struct tm* timeNow = localtime(&curTime);
	char timeFilePath[48];
		memset(timeFilePath, '\0', sizeof(timeFilePath));
	char timeBuffer[100];
		memset(timeBuffer, '\0', sizeof(timeBuffer));
	FILE* outFile;
	int success = 0;
	//char *path = directoryPath;


	//Get the Time & print it to screen
	//timeNow = time(&curTime);
	timeNow = localtime(&curTime);
	strftime(timeBuffer, 100, "%I:%M%P, %A, %B %d, %Y", timeNow);
	printf("%s", timeBuffer);

	
	/***************************************************************************
	//create filePath & open timeStamp file
	snprintf(timeFilePath, sizeof(timeFilePath), "%s/currentTime.txt", filePath); //dir path == filePath
	snprintf(timeFilePath, sizeof(timeFilePath), "%s/currentTime.txt", path); //dir path == filePath
	//snprintf(timeFilePath, sizeof(timeFilePath), "./currentTime.txt"); //dir path == filePath
	//printf("\nfile path variable created: %s\n", timeFilePath);

	//write to the file
	outFile = fopen(timeFilePath, "w");
	fprintf(outFile, "current time: %s\n", timeBuffer);
	printf("\nfile written\n");
	fclose(outFile);
	printf("\nfile closed\n");
	*****************************************************************************/

	//reset buffers (even though filePath prob won't change so it doesn't need it)
	memset(timeBuffer, '\0', sizeof(timeBuffer));
	memset(timeFilePath, '\0', sizeof(timeFilePath));

	pthread_exit(NULL);

	//return 
	//return (void *)success;
}


/******************************************************************************************
 * Function: 		main() function
 * Description:		This is a long, ugly and convoluted main function that I didn't
 * 			get the chance to break up into nice, simple, workable, and portable
 * 			smaller funcitons.
 * 			It is "broken up" into 2 main sections:
 * 				-Section 1 locates the most recent rooms directory, extracts
 * 				 the room files within and turns them into room structs
 * 				-Section 2 has the actual game play where it takes the array
 * 				 of Room structs from section 1 and uses them to run the game.
 * 				 if "time" is entered a thread is created to print and display
 * 				 the current time.
 * Parameters:		none	
 * Pre-Conditions: 	a valid rooms directory (run karczj.buildrooms)
 * Post-Conditions: 	a time stamped file, currentTime.txt, in the current rooms directory
 *******************************************************************************************/
int main() {
	//variables
	char c;
	char prefix[] = "karczj.rooms.";
	char* fileName; 
	char filePath[48];
		memset(filePath, '\0', sizeof(filePath));
	char thisFilePath[100];
		memset(filePath, '\0', sizeof(filePath));
	char* directoryPath = NULL; 
	// these are for the awesome RIP section that won't work	
	//char identifier1[48];
	//	memset(identifier1, '\0', sizeof(identifier1));
	//char identifier2[48];
	//	memset(identifier2, '\0', sizeof(identifier2));
	//char identifier3[48];
	//i	memset(identifier3, '\0', sizeof(identifier3)); 
	char roomName[49];
		memset(roomName, '\0', sizeof(roomName));
	char buffer[200];
		memset(buffer, '\0', sizeof(buffer));
	char* token = NULL;
	char* thisLine;


	DIR* dir;	// ./
	DIR* fileDir; 	// ./karczj.rooms.pid
	struct dirent *dirFiles; //directory data
	FILE* inputFile;

	struct dirent* directory;
	struct dirent* inFiles;
	struct stat fd;
	struct stat latest;
	struct Room* rooms[NUM_ROOMS];
	struct Room* newRoom;

	ssize_t Fbytes = 0; // # bytes read from file

	int aTime = 0;
	int i, j, k, h;
	int count;

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
	char* roomTypes[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

	//initialize rooms array to NULL
	for(i = 0; i < NUM_ROOMS; i++) {
		rooms[i] = NULL;
	}

	//get the rooms
	dir = opendir("./");
	if (dir != NULL) {
		//search the directory
		while (directory = readdir(dir)) {
			if(strstr(directory->d_name, prefix) != NULL) {
				//get directory file info
				stat(directory->d_name, &fd);
				//hold onto the most recent one
				if( aTime < fd.st_mtime) {
				       	aTime = fd.st_mtime;
					directoryPath = directory->d_name;
				}
				//print them all out
				//printf("%s: %s\n", directory->d_name, ctime(&fd.st_mtime));
			}
		}
		//print out the chosen one
		//printf("directory: %s @ %d \n", directoryPath, &time);

		//enter the most recent directory
//		chdir(directoryPath);
//		fileDir = opendir(".");
		snprintf(filePath, sizeof(filePath), "./%s", directoryPath);
		fileDir = opendir(filePath);	
		if (fileDir == NULL) {
			printf("Error: failed to open directory %s, errno = %d\n", directoryPath, errno);
			exit(1);
		}
		//read the files in it
		i = 0;
		j = 0;
		while (inFiles = readdir(fileDir)) {
			//process each actual file
			if(strcmp(inFiles->d_name,".") !=0 && strcmp(inFiles->d_name,"..") !=0 && inFiles->d_name != NULL) {

				//get file path and open file
				fileName = inFiles->d_name;
				snprintf(thisFilePath, sizeof(thisFilePath), "%s/%s", filePath, fileName);
				inputFile = fopen(thisFilePath, "r");
				if (inputFile == NULL) {
					printf("Error: failed to open file %s, errno = %d\n", fileName, errno);
					exit(1);
				}
				//printf("file: %s\n", fileName);
				
				//create & initialize a room
				//newRoom = buildRoom();
				//This section of code had a lot of promise, I think it was awesome and simple but for 
				//some reason (I'm thinking scope) I couldnt save the strings to the Room structs.
				/************************************RIP**********************************************
				//process the room file –> this should work, but it doesn't actually save the strings!
				while (fscanf(inputFile, "%s %s %s", identifier1, identifier2, identifier3) != EOF){
					if(strcmp(identifier1, "ROOM") == 0) {
						if(strcmp(identifier2, "NAME:") == 0) {
							//rooms[i] = buildRoom(identifier3);
							//newRoom->name = identifier3;
							strcpy(token, identifier3);
							rooms[i] = buildRoom(token);
							printf("room name: %s | %s\n", identifier3, rooms[i]->name);
						}
						else if (strcmp(identifier2, "TYPE:") == 0) {
							//rooms[i]->roomType = identifier3;
							strcpy(rooms[i]->roomType, identifier3);
							printf("room type: %s | %s\n", identifier3, rooms[i]->roomType);
						}
					}
					else if(strcmp(identifier1, "CONNECTION") == 0) {
						//rooms[i]->connectedRooms[j] = identifier3;
						strcpy(rooms[i]->connectedRooms[j], identifier3);
						rooms[i]->connections++;
						printf("Connection %d: %s: %s\n", j, identifier3, rooms[i]->connectedRooms[j]);
						j++;
					}
					memset(identifier1, '\0', sizeof(identifier1));
					memset(identifier2, '\0', sizeof(identifier2));
					memset(identifier3, '\0', sizeof(identifier3));

				}
				//rooms[i] = newRoom;
				printf("Room named %s of type %s with %d connections added to array\n", 
					rooms[i]->name, rooms[i]->roomType, rooms[i]->connections);
				i++;
				printf("i = %d || j = %d\n", i, j);
				memset(thisFilePath, '\0', sizeof(thisFilePath));
				memset(identifier1, '\0', sizeof(identifier1));
				memset(identifier2, '\0', sizeof(identifier2));
				memset(identifier3, '\0', sizeof(identifier3));
				j=0;
				*****************************************RIP************************************/
				memset(buffer, '\0', sizeof(buffer));
				count = 0;
				c = 0;
				while (c != EOF) {
					c = fgetc(inputFile);
					if(c == '\n') {
						count++;
					}
				}
				count--; // the number of connections

				memset(buffer, '\0', sizeof(buffer));
				//get the room name
				fseek(inputFile, 11, SEEK_SET);
				fgets(buffer, sizeof(buffer), inputFile);
				for(h = 0; h < 10; h++) {
					if(strncmp(buffer, roomNames[h], 10) == 0) {
						rooms[i] = buildRoom(roomNames[h]);
					}
				}

				//printf("this room is named: %s & the buffer is %s <––––––––\n", rooms[i]->name, buffer);	
				//get the connections			
				for(j = 0; j < count; j++) { //Why won't MAX_CONNS (6) work here!!!!
				//while(!feof(inputFile)) {
				//printf("------>I'm in the connections forLoop!\n");
        				memset(buffer, '\0', sizeof(buffer));
        				//position is at beg of 2nd line move to position after (CONNECTION #: )
        				fseek(inputFile, 14, SEEK_CUR);
					//get the actual connection
					fgets(buffer, sizeof(buffer), inputFile);
					//remove the newline from the end & compare strings
					if((token = strchr(buffer, '\n')) != NULL) {
					    *token = '\0';
					}
					//printf("connection: %s\n", buffer);
 					for(k = 0; k < 10; k++) {
		    				//printf("\tconnection: %s\n", roomNames[k]);
	 			           	//printf("room[%d]: %s\n",i, rooms[i]->name);
	    	    				if(strcmp(buffer, roomNames[k]) == 0) {
							rooms[i]->connectedRooms[rooms[i]->connections] = roomNames[k];
							rooms[i]->connections++;
							//printf("\t\tconnection: %s\n", rooms[i]->connectedRooms[j]);
	    					}
		    				
					}
    					//printf("numLines: %d\n", numLines);
            			}

	    			//printf("Num Lines: %d\n", gameRooms[i].capConnections);

				memset(buffer, '\0', sizeof(buffer));
 				fseek(inputFile, 11, SEEK_CUR);
				fgets(buffer, sizeof(buffer), inputFile);
				if(strcmp(buffer, "START_ROOM") == 0) {
				    rooms[i]->roomType = roomTypes[0];
				} 
				else if (strcmp(buffer, "MID_ROOM") == 0) {
				    rooms[i]->roomType = roomTypes[1];
				} 
				else if(strcmp(buffer, "END_ROOM") == 0) {
				    rooms[i]->roomType = roomTypes[2];
				}
				//printf("room type: %s\n", buffer);  
          			fclose(inputFile);
				i++;
				//printf("\n\ni = %d\n\n", i);
			
			}
		}
		closedir(fileDir);

	}
	//fclose(inputFile);
	closedir(dir);



	/******************************
	 *  gamePlay!
	 *****************************/
	int winning = 1; //1 == false
	int currentRoom = 0;
	int nextRoom = 0;
	int checkTime = 0;
	int response = 1; //1 == bad | 0 == good | 3 == time 
	int steps = 0;
	char* newRoomName;
	char* visited[100];
		for (i = 0; i < 100; i++) {
			visited[i] = NULL;
		}
	time_t curTime = time(0); //= time(&curTime);
	struct tm* timeNow = localtime(&curTime);
	char timeBuffer[100];
		memset(timeBuffer, '\0', sizeof(timeBuffer));
	char timeFilePath[48];
		memset(timeFilePath, '\0', sizeof(timeFilePath));
	FILE* outFile;
	pthread_t timeThread;
	pthread_mutex_t lock;
	int threadStat;
	
        //start the multithreading
	//pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
	//pthread_cond_t cond;
	pthread_mutex_init(&lock, NULL);
	//threadStat = pthread_create(&timeThread, NULL, timeKeeper, (void*)filePath);
	//pthread_cond_init(&cond, NULL);
	
	//printf("i got this far\n");
	for(i = 0; i < NUM_ROOMS; i++) {
		//printf("\nroom name: %s, room type: %s\n", rooms[i]->name, rooms[i]->roomType);
		if (strcmp(rooms[i]->roomType, roomTypes[0]) == 0) { //typeArr[0] == start
			currentRoom = i;
		}
	}
	visited[0] = rooms[currentRoom]->name;
	steps++;

	do {
		//the UI (interface)
		if (checkTime == 0) {
			printf("\nCURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", rooms[currentRoom]->name);		
			for (j = 0; j < (rooms[currentRoom]->connections - 1); j++) { 
				printf("%s, ", rooms[currentRoom]->connectedRooms[j]);
			}
			//print the last room with a . & \n
			printf("%s.\nWHERE TO? >", rooms[currentRoom]->connectedRooms[rooms[currentRoom]->connections - 1]);
		}
		else {
			printf("\nWHERE TO? >");
		}
	
		//get user input
		memset(buffer, '\0', sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
			//parse out the \n
		strtok(buffer, "\n");
		printf("\n");

		//compare the response |
		//---------------------
		response = 1;

		//check time
		if(strcmp(buffer, "time") == 0) {

			/*this part writes the temp file (bc casting args is evil)
			 ****************************************************/
			//timeNow = time(&curTime);
			timeNow = localtime(&curTime);
		        strftime(timeBuffer, 100, "%I:%M%P, %A, %B %d, %Y", timeNow);
			//printf("%s", timeBuffer); //<---- thread does that now

			//create filePath & open timeStamp file (bc passing char* to thread == null)
			snprintf(timeFilePath, sizeof(timeFilePath), "%s/currentTime.txt", filePath);
			outFile = fopen(timeFilePath, "w");
			fprintf(outFile, "current time: %s\n", timeBuffer);
			fclose(outFile);

			//reset buffers (even though filePath prob won't change so it doesn't need it)
			memset(timeBuffer, '\0', sizeof(timeBuffer));
			memset(timeFilePath, '\0', sizeof(timeFilePath)); 
			
			/*this part uses a thread to print the time to screen
			 ***************************************************/
			pthread_mutex_unlock(&lock); //unlock the mutex
			threadStat = pthread_create(&timeThread, NULL, timeKeeper, &filePath); //thread time
			pthread_join(timeThread, NULL);
			pthread_mutex_lock(&lock); //lock down this thread

			//while(!cond) {
			//	pthread_cond_wait(&cond, &lock); //wait for cond to be true
			//	timeKeeper(filePath);
			//}

			//timeKeeper(filePath); //this is now a relic
			checkTime = 1;
		}	

		//move to a new room?
		else {
			checkTime = 0;
			for (i = 0; i < rooms[currentRoom]->connections; i++) {
				//if (strncmp(buffer, rooms[currentRoom]->connectedRooms[i], 10) == 0) { //not specific enough
				//printf("\nchecking room %s...\n", rooms[currentRoom]->connectedRooms[i]);
				if (strcmp(buffer, rooms[currentRoom]->connectedRooms[i]) == 0) {
					//response was valid
					response = 0;
					newRoomName = rooms[currentRoom]->connectedRooms[i];
					//visited[steps] = rooms[currentRoom]->name;
					visited[steps] = newRoomName;
					steps++;
					//move to next room
					for (j = 0; j < NUM_ROOMS; j++) {
						if(strncmp(rooms[j]->name, newRoomName, 10) == 0) {
							currentRoom = j;
							//response = 1;
						}
					}
				}
			}
			if (response == 1) {
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
		}		
	
	} while (strcmp(rooms[currentRoom]->roomType, "END_ROOM") != 0);// (winning == 1);

	//Game over, you won!
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	for(j = 0; j < (steps); j++) {
	    printf("%s\n", visited[j]);
        }
	printf("\n\n");

	//exit thread
	//pthread_kill(timeThread, NULL); //SIGKILL);

	//free the heap!
	for (i = 0; i < NUM_ROOMS; i++) {
		free(rooms[i]);
    }
	return 0;
}


//  karczj.adventure.c
//  cs344_Adventure!
//
//  Created by Jacob Karcz on 2/8/17.
//  Copyright © 2017 Jacob Karcz. All rights reserved.
//


