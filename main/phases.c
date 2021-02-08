#include "phase_one.h"

/***********************************************
***** This file contains the main function *****
******* that sets the data and reads the *******
*** .as file and sends each line to handlers ***
***********************************************/

/*
This method handles each .as line 
INPUT: files* outputFiles - the stuct that contains all of output files, used only in phase two
INPUT: psr* ps - the parsed line of data that contains the data in strings
INPUT: symbol** symbolHead - the symbol table to check if there's already used label
INPUT: dWord** dataHead - the data word array lists
INPUT: entry** entryList - the entrylist to check or add entries
INPUT: char fileName[] - the file to print the error about, if there's an error
INPUT: int line - the line where the error occurs, if there's an error
INPUT: int *ic - the instruction counter that we sum with the data counter
INPUT: int* dc - the index of the data lines
INPUT: boolean firstPhase - flag, used to tell if its the first phase or the second (True = first, False = second)
OUTPUT: boolean - True if the line is correct else False
*/
boolean phasesHandler(files* outputFiles, psr* ps, symbol** symbolHead, dWord** dataHead, entry** entryList, char fileName[], int line, int* ic, int* dc, boolean firstPhase){
	boolean lableLine = False; /* flag - used to check if the line contains a lable so that in data extraction will not be out of bounds*/
	if (!ps->len) return True;
	if (ps->data != NULL) {
		/*first thing is to check if the line is a macro line*/
		if (isDataType(ps->data[0] , 0, (int)(strlen(ps->data[0]))) == DefineLine) {
			return (!firstPhase) ? True : handMacro(ps, symbolHead, entryList, fileName, line);
		}
		/*if not a macro line check if the first word of the line is a label*/
		else if (ps->data[0][(int)(strlen(ps->data[0])) - 1] == ':') {
			if (firstPhase && !handLabel(ps, symbolHead, fileName, line)) return False;
			lableLine = True;
		}
		/*now check if the line contains datatypes */
		if (ps->len >= 2) {
			switch (isDataType(ps->data[0 + lableLine], 0, (int)(strlen(ps->data[0 + lableLine])))) {
			case(DefineLine):
				break;
			case(DataLine):
				return (!firstPhase)? True : handData(ps, symbolHead, dataHead, fileName, line, dc, lableLine);
				break;
			case(StringLine):
				return (!firstPhase) ? True : handString(ps, symbolHead, dataHead, fileName, line, dc, lableLine);
				break;
			case(EntryLine):
				return (!firstPhase) ? True : handEntry(ps, symbolHead, entryList, fileName, line, lableLine);
				break;
			case(ExternLine):
				return (!firstPhase) ? True : handExtern(ps, symbolHead, entryList, fileName, line, lableLine);
				break;
			}
			/*if not a datatype then check if the line is a code line*/
			return handCodes(outputFiles, ps, symbolHead, fileName, line, ic, lableLine, firstPhase);
		}
		else {
			if (firstPhase) {
				printf("#Error in file %s at line %d- Invalid line:"
						" unrecognized operation! -> %s\n", fileName + firstNameIndex(fileName), line, ps->data[0]);
			}
			return False;
		}
	}
	return True;
}

/*
This method handles the first phase of the project
INPUT: int argc - th number of files 
INPUT: char* argv[] - the array of strings that contains the file names
OUTPUT: NONE
*/
void phaseOneProcess(int argc, char* argv[]) {
	
	/*set up the data structs*/
	symbol* symbolHead = NULL;
	dWord* dataHead = NULL;
	entry* entryList = NULL;
	psr* parsed = NULL;

	/*a flag used to block file making*/
	boolean status = True;

	int i, /* index for loops*/
		line, /*the number of line */
		ic = 0, /*instruction counter*/
		dc = 0; /*data counter*/
	char *fileName = NULL, /*file name given as a parameter from argv, might be converted if contains the .ps file name at the end*/
		 *asmFileName = NULL; /*the file name of the asm file, just the fileName but with .as*/
	FILE* asmFile = NULL; 
	char buffer[BUFFER_SIZE];

	if (argc >= 2) {
		for (i = 1; i < argc; i++) {
			/*reset the values after we've delt if the previous file*/
			symbolHead = NULL;
			dataHead = NULL;
			entryList = NULL;
			status = True;
			line = 1, ic = dc = 0;
			asmFileName = NULL;
			asmFile = NULL;
			parsed = NULL;

			if(stringCmp(".as", argv[i], (int)(strlen(argv[i]) - AS_LEN),(int)(strlen(argv[i])))){
				asmFile = fopen(argv[i], "r");
				fileName = (char*)malloc((int)(strlen(argv[i]) - AS_LEN + 1));
				if (!fileName) return;
				strncpy(fileName, argv[i], strlen(argv[i]) -AS_LEN);
				fileName[(int)(strlen(argv[i]) - AS_LEN)] = '\0';
			}
			else {
				fileName = argv[i];
				if (!fileName) return;
				asmFileName = (char*)malloc(strlen(fileName));
				if (!asmFileName) return;
				strcpy(asmFileName, fileName);
				strcat(asmFileName, ".as");
				asmFile = fopen(asmFileName, "r");
			}

			/*check if the file could be opened*/
			if (!asmFile) {
				printf("#Error: Can not open file!\n");
			}
			/*if the file is openable we check if it's empty or not*/
			else if (!checkIfFileEmpty(asmFile)) {
				/*if the file is not empty get each line and put it to the buffer*/
				while (fgets(buffer, BUFFER_SIZE, asmFile)) {
					/*a valid line should be less than 80 letters*/
					if (strlen(buffer) > LINE_LEN) {
						printf("#Error in file %s at line %d- Invalid line lenght:"
								" a line should be less than 80 words!\n", argv[i] + firstNameIndex(argv[i]), line);
						status = False;
					}
					else {
						/*add ' ' just for empty strings*/
						strcat(buffer, " ");
						/*get the processed line into word array*/
						parsed = parser(buffer, argv[i], line, True);
						/*now we send these words to word handlers*/
						if (parsed && !phasesHandler(NULL, parsed, &symbolHead, &dataHead, &entryList, argv[i], line, &ic, &dc, True) && status) {
							status = False;
						}
						freePsr(parsed);
					}
					line++;
				}
				/*if the file was valid we update the symbol table*/
				if (status) updateSymbols(&symbolHead, &ic);

				phaseTwoProcess(asmFile, fileName, &symbolHead, &dataHead, &entryList, status, &ic, &dc);
			}
			else {
				printf("#Error in file %s: file is empty\n", argv[i] + firstNameIndex(argv[i]));
				fclose(asmFile);
			}
		}
	}
	else {
		printf("#Error: no file was given!\n");
		return;
	}
}

void phaseTwoProcess(FILE* asmFile, char fileName[], symbol** symbolHead, dWord** dataHead, entry** entryList, boolean status, int* icSum, int* dcSum) {
	int line = 1, ic = 100;
	char* objectFileName = NULL,
		* externFileName = NULL,
		* entryFileName = NULL;
	FILE* objectFile = NULL,
		* externFile = NULL,
		* entryFile = NULL;
	psr* parsed = NULL;
	char buffer[BUFFER_SIZE];
	files* outputFiles = (files*)malloc(sizeof(files));
	if (!outputFiles) return;

	rewind(asmFile); /*we reset the pointer so that we will be able to use a non empty file*/

	objectFileName = (char*)malloc(strlen(fileName) + OB_LEN + 1);
	if (!objectFileName) return;
	strcpy(objectFileName, fileName);
	strcat(objectFileName, ".ob");
	objectFile = fopen(objectFileName, "w+");


	externFileName = (char*)malloc(strlen(fileName) + EXT_LEN + 1);
	if (!externFileName) return;
	strcpy(externFileName, fileName);
	strcat(externFileName, ".ext");
	externFile = fopen(externFileName, "w+");


	entryFileName = (char*)malloc(strlen(fileName) + ENT_LEN + 1);
	if (!entryFileName) return;
	strcpy(entryFileName, fileName);
	strcat(entryFileName, ".ent");
	entryFile = fopen(entryFileName, "w+");


	/*if (temp) free(temp);*/
	fprintf(objectFile, "	%d	%d\n", *icSum, *dcSum);
	outputFiles->entryFile = entryFile;
	outputFiles->externFile = externFile;
	outputFiles->objectFile = objectFile;

	/*the same lines as phase one function*/
	if (!asmFile) {
		printf("#Error: Can not open file!\n");
		return;
	}
	while (fgets(buffer, BUFFER_SIZE, asmFile)) {
		if (strlen(buffer) > LINE_LEN) {
			status = False;
		}
		else {
			strcat(buffer, " ");
			parsed = parser(buffer, fileName, line, False);
			if (parsed && !phasesHandler(outputFiles, parsed, symbolHead, NULL, NULL, fileName, line, &ic, 0, False) && status) {
				status = False;
			}
			freePsr(parsed);
		}
		line++;
	}
	if (!status) {
		fclose(asmFile);
		fclose(objectFile);
		fclose(externFile);
		fclose(entryFile);
		if (!remove(objectFileName) && !remove(externFileName) && !remove(entryFileName)) {
			printf("\nERROR:\n");
			printf("Assembly file %s was not created due to errors!!\n\n", fileName + firstNameIndex(fileName));
			return;
		}
	}
	else {
		/*if the file is fine print the entry and data word */
		printEntryList(outputFiles, symbolHead, entryList);
		printDataToFile(objectFile, dataHead, &ic);
		/*remove entry or extern files that are empty because there are none in the code*/
		if (checkIfFileEmpty(externFile)) {
			fclose(externFile);
			remove(externFileName);
		}
		else fclose(externFile);
		if (checkIfFileEmpty(entryFile)) {
			fclose(entryFile);
			remove(entryFileName);
		}
		else fclose(entryFile);
		fclose(asmFile);
		fclose(objectFile);
	}
	free(outputFiles);
	free(objectFileName);
	free(externFileName);
	free(entryFileName);
	clearData(symbolHead, dataHead, entryList);
	printf("\nSUCCESS:\n");
	printf("Assembly file %s was compiled\n\n", fileName + firstNameIndex(fileName));
}

int main(int argc, char* argv[]) {
	phaseOneProcess(argc, argv);
	return 0;
}