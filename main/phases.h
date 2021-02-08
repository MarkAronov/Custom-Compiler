#ifndef PHASE_ONE_H
#define PHASE_ONE_H
#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include"handler.h"

boolean phasesHandler(files* outputFiles, psr* ps, symbol** symbolHead, dWord** dataHead, entry** entryList, char fileName[], int line, int* ic, int* dc, boolean firstPhase);
void	phaseOneProcess	(int argc, char* argv[]);
#endif