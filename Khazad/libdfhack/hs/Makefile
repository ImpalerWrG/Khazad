CC = gcc
CFLAGS = -g -Wall -W -mconsole -mno-cygwin

# strip all debug information
#LFLAGS = -Wl,-s

# include full debug information
LFLAGS = -g

LIBS = -lmapi32 -lpsapi
#LIBS = -lpsapi

reveal2.exe: reveal2.o hexsearch.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $+  $(LIBS)

hexsearch.o: ../hexsearch.c ../hexsearch.h
	$(CC) $(CFLAGS) -c $<

reveal2.o: reveal2.c ../hexsearch.h 
	$(CC) $(CFLAGS) -c $<

.PHONY:zip
zip:	reveal2.exe reveal2_readme.txt reveal2_opengl.bin reveal2_sdlapp.bin
	-@rm reveal2_sourcecode.zip
	zip -9j reveal2_sourcecode.zip *.c *.h ..\\hex*.c ..\\hex*.h Makefile
	-@rm reveal2.zip
	zip -9 reveal2.zip reveal2.exe reveal2.ini \
		reveal2_readme.txt reveal2_sourcecode.zip \
		reveal2_opengl.bin reveal2_sdlapp.bin
	zip -z reveal2.zip < reveal2_readme.txt
