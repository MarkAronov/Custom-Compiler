CC=gcc
CFLAGS=-Wall -ansi -pedantic
DEPS =	data.h helper.h handler.h parser.h phases.h
OBJ =	data.o helper.o handler.o parser.o phases.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

assembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	-rm *.o

clean:
	-rm assembler

help:
	@echo  ''
	@echo  'Commands:'
	@echo  '  clean           - Remove the assembler file'
	@echo  '  assembler       - Compile the project'
	@echo  ''