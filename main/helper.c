#include"helper.h"
/***********************************************
**** This file contains functions that help ****
*********** the handlers to work ***************  
***********************************************/

/*
This method finds the first index of the file name because the user could give the 
program a full lenght name the includes the directory with the file name
INPUT: char str[] - the string to check 
OUTPUT: int - the position of the file name else 0 
*/
int	firstNameIndex(char str[]) {
	int i, lastSlash = 0;
	for (i = 0; i < strlen(str); i++) {
		/*in linux we have / as a dir symbol and in windows we have \*/
		if ((str[i] == '/' || str[i] == '\\' )&& lastSlash < i) lastSlash = i;
	}
	return (lastSlash)? lastSlash + 1 : 0;
}


/*
This method converts a decimal number into a binary number
INPUT: char str[] - the string to store the binary word into
INPUT: int number - the number to convert
INPUT: int start - the first index to store the data into
INPUT: int lenght - the lenght of the binary word
OUTPUT: NONE
*/
void dataToBin(char str[], int number, int start,  int lenght){
	int i, j;
	for (i = lenght - 1, j = start; i >= 0; i--, j++){
		str[j] = ((number >> i)  & 1)? '1' : '0'; 
	}
}

/*
This method converts a binary string into a special machine string 
INPUT: char src[] - the source string that contains the binary data
INPUT: char dst[] - the destination string to store the special converted string
OUTPUT: NONE
*/
void binToSpe(char src[], char dst[]) {
	int i, j ;
	for (i = 0, j = 0; i < WORD_LEN; j++, i = i+2) {
		if		(src[i] == '0' && src[i + 1] == '0') dst[j] = '*';
		else if (src[i] == '0' && src[i + 1] == '1') dst[j] = '#';
		else if (src[i] == '1' && src[i + 1] == '0') dst[j] = '%';
		else if (src[i] == '1' && src[i + 1] == '1') dst[j] = '!';
	}
}

/*
This method checks if the string between two indexes is a datatype
INPUT: char src[] - the source string
INPUT: int start - the first index
INPUT: int end - the last index
OUTPUT: dataTypes - the type of the datatype else -1 
*/
dataTypes isDataType(char str[], int start, int end) {
	static const char* datatypes[] = { ".define", ".data", ".string" , ".entry", ".extern" };
	int i;
	for (i = 0; i < DTR_LEN; i++) {
		if (stringCmp(datatypes[i], str, start, end)) return i;
	}
	return INVALID;
}

/*
This method checks if the string between two indexes is a code
INPUT: char src[] - the source string
INPUT: int start - the first index
INPUT: int end - the last index
OUTPUT: codeTypes - the type of the code else -1
*/
codeTypes isCode(char str[], int start, int end){
	static const char* codes[] = { "mov", "cmp", "add", "sub", "not", "clr", "lea",
								   "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
	int i;
	for (i = 0; i < OPR_LEN; i++) {
		if (stringCmp(codes[i], str, start, end)) return i;
	}
	return INVALID;
}

/*
This method checks if the string between two indexes is a register
INPUT: char src[] - the source string
INPUT: int start - the first index
INPUT: int end - the last index
OUTPUT: int - the number of the register else -1
*/
int isReg(char str[], int start, int end) {
	static const char* registers[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };
	int i;
	for (i = 0; i < REG_LEN; i++) {
		if (stringCmp(registers[i], str, start, end)) return i;
	}
	return INVALID;
}

/*
This method finds a label in the entry list and if they match the label will be printed in the entry list
INPUT: entry** entryList - the label list
INPUT: char str[] - the name of the label to find
OUTPUT: True if label is signed to Entry else false
*/
void printEntryList(files* outputFiles, symbol** symbolHead, entry** entryList) {
	entry* entryPtr;
	symbol* symbolPtr;
	for (symbolPtr = *symbolHead; symbolPtr; symbolPtr = symbolPtr->next) {
		for (entryPtr = *entryList; entryPtr; entryPtr = entryPtr->next) {
			if (!strcmp(symbolPtr->name, entryPtr->name)) {
				fprintf(outputFiles->entryFile, "%s	%04d\n", symbolPtr->name, symbolPtr->value);
			}
		}
	}
}

/*
This method checks if the string between two indexes is a real number
INPUT: char str[] - the string that contains the number 
INPUT: int start -  the first index to check 
INPUT: int end - the index to stop 
OUTPUT: boolean - True if real number else False 
*/
boolean checkNumber(char str[], int start, int end) {
	int i, num = 0, pm = 0;
	if (str[start] == '+' || str[start] == '-') pm++;  /*check if there's one plus or minus*/
	for (i = start; i < end; i++)  if (str[i] >= '0' && str[i] <= '9') num++; /*if its a number add it to the  counter*/
	return (num + pm != end - start)? False : True; /*return true if the lenght of the string is equal to the count of numbers*/
}

/*
This method checks if the string between two indexes is a true label 
INPUT: char str[] - the string that contains the label   
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int start -  the first index to check 
INPUT: int end - the index to stop 
INPUT: boolean printError - flag, used to not print error, needed for the handelCode fucntion
OUTPUT: boolean - True if real label else False
*/
boolean checkLabel(char str[], char fileName[], int line, int start, int end, boolean printError) {
	int i;
	if (strlen(str) > 31) { /*it's invalid for a label to longer than 31 bits so we need to check if the label isn't that long*/
		if (printError) {
			printf("#Error in file %s at line %d- Invalid label lenght\n", fileName + firstNameIndex(fileName), line);
		}
		return False;
	} /*it's invalid for a label to a register or a code so we need to check if the label isn't those*/
	else if (isReg(str, start, end) != INVALID || isCode(str, start, end) != INVALID) {
		if (printError) {
			printf("#Error in file %s at line %d- Invalid label name: "
					"name should not be a register or an operation \n", fileName + firstNameIndex(fileName), line);
		}
		return False;
	} /*it's invalid for a label to not an alphabet so we need to check if the label starts with one*/
	else if ((str[start] < 'A') || (str[start] > 'Z' && str[start] < 'a') || (str[start] > 'z')) {
		if (printError) {
			printf("#Error in file %s at line %d- Invalid label name: "
					"name needs to start with alphabetic letters\n", fileName+ firstNameIndex(fileName), line);
		}
		return False;
	} /*a valid lable should only contain numeric or alphabet letters*/
	for (i = start + 1; i < end; i++) {
		if ((str[i] < '0') ||(str[i] > '9' && str[i] < 'A') ||(str[i] > 'Z' && str[i] < 'a') || (str[i] > 'z')) {
			if (printError) {
				printf("#Error in file %s at line %d- Invalid label name: "
						"name should only conntain numeric or alphabet chars\n", fileName + firstNameIndex(fileName), line);
			}
			return False;
		}
	}
	return True;
}

/*
This method checks if the file is empty 
INPUT: FILE* file - the file to check if it's empty 
OUTPUT: boolean - True if file empty else False
*/
boolean checkIfFileEmpty(FILE* file) {
	int size;
	if (file) {
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file); /*we reset the pointer so that we will be able to use a non empty file*/
		return(size == 0) ? True : False;
	}
	return True;
}

/*
This method checks if the operad type fit the code type
INPUT: operand* src - the source operand 
INPUT: operand* dst - the destination operand
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: codeTypes type - the type of code to check with the operands
INPUT: boolean printError - flag, used to not print error, needed for the handelCode fucntion
OUTPUT: boolean - True if the operands fit the code type else False
*/
boolean checkCode(operand* src, operand* dst, char fileName[], int line, codeTypes type, boolean printError) {
	boolean dstFlag = False, srcFlag = False;
	/*	mov			 add		  sub*/
	if (type ==	Mov || type == Add || type == Sub) {
		if (src->operandType != Immediate &&
			src->operandType != Direct &&
			src->operandType != FixedIndex &&
			src->operandType != DirectRegister) srcFlag = True; /* if the opernads dont fit the code we will print an error */
		if (dst->operandType != Direct &&						 /* at the end of the function and return False*/
			dst->operandType != FixedIndex &&
			dst->operandType != DirectRegister) dstFlag = True;
	}/*		 cmp*/
	else if (type == Cmp) {
		if (src->operandType != Immediate &&
			src->operandType != Direct &&
			src->operandType != FixedIndex &&
			src->operandType != DirectRegister) srcFlag = True;
		if (dst->operandType != Immediate &&
			dst->operandType != Direct &&
			dst->operandType != FixedIndex &&
			dst->operandType != DirectRegister) dstFlag = True;

	}/*	     not		  clr		   inc			dec			 red*/
	else if (type == Not || type == Clr || type == Inc || type == Dec || type == Red) {
		if (dst->operandType != Direct &&
			dst->operandType != FixedIndex &&
			dst->operandType != DirectRegister) dstFlag = True;
	}/*		lea*/
	else if (type == Lea) {
		if (src->operandType != Direct &&
			src->operandType != FixedIndex) srcFlag = True;
		if (dst->operandType != Direct &&
			dst->operandType != FixedIndex &&
			dst->operandType != DirectRegister) dstFlag = True;
	}/*		 jmp		  bne			jsr*/
	else if (type == Jmp || type == Bne || type == Jsr) {
		if (dst->operandType != Direct &&
			dst->operandType != DirectRegister) dstFlag = True;
	}/*		 prn*/
	else if (type == Prn) {
		if (dst->operandType != Immediate &&
			dst->operandType != Direct &&
			dst->operandType != FixedIndex &&
			dst->operandType != DirectRegister) dstFlag = True;
	}
	if (printError) {
		if (srcFlag) {
			printf("#Error in file %s at line %d- Invalid code:"
					" illegal src operand type\n", fileName + firstNameIndex(fileName), line);
		}
		if (dstFlag) {
			printf("#Error in file %s at line %d- Invalid code:"
					" illegal destination operand type\n", fileName + firstNameIndex(fileName), line);
		}
	}
	return (!dstFlag && !srcFlag);
}

/*
This method gets the operand size and type 
INPUT: char str[] - the string that contains the operand
INPUT: files* outputFiles - a struct of output files used to print to 
INPUT: int* ic - the instruction counter used to print the code into the file in the second phase
INPUT: symbol** symbolHead - the symbol table to check if there's a label to use
INPUT: entry** entryList - the entry list to check if there's a label to use
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int end - the index to stop
INPUT: boolean firstPhase - flag, used to skip the symbol checking 
OUTPUT: operand* - The datatype that will contain the data of the operand 
*/
operand* getOperandData(char str[], files* outputFiles, int* ic, symbol** symbolHead, char fileName[], int line, int end, boolean firstPhase) {
	int i, bracketStart = INVALID;
	operand* operandTemp = (operand*)malloc(sizeof(operand));
	symbol* symbolTemp;
	if (!operandTemp) return NULL;
	/*check if the string at the beginning contains the number sign*/
	/*if there is a number sign either check that the rest of the string is the number or a label*/
	if	(str[0] == '#' && checkNumber(str, 1, end)) {
		/*if it's a number add the Immediate code type and add no symbol */
		/*because we will use the number variable */
		operandTemp->size = IMD_LEN;
		operandTemp->operandType = Immediate;
		operandTemp->firstSymbol = NULL;
		operandTemp->secondSymbol = NULL; 
		operandTemp->number = atoi(str + 1);
	}
	else if (str[0] == '#' && checkLabel(str, fileName, line, 1, end, False)) {
		/*if it's a label add the Immediate code type and add the label to the first symbol variable */
		/*other then that we need to check if the label is the right one and also if it has*/
		/*any connections to extern and entry datatypes*/
		symbolTemp = findSymbol(symbolHead, str, 1, end);
		if (firstPhase || (symbolTemp && (symbolTemp->type == Macro || symbolTemp->type == External))) {
			if (!firstPhase && symbolTemp->type == External) {
				fprintf(outputFiles->externFile, "%s	%04d\n", symbolTemp->name, IMD_LEN + (*ic));
			}
			operandTemp->size = IMD_LEN;
			operandTemp->operandType = Immediate;
			operandTemp->firstSymbol = symbolTemp;
			operandTemp->secondSymbol = NULL;
			operandTemp->number = INVALID;
		}
		else {
			printf("#Error in file %s at line %d- Invalid operation:"
					" lable -> %s isn't a macro\n", fileName + firstNameIndex(fileName), line, str);
			free(operandTemp);
			return NULL;
		}
	}
	/*if the string is a label go here */
	else if (checkLabel(str, fileName, line, 0, end, False)){
		/*if it's a label add the Direct code type and add the label to the first symbol variable*/
		/*other then that we need to check if the label is the right one and also if it has*/
		/*any connections to extern and entry datatypes*/
		symbolTemp = findSymbol(symbolHead, str, 0, end);
		if (firstPhase || (symbolTemp && (symbolTemp->type == Data || symbolTemp->type == External || symbolTemp->type == Code))) {
			if (!firstPhase && symbolTemp->type == External) {
				/*if the data type is external print it to the ext file*/
				fprintf(outputFiles->externFile, "%s	%04d\n", symbolTemp->name, DIR_LEN + (*ic));
			}
			operandTemp->size = DIR_LEN;
			operandTemp->operandType = Direct;
			operandTemp->firstSymbol = symbolTemp;
			operandTemp->secondSymbol = NULL;
			operandTemp->number = INVALID;
		}
		else {
			printf("#Error in file %s at line %d- Invalid operation:"
					" lable -> %s isn't a data\n", fileName + firstNameIndex(fileName), line, str);	
			free(operandTemp);
			return NULL;
		}
	}
	/*if the string contains at the end of it a square bracket it means it might be the third operand type */
	else if (str[end - 1] == ']') {
		/*so we will have to check if it has a beginning bracket*/
		for (i = 0; i < end && bracketStart == INVALID; i++) {
			if (str[i] == '[') bracketStart = i;
		}
		if (bracketStart == INVALID) {
			printf("#Error in file %s at line %d- Invalid operation:"
					" missing beginning square bracket ([)\n", fileName + firstNameIndex(fileName), line);
			free(operandTemp);
			return NULL;
		}
		/*now we will need to check if string before the brackets is a label*/
		else if (checkLabel(str, fileName, line, 0, bracketStart, False)){
			/*if it's a label add the FixedIndex code type and add the label to the first symbol variable*/
			/*other then that we need to check if the label is the right one and also if it has*/
			/*any connections to extern and entry datatypes*/
			symbolTemp = findSymbol(symbolHead, str, 0, bracketStart);
			if (!firstPhase && (!symbolTemp || (symbolTemp->type != Data && symbolTemp->type != External))) {
				printf("#Error in file %s at line %d- Invalid operation:"
						" lable -> %s isn't data or extern\n", fileName + firstNameIndex(fileName), line, str);
				free(operandTemp);
				return NULL;
			}
			/*now we check the string between the brackets*/
			if (checkLabel(str, fileName, line, bracketStart + 1, end - 1, False)) {
				/*if it's a label add the FixedIndex code type and add the label to the first symbol variable*/
				/*other then that we need to check if the label is the right one and also if it has*/
				/*any connections to extern and entry datatypes*/
				if (!firstPhase && symbolTemp->type == External) {
					/*if the data type is external print it to the ext file*/
					fprintf(outputFiles->externFile, "%s	%04d\n", symbolTemp->name, (FIX_LEN - 1) + (*ic));
				}
				operandTemp->firstSymbol = symbolTemp;
				symbolTemp = findSymbol(symbolHead, str, bracketStart + 1, end - 1);
				if (firstPhase || (symbolTemp && (symbolTemp->type == Macro || symbolTemp->type == External))) {
					if (!firstPhase && symbolTemp->type == External) {
						/*if the data type is external print it to the ext file*/
						fprintf(outputFiles->externFile, "%s	%04d\n", symbolTemp->name, FIX_LEN + (*ic));
					}
					operandTemp->size = FIX_LEN;
					operandTemp->operandType = FixedIndex;
					operandTemp->secondSymbol = symbolTemp;
					operandTemp->number = INVALID;
				}
				else {
					printf("#Error in file %s at line %d- Invalid operation: lable isn't a macro\n", fileName + firstNameIndex(fileName), line);
					free(operandTemp);
					return NULL;
				}
			}
			/*if not a label we check if the string between the brackets is a number */
			else if (checkNumber(str, bracketStart + 1, end - 1)) {
				/*if it's a number add the FixedIndex code type and add no symbol */
				/*because we will use the number variable */
				operandTemp->size = FIX_LEN;
				operandTemp->operandType = FixedIndex;
				operandTemp->firstSymbol = symbolTemp;
				operandTemp->secondSymbol = NULL;
				operandTemp->number = atoi(str + bracketStart +1); /*move the pointer of the string to the first index of the number*/
			}
		}
		else {
			printf("#Error in file %s at line %d- Invalid operation:"
					" invalid lable in between the square brackets\n", fileName + firstNameIndex(fileName), line);
			free(operandTemp);
			return NULL;
		}
	}
	/*if its not a fixed lenght operand we finally check if it's a register type operand*/
	else if (isReg(str, 0, end) != INVALID) {
	/*if it's a register add the DirectRegister code type and add no symbol */
	/*because we will use the number of the register*/
		operandTemp->size = DRR_LEN;
		operandTemp->operandType = DirectRegister;
		operandTemp->firstSymbol = NULL;
		operandTemp->secondSymbol = NULL;
		operandTemp->number = isReg(str, 0, end);
	}
	else {
		free(operandTemp);
		return NULL;
	}
	return operandTemp;
}