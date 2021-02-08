#include "phases.h"

boolean phaseOneHandle(files* outputFiles, psr* ps, symbol** symbolHead, dWord** dataHead, entry** entryList, char fileName[], int line, int* ic, int* dc){
	int lableLine = 0;
	if (!ps->len) return True;
	if (ps->data != NULL) {
		/*if the command is print then check what matrix and check for errors*/
		if (isDataType(ps->data[0] , 0, (int)(strlen(ps->data[0]))) == DefineLine) {
			return handMacro(ps, symbolHead, fileName, line);
		}
		else if (ps->data[0][(int)(strlen(ps->data[0])) - 1] == ':') {
			if(!handLabel(ps, symbolHead, fileName, line)) return False;
			lableLine = 1;
		}
		if (ps->len >= 2) {
			switch (isDataType(ps->data[0 + lableLine], 0, (int)(strlen(ps->data[0 + lableLine])))) {
			case(DefineLine):
				break;
			case(DataLine):
				return handData(ps, symbolHead, dataHead, fileName, line, dc, lableLine);
				break;
			case(StringLine):
				return handString(ps, symbolHead, dataHead, fileName, line, dc, lableLine);
				break;
			case(EntryLine):
				return handEntry(ps, symbolHead, entryList, fileName, line, lableLine);
				break;
			case(ExternLine):
				return handExtern(ps, symbolHead, fileName, line, lableLine);
				break;
			}
			return handCodes(NULL, ps, symbolHead, fileName, line, ic, lableLine, True);
		}
		else {
			printf("#Error in file %s at line %d- Invalid line: unrecognized operation! -> %s\n", fileName, line, ps->data[0]);
			return False;
		}
	}
	return True;
}

void phaseOneProcess(int argc, char* argv[]) {
	symbol* symbolHead = NULL;
	dWord* dataHead = NULL;
	entry* entryList = NULL;
	boolean status = True;
	int i, line, ic = 0, dc = 0;
	char* fileName = NULL;
	FILE* asmFile = NULL;
	psr* parsed = NULL;
	char buffer[BUFFER_SIZE];
	if (argc >= 2) {
		for (i = 1; i < argc; i++) {
			symbolHead = NULL; 
			dataHead = NULL;
			entryList = NULL;
			status = True;
			line = 1, ic = dc = 0;
			fileName = NULL;
			asmFile = NULL;
			parsed = NULL;

			fileName = (char*)malloc(strlen(argv[i]));
			if (!fileName) return;

			strcpy(fileName, argv[i]);
			strcat(fileName, ".as");
			asmFile = fopen(fileName, "r");

			if (!asmFile) {
				printf("#Error: Can not open file!\n");
				return;
			}
			if (!checkIfFileEmpty(asmFile)) {
				while (fgets(buffer, BUFFER_SIZE, asmFile)) {
					if (strlen(buffer) > LINE_LEN) {
						printf("#Error in file %s at line %d- Invalid line lenght: a line should be less than 80 words!\n", argv[i], line);
						status = False;
					}
					else {
						strcat(buffer, " ");
						parsed = parser(buffer, argv[i], line, True);
						if (parsed && !phaseOneHandle(parsed, &symbolHead, &dataHead, &entryList, argv[i], line, &ic, &dc) && status) {
							status = False;
						}
						/*
						if (parsed) {
							printf("%d\n", line);
							for (int j = 0; j < parsed->len; j++) {
								printf("%s\n", parsed->data[j]);
							}
							printf("\n");
						}
						*/
						freePsr(parsed);
					}
					line++;
				}
				if (status) updateSymbols(&symbolHead, &ic);
				phaseTwoProcess(asmFile,argv[i], &symbolHead, &dataHead, &entryList, status, &ic, &dc);
			}
			else {
				printf("#Error in file %s: file is empty\n", argv[i]);
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

	objectFileName = (char*)malloc(strlen(fileName) + 4);
	if (!objectFileName) return;
	strcpy(objectFileName, fileName);
	strcat(objectFileName, ".ob");
	objectFile = fopen(objectFileName, "w+");


	externFileName = (char*)malloc(strlen(fileName) + 5);
	if (!externFileName) return;
	strcpy(externFileName, fileName);
	strcat(externFileName, ".ext");
	externFile = fopen(externFileName, "w+");


	entryFileName = (char*)malloc(strlen(fileName) + 5);
	if (!entryFileName) return;
	strcpy(entryFileName, fileName);
	strcat(entryFileName, ".ent");
	entryFile = fopen(entryFileName, "w+");


	/*if (temp) free(temp);*/
	fprintf(objectFile, "%d	%d\n", *icSum, *dcSum);
	outputFiles->entryFile = entryFile;
	outputFiles->externFile = externFile;
	outputFiles->objectFile = objectFile;

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
			if (parsed && !phaseOneHandle(outputFiles, parsed, symbolHead, fileName, line, &ic) && status) {
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
			printf("Assembly file %s was not created due to errors!!\n\n", fileName);
			return;
		}
	}
	else {
		printEntryList(outputFiles, symbolHead, entryList);
		printDataToFile(objectFile, dataHead, &ic);
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
	printf("Assembly file %s was compiled\n\n", fileName);
}

int main(int argc, char* argv[]) {
	phaseOneProcess(argc, argv);
	return 0;
}