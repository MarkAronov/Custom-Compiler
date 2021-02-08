#ifndef HANDLER
#define HANDLER
#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "data.h"

boolean handMacro		(psr* ps, symbol** symbolHead, entry** entryList, char fileName[], int line);
boolean handLabel		(psr* ps, symbol** symbolHead, char fileName[], int line);
boolean handExtern		(psr* ps, symbol** symbolHead, entry** entryList, char fileName[], int line, boolean lableLine);
boolean handEntry		(psr* ps, symbol** symbolHead, entry** entryList, char fileName[], int line, boolean lableLine);
boolean handData		(psr* ps, symbol** symbolHead, dWord** dataHead, char fileName[], int line, int* dc, boolean lableLine);
boolean handString		(psr* ps, symbol** symbolHead, dWord** dataHead, char fileName[], int line, int* dc, boolean lableLine);
boolean handCodes		(files* outputFiles, psr* ps, symbol** symbolHead, char fileName[], int line, int* ic, boolean lableLine, boolean firstPhase);
void	printCodes		(FILE* file, operand* src, operand* dst, codeTypes codeType, int* ic);
void	printEntryList	(files* outputFiles, symbol** symbolHead, entry** entryList);
#endif