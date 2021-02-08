#ifndef PARSER
#define PARSER
#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "data.h"


void			freePsr				(psr* parsed);
char**			resizeData			(char** data, int size);
char*			resizeString		(char str[]);
void			makeString			(char** data, char buffer[], int size, int numOfStrings, int firstIndex);
stringIndexes*	getStringIndexes	(char buffer[], char fileName[], int line, int i, int firstStrIndex, int lastStrIndex, boolean firstPhase);
psr*			parser				(char buffer[], char fileName[], int line, boolean firstPhase);
#endif