#ifndef DATA
#define DATA
#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define INVALID -1		/*input is invalid if -1*/

#define COLLON 1		/*the lengh of the collon*/

#define AS_LEN 3		/*the lenght of the .as extention*/
#define OB_LEN 3		/*the lenght of the .ob extention*/
#define ENT_LEN 4		/*the lenght of the .ext extention*/
#define EXT_LEN 4		/*the lenght of the .ent extention*/

#define ABSOLUTE 0
#define EXTERNAL 1
#define RELOCATABLE 2

#define IMD_LEN	1		/*The lenght of the Immediate operand type*/
#define DIR_LEN 1		/*The lenght of the Direct operand type*/
#define FIX_LEN 2		/*The lenght of the FixedIndex operand type*/
#define DRR_LEN 1		/*The lenght of the DirectRegister operand type*/

#define REG_LEN 8		/*The number of Registers*/
#define OPR_LEN 16		/*The number of Codes*/
#define DTR_LEN 5		/*The number of DataTypes*/
#define WORD_LEN 14		/*The lenght of Word*/
#define BUFFER_SIZE 256	/*Used to get data from the input .as file*/
#define LINE_LEN 80		/*The lenght of a proper line*/

typedef enum boolean { False, True } boolean;			/*A boolean flag*/
typedef enum symbolTypes { Macro, Code, Data, External } symbolTypes;	/*The types of symbols we are going to use*/
typedef enum dataTypes { DefineLine, DataLine, StringLine, EntryLine, ExternLine } dataTypes;
typedef enum operandessTypes { Immediate, Direct, FixedIndex, DirectRegister } operandessTypes;
typedef enum codeTypes { Mov, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop } codeTypes;
/*The symbol table, it contains:
	char* name - the name of the symbol
	type type - the type of symbol 
	int value - the value either the code or the actual value 
	struct symbol* next - the next node */
typedef struct symbol {
	char* name;
	symbolTypes type;
	int value;
	struct symbol* next;
} symbol;

/*The entry table, it contains:
	char* name - the name of the entry
	struct entry* next - the next node */
typedef struct entry {
	char* name;
	struct entry* next;
} entry;

/*The parsed struct, it contains:
	char**data - the array of strings that were processed from the input
	int len - the number of strings */
typedef struct psr {
	char**data;
	int len;
} psr;

/*The string's first and last quotes for the string to be processed 
	int first - the first quote
	int last - the last quote */
typedef struct stringIndexes {
	int first;
	int last;
} stringIndexes;

/*The operand type of the code 
	int size - how many words does the operand takes
	operandessTypes codeType - the code type of the operand 
	symbol* firstSymbol - the symbol or the first one if we have a Fixed Index type operand
	symbol* secondSymbol - the second operand for the fixed type
	int number - if its a number and not a symbol
*/
typedef struct operand {
	int size;
	operandessTypes operandType;
	symbol* firstSymbol;
	symbol* secondSymbol;
	int number;
} operand;

/*The data word struct that contains:
	char* word - the special assembly word
	int dc - the index of the word
	struct dWord* next - the next word */
typedef struct dWord {
	char* word;
	int dc;
	struct dWord* next;
} dWord;

 /*The struct that contains all of the output files to print to*/
typedef struct files {
	FILE* objectFile;
	FILE* entryFile;
	FILE* externFile;
} files;

void	clearData		(symbol** symbolHead, dWord** dataHead, entry** entryList);
void	addDataWord		(dWord** dataHead, char word[], int dc);
void	addSymbol		(symbol** symbolHead, char name[], symbolTypes type, int value, int size);
void	addEntry		(entry** entryList, char name[]);
void	updateSymbols	(symbol** symbolHead, int *ic);
symbol* findSymbol		(symbol** symbolHead, char str[], int start, int end);
entry*	findEntry		(entry** entryList, char str[], int start, int end);
boolean stringCmp		(const char src[], char dst[], int start, int end);
#endif