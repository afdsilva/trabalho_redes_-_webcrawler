#Makefile

#Compilador
CC=g++

# Objects
OBJETOS=http.o Thread.o

# Compilador flags - Warning, debug, boost
CPPFLAGS=-std=c++0x -Wall -g -lm -lboost_regex

EXECUTAVEL=webcrawler

webcrawler: $(OBJETOS)
	$(CC) $(OBJETOS) $(CPPFLAGS) -o $(EXECUTAVEL)
	rm -rf $(OBJETOS)

http: http.h http.cpp
	$(CC) $(CPPFLAGS) -c http.cpp
	
Thread: Thread.h Thread.cpp
	$(CC) $(CPPFLAGS) -c Thread.cpp

clean:
	rm -rf $(EXECUTAVEL)
