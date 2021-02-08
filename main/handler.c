#include"handler.h"

/***********************************************
***** This file contains line handlers *********
*********** and the file printers **************
***********************************************/

/*
This method handles a macro/define line
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: entry** entryList - the entrylist to check or add entries
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
OUTPUT: boolean - True if the line is a correct macro else False
*/
boolean handMacro(psr* ps, symbol** symbolHead, entry** entryList, char fileName[], int line) {
	/*the number of words that a macro line contains are only 4*/
	if (ps->len == 4) {
		/*the third string of the line is a equals sign*/
		if (!strcmp(ps->data[2], "=")) {
			/*the second string is the name of the macro, a label*/
			if (!checkLabel(ps->data[1], fileName, line, 0, (int)(strlen(ps->data[1])), True)) {
				/*if not a real label name print an error*/
				printf("#Error in file %s at line %d- Invalid define: invalid name\n", fileName + firstNameIndex(fileName), line);
				return False;
			}
			else if (findSymbol(symbolHead, ps->data[1], 0, (int)(strlen(ps->data[1]))) ||
					 findEntry(entryList, ps->data[1], 0, (int)(strlen(ps->data[1])))) {
				/*if its an already used label we print an error */
				printf("#Error in file %s at line %d- Invalid label name:"
						" name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
				return False;
			}
			/*the fourth and last string is the number of the macro */
			else if (!checkNumber(ps->data[3], 0, (int)(strlen(ps->data[3])))){
				printf("#Error in file %s at line %d- Invalid define: invalid number\n", fileName + firstNameIndex(fileName), line);
				return False;
			}
			else {
				/*add the macro to the symbol table with Macro symbol*/
				addSymbol(symbolHead, ps->data[1], Macro, (int)(atoi(ps->data[3])), (int)(strlen(ps->data[1])));
				return True;
			}
		}
		else {
			printf("#Error in file %s at line %d- Invalid define: missing equals\n", fileName + firstNameIndex(fileName), line);
			return False;
		}
	}
	else if (ps->len > 4) {
		printf("#Error in file %s at line %d- Invalid define: unnecessary parameters\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	else {
		printf("#Error in file %s at line %d- Invalid define: few parameters\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
}

/*
This method handles a label
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
OUTPUT: boolean - True if the label is correct else False
*/
boolean handLabel(psr* ps, symbol** symbolHead, char fileName[], int line) {
	/*check if the label is a valid one*/
	if (!checkLabel(ps->data[0], fileName, line, 0, (int)(strlen(ps->data[0]) - 1), True)) {
		return False;
	}
	/*check if there's not a used label*/
	if (findSymbol(symbolHead, ps->data[0], 0, (int)(strlen(ps->data[0]) - 1))) { /* -1 is added to remove the colloms */
		printf(	"#Error in file %s at line %d- Invalid label name:"
				" name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	/*if the lable is in the same line as a macro, it's an error and should not be processed*/
	else if (ps->len >= 2 && (isDataType(ps->data[1], 0, (int)(strlen(ps->data[1]))) == DefineLine)) {
		printf("#Error in file %s at line %d- Invalid define:"
				" labels are not allowed\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	/*if the label is the only the one in the line add it to the list */
	else if(ps->len == 1){
		printf("#Error in file %s at line %d- Invalid line:"
				" only label line is not allowed\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	return True;
}

/*
This method handles an extern line
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: entry** entryList - the entrylist to check or add entries
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: boolean lableLine - flag, used to tell if the line contains a label or not
OUTPUT: boolean - True if the extern line is correct else False
*/
boolean handExtern(psr* ps, symbol** symbolHead, entry** entryList, char fileName[], int line, boolean lableLine) {
	/*if the line contains a label line do not include it to the symbol table and print a warning*/
	if (lableLine && checkLabel(ps->data[0], fileName, line, 0, (int)(strlen(ps->data[0]) - 1), True)) { /*-1 is added to remove the colloms*/
		/*check if there's already a symbol*/
		if (findSymbol(symbolHead, ps->data[0], 0, (int)(strlen(ps->data[0]) - 1))) {
			printf("#Error in file %s at line %d- Invalid label name:"
					" name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
			return False;
		}
		else {
			printf("#Warning in file %s at line %d- Invalid extern:"
					" you should not use a lable in an extern line\n", fileName + firstNameIndex(fileName), line);
		}
	}
	/*an extern line should only contain 2 words, the .entry and the label*/
	if (ps->len > 2 + lableLine) {
		printf("#Error in file %s at line %d- Invalid extern: too many parameters\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	else if (ps->len == 1 + lableLine) {
		printf("#Error in file %s at line %d- Invalid extern: missing parameter\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	/*check if the label is not included in symbol table*/
	else if (checkLabel(ps->data[1 + lableLine], fileName, line, 0, (int)(strlen(ps->data[1 + lableLine])), True)) {
		/*if it's already defined print an error*/
		if (findSymbol(symbolHead, ps->data[1 + lableLine], 0, (int)(strlen(ps->data[1 + lableLine]))) ||
			findEntry(entryList, ps->data[1 + lableLine], 0, (int)(strlen(ps->data[1 + lableLine])))) {
			printf("#Error in file %s at line %d- Invalid extern name:"
					"name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
			return False;
		}
		else {
			/*else add it to the symbol list*/
			addSymbol(symbolHead, ps->data[1 + lableLine], External, 0, (int)(strlen(ps->data[1 + lableLine])));
		}
	}
	return True;
}

/*
This method handles an entry line
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: entry** entryList - the entrylist to check or add entries
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: boolean lableLine - flag, used to tell if the line contains a label or not
OUTPUT: boolean - True if the entry line is correct else False
*/
boolean handEntry(psr* ps, symbol** symbolHead, entry** entryList,  char fileName[], int line, boolean lableLine) {
	symbol* tempSymbol;
	/*if the line contains a label line do not include it to the symbol table and print a warning*/
	if (lableLine && checkLabel(ps->data[0], fileName, line, 0, (int)(strlen(ps->data[0]) - 1), True)) {
		/*check if there's already a symbol*/
		if (findSymbol(symbolHead, ps->data[0], 0, (int)(strlen(ps->data[0]) - 1))) {
			printf("#Error in file %s at line %d- Invalid label name:"
					" name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
			return False;
		}
		else {
			printf("#Warning in file %s at line %d- Invalid entry:"
					"you should not use a lable in an entry line\n", fileName + firstNameIndex(fileName), line);
		}
	}
	/*an extern line should only contain 2 words, the .extern and the label*/
	if (ps->len > 2 + lableLine) {
		printf("#Error in file %s at line %d- Invalid entry: too many parameters\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	else if (ps->len == 1 + lableLine) {
		printf("#Error in file %s at line %d- Invalid entry: missing parameter\n", fileName + firstNameIndex(fileName), line);
		return False;
	}
	else if (checkLabel(ps->data[1 + lableLine], fileName, line, 0, (int)(strlen(ps->data[1 + lableLine])), False)) {
		/*check if the label is code or data else print error*/
		tempSymbol = findSymbol(symbolHead, ps->data[1 + lableLine], 0, (int)(strlen(ps->data[1 + lableLine])));
		if ((tempSymbol && tempSymbol->type != Code && tempSymbol->type != Data) ||
			findEntry(entryList, ps->data[1 + lableLine], 0, (int)(strlen(ps->data[1 + lableLine])))) {
			printf("#Error in file %s at line %d- Invalid label name:"
					" name should not be a used lable\n", fileName + firstNameIndex(fileName), line);
			return False;
		}
		else {
			/*else add it to the entry list*/
			addEntry(entryList, ps->data[1 + lableLine]);
		}
	}
	return True;
}

/*
This method handles a data line
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: dWord** dataHead - the data word array lists
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int* dc - the index of the data lines
INPUT: boolean lableLine - flag, used to tell if the line contains a label or not
OUTPUT: boolean - True if the data line is correct else False
*/
boolean handData(psr* ps, symbol** symbolHead, dWord** dataHead, char fileName[], int line, int* dc, boolean lableLine) {
	/*set the data strings for converting the data lines for the compiled file*/
	char* word = (char*)malloc(WORD_LEN + 1), * finished = (char*)malloc(WORD_LEN / 2 + 1);
	symbol* temp;
	int i, startDc = *dc; /*start data counter used to add the first dc to the symbol list*/
	boolean withComma = False; /*flag - used to tell if the word contains a comma so that the convertion will exclude that*/

	if (!word || !finished) return False;
	finished[WORD_LEN / 2] = word[WORD_LEN] = '\0';
	/*a correct data line should contain more than 1 or 2 words depending if there's a label*/
	if (ps->len == 1  + lableLine) {
		printf("#Error in file %s at line %d- Invalid data: no parameters were found\n", fileName + firstNameIndex(fileName), line);
		free(word);
		free(finished);
		return False;
	}
	/*now loop to process the numbers/macros of the data array*/
	for (i = 1 + lableLine; i < ps->len; i++, (*dc)++, withComma = False) {
		/*if the string is just a comma print an error of too much commas*/
		if (!strcmp(ps->data[i], ",")) {
			printf("#Error in file %s at line %d- Invalid data: unnecessary commas (,)\n", fileName + firstNameIndex(fileName), line);
			free(word);
			free(finished);
			return False;
		}
		/*while we're not in the end check if at every end in the word*/
		else if (i < ps->len - 1) {
			if (ps->data[i][strlen(ps->data[i]) - 1] != ',') {
				printf("#Error in file %s at line %d- Invalid data: missing comma/s (,)\n", fileName + firstNameIndex(fileName), line);
				free(word);
				free(finished);
				return False;
			}
			else withComma = True;
		}
		/*now check if there's a macro in the line*/
		if ((temp = findSymbol(symbolHead, ps->data[i], 0, (int)(strlen(ps->data[i]) - withComma)))) {
			if (temp->type == Macro) {
				/*if the symbol is a macro print the value of the macro into all of the 14 bits */
				dataToBin(word, temp->value, 0, WORD_LEN);
				/*then convert the string into a special 4 bit language*/
				binToSpe(word, finished);
				/*add it to the data arraylist to be printed at the end of the compiled object file*/
				addDataWord(dataHead, finished, *dc);
			}
			else { /*if the symbol is not a macro print an error*/
				printf("#Error in file %s at line %d- Invalid data: lable is not a macro\n", fileName + firstNameIndex(fileName), line);
				free(word);
				free(finished); 
				return False;
			}
		}
		/*if word isnt a symbol it could be a number*/
		else if (checkNumber(ps->data[i], 0, (int)(strlen(ps->data[i]) - withComma))) {
			/*if the word is a number print the value of the number into all of the 14 bits */
			dataToBin(word, atoi(ps->data[i]), 0, WORD_LEN);
			/*then convert the string into a special 4 bit language*/
			binToSpe(word, finished);
			/*add it to the data arraylist to be printed at the end of the compiled object file*/
			addDataWord(dataHead, finished, *dc);
		}
		else { 
			printf("#Error in file %s at line %d- Invalid data: is not a macro or a number\n", fileName + firstNameIndex(fileName), line);
			free(word);
			free(finished);
			return False; 
		}
	}
	/*if the data line contains a label add the data line to the symbol table*/
	if (lableLine) addSymbol(symbolHead, ps->data[0], Data, startDc, (int)(strlen(ps->data[0]) - COLLON));
	free(word);
	free(finished);
	return True;
}

/*
This method handles a string line
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: dWord** dataHead - the data word array lists
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int* dc - the index of the data lines
INPUT: boolean lableLine - flag, used to tell if the line contains a label or not
OUTPUT: boolean - True if the string line is correct else False
*/
boolean handString(psr* ps, symbol** symbolHead, dWord** dataHead, char fileName[], int line, int* dc, boolean lableLine) {
	/*set the data strings for converting the data lines for the compiled file*/
	char* word = (char*)malloc(WORD_LEN + 1), * finished = (char*)malloc(WORD_LEN / 2 + 1);
	int i, startDc = *dc;  /*start data counter used to add the first dc to the symbol list*/
	
	if (!word || !finished) return False;
	finished[WORD_LEN / 2] = word[WORD_LEN] = '\0';
	/*a correct data line should only contain 2 or 3 words depending if there's a label*/
	if (ps->len > 2 + lableLine) {
		printf("#Error in file %s at line %d- Invalid string: too many parameters\n", fileName + firstNameIndex(fileName), line);
		free(word);
		free(finished); 
		return False;
	}
	else if (ps->len == 1 + lableLine) {
		printf("#Error in file %s at line %d- Invalid string: missing string parameter\n", fileName + firstNameIndex(fileName), line);
		free(word);
		free(finished); 
		return False;
	}
	else if (strlen(ps->data[1 + lableLine]) == 2) {
		printf("#Error in file %s at line %d- Invalid string: an empty string ("")\n", fileName + firstNameIndex(fileName), line);
		free(word);
		free(finished);
		return False;
	}
	/*loop in a for loop that starts in index 1 to skip the double quotes and ends right before the last double qoutes*/
	for (i = 1; i < strlen(ps->data[1 + lableLine]) - 1; i++, (*dc)++) {
		/*if the word is a number print the value of the number into all of the 14 bits */
		dataToBin(word, ps->data[1 + lableLine][i], 0, WORD_LEN);
		/*then convert the string into a special 4 bit language*/
		binToSpe(word, finished);
		/*add it to the data arraylist to be printed at the end of the compiled object file*/
		addDataWord(dataHead, finished, *dc);
	}
	/*then add the \0 symbol to the end of the string which is just a 14 bit zero word*/
	dataToBin(word, 0, 0, WORD_LEN);
	binToSpe(word, finished);
	addDataWord(dataHead, finished, *dc);
	(*dc)++; /*dont forget to increment the dc */
	/*if the data line contains a label add the data line to the symbol table*/
	if (lableLine) addSymbol(symbolHead, ps->data[0], Data, startDc, (int)(strlen(ps->data[0]) - COLLON));
	free(word);
	free(finished);
	return True;
}

/*
This method handles a string line
INPUT: files* outputFiles - the stuct that contains all of output files, used only in phase two
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: dWord** dataHead - the data word array lists
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int* dc - the index of the data lines
INPUT: boolean lableLine - flag, used to tell if the line contains a label or not
INPUT: boolean firstPhase - flag, used to tell if its the first phase or the second (True = first, False = second)
OUTPUT: boolean - True if the code line is correct else False
*/
boolean handCodes(files* outputFiles, psr* ps, symbol** symbolHead, char fileName[], int line, int* ic, boolean lableLine, boolean firstPhase) {
	/*the max number of operands is 2 and the minimum is 0*/
	operand *src = NULL, *dst = NULL; 
	int startIc = *ic, 	/*the start instruction counter is used to all it to the symbol table */
		codeLen = 0; /*the codeLen is used for to get the total number of words in the line*/
	/*now check if the line contains a knowen code type*/
	codeTypes codeType = isCode(ps->data[0 + lableLine], 0, (int)(strlen(ps->data[0 + lableLine])));
	/*if code type is unknown print error but only once, and that's when its first phase*/
	if (codeType == INVALID) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid code: unknown code type\n", fileName + firstNameIndex(fileName), line);
		}
		return False;
	}
	/*if the code type is the following ones we have 2 operands and thus we check for two operands in this if*/
	else if (codeType == Mov || codeType == Cmp || codeType == Add || codeType == Sub || codeType == Lea) {
		/*the number of words are only 3 or 4 depending if there's a label*/
		if (ps->len == 3 + lableLine) {
			/*check if the first opernad contains a comma */
			if (ps->data[1+ lableLine][strlen(ps->data[1 + lableLine]) - 1] != ',') {
				if (firstPhase) {
					printf("#Error in file %s at line %d- Invalid code: missing comma/s (,)\n", fileName + firstNameIndex(fileName), line);
				}
				return False;
			}
			/*now get the size and type of the source and destination oprands */
			src = getOperandData(ps->data[1 + lableLine], outputFiles, ic, symbolHead, 
									fileName, line, (int)(strlen(ps->data[1 + lableLine]) - 1), firstPhase);
			dst = getOperandData(ps->data[2 + lableLine], outputFiles, ic, symbolHead, 
									fileName, line, (int)(strlen(ps->data[2 + lableLine])), firstPhase);
			/*if the operands are not right print the correct error*/
			if (!dst && !src) { 
				if (firstPhase) {
					printf("#Error in file %s at line %d- Invalid operation:"
							" both operandesses are unrecognized \n", fileName + firstNameIndex(fileName), line);
				}
				return False;
			}
			else if (!dst) {
				if (firstPhase) {
					printf("#Error in file %s at line %d- Invalid operation:"
							" destination operandess is unrecognized \n", fileName + firstNameIndex(fileName), line);
				}
				free(src);
				return False;
			}
			else if (!src) {
				if (firstPhase) {
					printf("#Error in file %s at line %d- Invalid operation:"
							" source operandess is unrecognized \n", fileName + firstNameIndex(fileName), line);
				}
				free(dst);
				return False;
			}
			/*now check if the code and the operands fit each other*/
			if (checkCode(src, dst, fileName, line, codeType, firstPhase)) {
				/*if they do and its the first phase add the size from the line to the total ic*/
				/*else if it's the second phase print the code into the object file*/
				if (firstPhase) {
					/*if the two operands are registers then they share the same word*/
					if (dst->operandType == DirectRegister && src->operandType == DirectRegister)
						 (*ic) = (*ic) + src->size + dst->size;
					else (*ic) = (*ic) + src->size + dst->size + 1; 
				}
				else printCodes(outputFiles->objectFile, src, dst, codeType, ic);
				free(src);
				free(dst);
			}
			else { 
				/*no need to print an error because the checkCode function prints it*/
				free(src);
				free(dst);
				return False;
			}
		}
		/*the lenght of of this code line is 3*/
		codeLen = 3;
	}
	/*if the code type is the following ones we have 1 operand and thus we check for one operand in this if*/
	/*p.s the operand to be used here is the destination*/
	else if (codeType == Not || codeType == Clr || codeType == Inc || codeType == Dec || codeType == Jmp ||
			 codeType == Bne || codeType == Red || codeType == Prn || codeType == Jsr){
		/*the number of words are only 2 or 3 depending if there's a label*/
		if (ps->len == 2 + lableLine) {
			/*now get the size and type of the destination oprand */
			dst = getOperandData(ps->data[1 + lableLine], outputFiles, ic, symbolHead, fileName, line, (int)(strlen(ps->data[1 + lableLine])), firstPhase);
			/*if the operand is not right print error*/
			if (!dst) {
				if (firstPhase) {
					printf("#Error in file %s at line %d- Invalid operation:"
							" destination operandess is unrecognized \n", fileName + firstNameIndex(fileName), line);
				}
				free(dst);	
				return False;
			}		
			/*now check if the code and the operand fit each other*/
			if (checkCode(src, dst, fileName, line, codeType, firstPhase)) {
				/*if they do and its the first phase add the size from the line to the total ic*/
				/*else if it's the second phase print the code into the object file*/
				if (firstPhase) (*ic) = (*ic) + dst->size + 1;
				else printCodes(outputFiles->objectFile, NULL, dst, codeType, ic);
				free(dst);
			}
			else { 
				/*no need to print an error because the checkCode function prints it*/
				free(dst);
				return False;
			}
		}
		/*the lenght of of this code line is 3*/
		codeLen = 2;
	}
	/*these two last code types do not contain operands*/
	else if (codeType == Rts || codeType == Stop) {
		/*meaning that we only check if lenght is ri*/
		if (ps->len == 1 + lableLine) {
			/*if its the first phase add the size from the line to the total ic*/
			/*else if it's the second phase print the code into the object file*/
			if (firstPhase) (*ic)++;
			else printCodes(outputFiles->objectFile, NULL, NULL, codeType, ic);
		}
		codeLen = 1;
	}
	if (ps->len > codeLen + lableLine) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid code: too many parameters\n", fileName + firstNameIndex(fileName), line);
		}
		return False;
	}
	else if (ps->len < codeLen + lableLine) {
		if (firstPhase) {
			printf("#Error in file %s at line %d- Invalid code: missing code parameters\n", fileName + firstNameIndex(fileName), line);
		}
		return False;
	}
	/*if there's a label add it only once which is only at the first phase*/
	if (lableLine && firstPhase) {
		addSymbol(symbolHead, ps->data[0], Code, startIc, (int)(strlen(ps->data[0]) - COLLON));
	}
	return True;
}

/*
This method prints the code line into the 
INPUT: FILE* file - the file to print to 
INPUT: operand* src - the source opernad if there's any 
INPUT: operand* dst - the destination operand if there's any
INPUT: int codeType - the type of code 
INPUT: int* ic - the instruction counter that will be printed in the object file 
OUTPUT: NONE
*/
void printCodes(FILE* file, operand* src, operand* dst, codeTypes codeType, int* ic) {
	/*set the data strings for converting the data lines for the compiled file*/
	char* word = (char*)malloc(WORD_LEN + 1), * finished = (char*)malloc(WORD_LEN / 2 + 1);
	/*the array that contains the max numbers of the operands*/
	operand* operandTemp[2];
	int i;
	if (!word || !finished) return;
	finished[WORD_LEN / 2] = word[WORD_LEN] = '\0';

	operandTemp[0] = src;
	operandTemp[1] = dst;
	/*now if there's no operands then it means its a code type*/
	/*that does not have operands and could be printed right away to the file*/
	if (!src && !dst) {
		/*the first 4 bits are always zero*/
		dataToBin(word, 0, 0, 4);
		/*the next 4 bits are the code type*/
		dataToBin(word, codeType, 4, 4);
		/*because the code does not have any operands the last bits are zero too*/
		dataToBin(word, 0, 8, 6);
		binToSpe(word, finished);
		fprintf(file, "%04d	%s\n", (*ic)++, finished);
	}
	else { /*if there are operands do the following*/
		/*the first 4 bits are always zero*/
		dataToBin(word, 0, 0, 4);
		/*the next 4 bits are the code type*/
		dataToBin(word, codeType, 4, 4);
		/*the source operand is not a must so we check only the source if there is any*/
		/*if there is the 8th and 9th bits are the source type */
		(src) ? dataToBin(word, src->operandType, 8, 2) : dataToBin(word, 0, 8, 2);
		/*the next 2 bits are the destination operand*/
		dataToBin(word, dst->operandType, 10, 2);
		/*the last 2 bits (ARE) in the code word are always zero*/
		dataToBin(word, 0, 12, 2);
		binToSpe(word, finished);
		fprintf(file, "%04d	%s\n", (*ic)++, finished);
		/*if there's a source operand start from the 0 index else 1*/
		for (i = (!src); i < 2; i++) {
			/*if the operand type is Immediate we check if its a number or a lable*/
			if (operandTemp[i]->operandType == Immediate) {
				/*then we add that number to the 12 bits of the word*/
				dataToBin(word, (operandTemp[i]->number == INVALID) ? operandTemp[i]->firstSymbol->value : operandTemp[i]->number, 0, 12);
				/*the last 2 bits are ARE and this operand type is absolute which is 2*/
				dataToBin(word, ABSOLUTE, 12, 2);
				binToSpe(word, finished);
				fprintf(file, "%04d	%s\n", (*ic)++, finished);
			}
			/*if the operand type is Direct we check if the symbols are external or relocatable*/
			if (operandTemp[i]->operandType == Direct) {
				/*then we add that symbols value to the 12 bits of the word*/
				dataToBin(word, operandTemp[i]->firstSymbol->value, 0, 12);
				/*the last 2 bits are ARE and this operand type is external or relocatable depends if the type is external or data*/
				(operandTemp[i]->firstSymbol->type == External) ? dataToBin(word, EXTERNAL, 12, 2) : dataToBin(word, RELOCATABLE, 12, 2);
				binToSpe(word, finished);
				fprintf(file, "%04d	%s\n", (*ic)++, finished);
			}
			/*if the operand type is FixedIndex we check if its a number or a lable*/
			if (operandTemp[i]->operandType == FixedIndex) {
				/*then we add that symbols value to the 12 bits of the word*/
				dataToBin(word, operandTemp[i]->firstSymbol->value, 0, 12);
				/*the last 2 bits are ARE and this operand type is external or relocatable depends if the type is external or data*/
				(operandTemp[i]->firstSymbol->type == External) ? dataToBin(word, EXTERNAL, 12, 2) : dataToBin(word, RELOCATABLE, 12, 2);
				binToSpe(word, finished);
				fprintf(file, "%04d	%s\n", (*ic)++, finished);
				/*now we're on the second operand and we check if its a number or a lable*/
				dataToBin(word, (operandTemp[i]->number == INVALID) ? operandTemp[i]->secondSymbol->value : operandTemp[i]->number, 0, 12);
				/*the last 2 bits are ARE and this operand type is absolute which is 2*/
				dataToBin(word, ABSOLUTE, 12, 2);
				binToSpe(word, finished);
				fprintf(file, "%04d	%s\n", (*ic)++, finished);
			}
			/*the last operand type is the DirectRegister*/
			if (operandTemp[i]->operandType == DirectRegister) {
				/*with this type the first 6 bits are always zero*/
				dataToBin(word, 0, 0, 6);
				/*if the source and destination are both registers we share the same word*/
				if (src && (src->operandType == dst->operandType)) {
					/*the next 3 bits are for the source*/
					dataToBin(word, operandTemp[i]->number, 6, 3);
					/*the next 3 bits are for the destination*/
					/*both of which are the register number*/
					dataToBin(word, operandTemp[i + 1]->number, 9, 3);
					/*the last 2 bits are ARE and this operand type is absolute which is 2*/
					dataToBin(word, ABSOLUTE, 12, 2);
					binToSpe(word, finished);
					fprintf(file, "%04d	%s\n", (*ic)++, finished);
					i = 1; /*to skip the other operandess */
				}
				else {
					/*if both are not registers we do the same with the same position for the respective opperand*/
					dataToBin(word, (!i) ? operandTemp[i]->number : 0, 6, 3);
					dataToBin(word, (!i) ? 0 : operandTemp[i]->number, 9, 3);
					dataToBin(word, ABSOLUTE, 12, 2);
					binToSpe(word, finished);
					fprintf(file, "%04d	%s\n", (*ic)++, finished);
				}
			}
		}
	}
	free(word);
	free(finished);
}

/*
This method prints Data and String lines to the end of the object file
INPUT: FILE* file -	the file to print to
INPUT: dWord** dataHead - the array list that contains the data
INPUT: int *ic - the instruction counter that we sum with the data counter
OUTPUT: NONE
*/
void printDataToFile(FILE* file, dWord** dataHead, int* ic) {
	dWord* dTemp;
	for (dTemp = *dataHead; dTemp; dTemp = dTemp->next) {
		fprintf(file, "%04d	%s\n", dTemp->dc + *ic, dTemp->word);
	}
}
