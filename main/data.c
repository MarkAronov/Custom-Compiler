#include "data.h"
/***********************************************
**** This file contains functions that make ****
******* use of the data structs that the *******
************* project uses *********************
***********************************************/

/*
This method frees up the data of the symbols and data words
INPUT: symbol** symbolHead - the symbol table array list
INPUT: dWord** dataHead - the data word array list 
OUTPUT: NONE
*/
void clearData(symbol** symbolHead, dWord** dataHead, entry** entryList) {
	symbol* symbolTemp = *symbolHead, *syNext;
	dWord* dtTemp = *dataHead, * dtNext;
	entry* entryTemp = *entryList, * entryNext;
	if(symbolTemp) {
		while (symbolTemp) {
			syNext = symbolTemp->next;
			free(symbolTemp);
			symbolTemp = syNext;
		}
	}
	if(dtTemp) {
		while (dtTemp) {
			dtNext = dtTemp->next;
			free(dtTemp);
			dtTemp = dtNext;
		}
	}
	if (entryTemp) {
		while (entryTemp) {
			entryNext = entryTemp->next;
			free(entryTemp);
			entryTemp = entryNext;
		}
	}
}

/*
This method adds a new data word to the array list 
INPUT: dWord** dataHead - the data word array list
INPUT: char word[] - the string that contains the word
INPUT: int dc - the index of where it is placed
OUTPUT: NONE
*/
void addDataWord(dWord** dataHead, char word[], int dc) {
	dWord* ptr, * temp = (dWord*)malloc(sizeof(dWord));
	int size;
	if (!temp) return;
	size = (int)(strlen(word)) + 1;
	temp->word = (char*)malloc(size);
	if (!temp->word) return;
	temp->word[(int)(size - 1)] = '\0';
	strcpy(temp->word, word);
	temp->dc = dc;
	temp->next = NULL;
	if (!*dataHead) (*dataHead) = temp; 
	else {
		for (ptr = *dataHead; ptr->next; ptr = ptr->next);
		ptr->next = temp;
	}
}

/*
This method adds a new symbol to the symbol table
INPUT: symbol** symbolHead - the symbol table
INPUT: char name[] - the string that contains the word
INPUT: symbolTypes type - the type of symbol
INPUT: int value -  the numeric value of the symbol
INPUT: int size - the size of the name 
OUTPUT: NONE
*/
void addSymbol(symbol** symbolHead, char name[], symbolTypes type, int value, int size){
	symbol *ptr,*temp = (symbol*)malloc(sizeof(symbol));
	int allocSize = size + 1;
	if (!temp) return;
	temp->name = (char*)malloc(allocSize);
	if (!temp->name) return;
	temp->name[(int)(size)] = '\0';
	strncpy(temp->name, name, size);
	temp->type = type;
	temp->value = value;
	temp->next = NULL;
	if (!*symbolHead) (*symbolHead) = temp;
	else {
		for (ptr = *symbolHead; ptr->next; ptr = ptr->next);
		ptr->next = temp;
	}
}

/*
This method adds a new entry to the entry list
INPUT: entry** entryList - the entry array list
INPUT: char name[] - the string that contains the word
OUTPUT: NONE
*/
void addEntry(entry** entryList, char name[]) {
	entry* ptr, * temp = (entry*)malloc(sizeof(entry));
	int size = strlen(name) + 1;
	if (!temp) return;
	temp->name = (char*)malloc(size);
	if (!temp->name) return;
	temp->name[(int)(size - 1)] = '\0';
	strcpy(temp->name, name);
	temp->next = NULL;
	if (!*entryList) (*entryList) = temp;
	else {
		for (ptr = *entryList; ptr->next; ptr = ptr->next);
		ptr->next = temp;
	}
}

/*
This method updates the indexes of the symbols in the symbol table
INPUT: symbol** symbolHead - the symbol table
INPUT: int* ic - the lenght of the ic to sum with 
OUTPUT: NONE
*/
void updateSymbols(symbol** symbolHead, int* ic) {
	symbol* ptr;
	if (*symbolHead) {
		for (ptr = *symbolHead; ptr; ptr = ptr->next) {
			if (ptr->type == Code) ptr->value += 100;
			else if (ptr->type == Data) ptr->value += 100 + *ic;
		}
	}
}

/*
This method finds a symbols in the symbol table
INPUT: symbol** symbolHead - the symbol table
INPUT: char str[] - the name of the symbol to find
INPUT: int start - the starting index
INPUT: int end - the ending index
OUTPUT: symbol* - the symbol found else NULL
*/
symbol* findSymbol(symbol** symbolHead, char str[], int start, int end) {
	symbol* ptr;
	for (ptr = *symbolHead; ptr; ptr = ptr->next) {
		if (stringCmp(ptr->name, str, start, end)) return ptr;
	}
	return NULL;
}

/*
This method finds an entry in the entry list
INPUT: entry** entryList - the symbol table
INPUT: char str[] - the name of the symbol to find
INPUT: int start - the starting index
INPUT: int end - the ending index
OUTPUT: entry* - the entry found else NULL
*/
entry* findEntry(entry** entryList, char str[], int start, int end) {
	entry* ptr;
	for (ptr = *entryList; ptr; ptr = ptr->next) {
		if (stringCmp(ptr->name, str, start, end)) return ptr;
	}
	return NULL;
}

/*
This method compares between a fixed lenght string and string bounded between two indexes
INPUT: const char src[] - the fixed lenght string 
INPUT: char dst[] - the string that is bounded between two indexes
INPUT: int start - the first index of the bounded string 
INPUT: int end - the last index
OUTPUT: boolean - True if the two strings are equal else False
*/
boolean stringCmp(const char src[], char dst[], int start, int end) {
	int i, j, len = end - start;
	for (j =0, i = start; i < end; j++, i++) {
		if (src[j] != dst[i]) return False;
	}
	if (len != strlen(src)) return False;
	return True;
}