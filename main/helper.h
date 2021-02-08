#ifndef HELPER
#define HELPER
#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"data.h"

int			firstNameIndex	(char str[]);
void		printDataToFile	(FILE* file, dWord** dataHead, int *ic);
void		dataToBin		(char str[], int number, int start, int lenght);
void		binToSpe		(char src[], char dst[]);
dataTypes	isDataType		(char str[], int start, int end);
codeTypes	isCode			(char str[], int start, int end);
int			isReg			(char str[], int start, int end);
boolean		checkNumber		(char str[], int start, int end);
boolean		checkLabel		(char str[], char fileName[], int line, int start, int end, boolean printError);
boolean		checkIfFileEmpty(FILE* file);
boolean		checkCode		(operand* src, operand* dst, char fileName[], int line, codeTypes type, boolean printError);
operand*	getOperandData	(char str[], files* outputFiles, int* ic, symbol** symbolHead, char fileName[], int line, int end, boolean firstPhase);
#endif