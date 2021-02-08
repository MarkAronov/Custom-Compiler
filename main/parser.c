#include "parser.h"

/***********************************************
**** This file contains functions that  ********
*** convert the input buffer into a string *****
***********************************************/

/*
This method frees up the data of the parsed line
INPUT: psr* parsed - the parsed line to free
OUTPUT: NONE
*/
void freePsr(psr* parsed) {
	int i;
	if (parsed) {
		for (i = 0; i < parsed->len; i++) free(parsed->data[i]);
		free(parsed->data);
		free(parsed);
	}
}

/*
This method resizes the string array to fit a new string
INPUT: char** data - the array of strings that cp 
INPUT: int size - new size to resize to
OUTPUT: char** - the newly resized data
*/
char** resizeData(char** data, int size) {
	char** temp = (char**)realloc(data, size * sizeof(char*));
	if (!temp) {
		printf("Error: Failed Allocation.");
		exit(-1);
	}
	return temp;
}
/*
This method resizes the string to fit a comma 
INPUT: char* str - the string to concate the comma with 
OUTPUT: char* - the newly resized data
*/
char* resizeString(char str[]) {
	char* temp = (char*)realloc(str, strlen(str) + 2);
	if (!temp) {
		printf("Error: Failed Allocation.");
		exit(-1);
	}
	strcat(temp, ",");
	return temp;
}

/*
This method makes a new string for array strings
INPUT: char** data - the array of strings that cp
INPUT: char buffer[] - the buffer to copy the string data from
INPUT: int size - size of the new string
INPUT: int numOfStrings - the number of strings in the array
INPUT: int firstIndex - the first index to copy from 
OUTPUT: char** - the newly resized data
*/
void makeString(char** data, char buffer[], int size, int numOfStrings, int firstIndex) {
	int alloc = size - 1;
	data[numOfStrings] = (char*)malloc(size * sizeof(char));
	if (!data[numOfStrings]) {
		printf("Error: Failed Allocation.");
		exit(-1);
	}
	/*copy the data*/
	strncpy(data[numOfStrings], (buffer + firstIndex), alloc);
	if (data[numOfStrings]) { /*make sure that there is an end to the string*/
		data[numOfStrings][size - 1] = '\0';
	}
}

/*
This method gets the fisrt and last indexes for the string parameter in a string data type 
INPUT: char buffer[] - the buffer to copy the string data from
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int i - the index where the buffer stopped, and where we will continue
INPUT: int firstStrIndex - the first index of the string parameter
INPUT: int lastStrIndex - the last index of the string parameter 
INPUT: boolean firstPhase - flag, used to check if the errors will be printed only in the first phase
OUTPUT: stringIndexes* - the struct that contains the first and last indexes
*/
stringIndexes* getStringIndexes(char buffer[], char fileName[], int line, int i, int firstStrIndex, int lastStrIndex, boolean firstPhase) {
	stringIndexes* temp = NULL;
	/*first we check for the first double quotes to show up then we check the last double qoutes to show up*/
	for (; i < strlen(buffer); i++) {
		if (firstStrIndex == INVALID && buffer[i] == '\"') firstStrIndex = i;
		else if ((lastStrIndex == INVALID || lastStrIndex < i) && buffer[i] == '\"' && i != firstStrIndex) lastStrIndex = i;
	}
	if (firstStrIndex == INVALID && lastStrIndex == INVALID && firstPhase) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid string: invalid string parameter\n", fileName + firstNameIndex(fileName), line);
		}
		return temp;
	}
	else if (firstStrIndex == INVALID && firstPhase) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid string:"
					" missing beginning quotes (\")\n", fileName + firstNameIndex(fileName), line);
		}
		return temp;
	}
	else if (lastStrIndex == INVALID) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid string:"
					" missing ending quotes (\")\n", fileName + firstNameIndex(fileName), line);
		}
		return temp;
	}
	/*if we found the two indexes we send them back in a struct*/
	temp = (stringIndexes*)malloc(sizeof(stringIndexes));
	if (!temp) return NULL;
	temp->first = firstStrIndex;
	temp->last = lastStrIndex;
	return temp;
}

/*	
This is the main processing function, it gets command lines and then proceses them 
INPUT:	char buffer[] - the buffer of the input 
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: boolean firstPhase - flag, used to check if the errors will be printed only in the first phase
OUTPUT: psr - the pasred array of strings used to process the line
*/
psr* parser(char buffer[], char fileName[], int line, boolean firstPhase) {
	/*the return struct for the process*/
	psr* ps = NULL;

	stringIndexes* ptr = NULL;
	/*the data that is extracted from the input*/
	char** data = NULL;
	/*flags and indexes*/
	int i = 0, numOfStrings = 0, /*the number of words in the array*/
		counter = 0, /*lenght of a word to be allocated and added to the data*/
		firstIndex = INVALID, /*index - the first index of the word*/
		firstStrIndex = INVALID, /*index - the first index of the string*/
		lastStrIndex = INVALID; /*index - the last index of the string*/
	boolean commaSkip = False, /*flag - used to check if we hit a ',' if so then more to a new word*/
			stringHandle = False, /*flag - used to check if the line is a string*/
			tripped = False, /*flag - used to check if the first whitespace is caught*/
			hitsemicolon = False, /*flag - used to check if a semicolon was found*/
			readyToCut = False; /*flag - used to check if there's no whitespace so that we can make a new word */
	/*loop untill the end of the input*/
	for (i = 0; i < strlen(buffer) && !hitsemicolon; i++) {
		readyToCut = True;
		hitsemicolon = (buffer[i] == ';');
		/*while there's no whitespace nor ',' add +1 to the counter*/
		if (data &&! strcmp(data[numOfStrings - 1], ".string") && !stringHandle) {
			/*we use a function to get the first and last indexs of the string parameter*/
			ptr = getStringIndexes(buffer,fileName, line, i, firstStrIndex, lastStrIndex, firstPhase);
			if (!ptr) return NULL;
			else {
				i = ptr->last + 1;
				data = resizeData(data, numOfStrings + 1);
				makeString(data, buffer, ptr->last - ptr->first + 2, numOfStrings, ptr->first);
				/*up the number of words by one*/
				numOfStrings++;
				/*clear the tripped and counter flags for the next word*/
				tripped = False;
				counter = 0;
			}
			free(ptr);
		}
		if (buffer[i] != ' ' && buffer[i] != '	' && buffer[i] != '\n' && buffer[i] != '\r' && !commaSkip && !hitsemicolon){
			readyToCut = False; /* if we hit a letter we do not create a string*/
			if (!tripped) firstIndex = i; /*if tripped add i to the first index */
			tripped = True;
			counter++;
			if (buffer[i] == ',') {
				/*now check if the comma is connected to a word*/
				if (buffer[i-1] != ' ' && buffer[i-1] != '	') {
					commaSkip = True;
					readyToCut = True;
				}
				/*if not a word we check if the previous word does not contain a comma */
				else if (data && data[numOfStrings - 1][(int)(strlen(data[numOfStrings - 1]) - 1)] != ',') {
					data[numOfStrings - 1] = resizeString(data[numOfStrings - 1]);
					tripped = commaSkip = False;
					counter = 0;
				}
			}
		}
		/*if we hit the first whitespace do the following*/
		if (tripped && readyToCut) {
			/*resize of the array of strings by 1 each time*/
			data = resizeData(data, numOfStrings + 1);
			/*allocate the memory for the word with the exact size*/
			makeString(data, buffer, counter + 1, numOfStrings, firstIndex);
			/*up the number of words by one*/
			numOfStrings++;
			/*clear the tripped and counter flags for the next word*/
			tripped = commaSkip = False;
			counter = 0;
		}
	}
	if (numOfStrings) {
		ps = (psr*)malloc(sizeof(psr));
		if (!ps) return NULL;
		ps->data = data;
		ps->len = numOfStrings;
	}
	return ps;
}