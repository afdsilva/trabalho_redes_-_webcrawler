#Makefile

#Compilador
CC=g++

# Objects
OBJETOS=http.o ssl.o Thread.o webcrawler.o

# Compilador flags - Warning, debug, boost
CPPFLAGS=-std=c++0x -Wall -g

LDFLAGS=-lm -lboost_regex -lssl
EXECUTAVEL=webcrawler
BASE_FILENAME=trabalho_redes_-_webcrawler

all: $(OBJETOS)
	$(CC) $(OBJETOS) $(CPPFLAGS) $(LDFLAGS) -o $(EXECUTAVEL)

webcrawler: http.h webcrawler.cpp
	$(CC) $(CPPFLAGS) -c webcrawler.cpp

http: http.h http.cpp
	$(CC) $(CPPFLAGS) -c http.cpp
	
ssl: ssl.h ssl.cpp
	$(CC) $(CPPFLAGS) -c ssl.cpp
	
Thread: Thread.h Thread.cpp
	$(CC) $(CPPFLAGS) -c Thread.cpp

clean:
	rm -rf *.o
	rm -rf $(EXECUTAVEL)
	rm -rf saida.txt

package:
	@echo "Empacotando: "
ifneq ($(wildcard *$(BASE_FILENAME)*),)
	rename 's/tar.gz/$(REVISION).tar.gz' $(wildcard *$(BASE_FILENAME)*) 
	#@echo "Arquivo Existe"
endif
ifeq ($(wildcard $(BASE_FILENAME).tar.gz),)
	mkdir $(BASE_FILENAME)
	cp -R README.txt *.cpp *.h makefile $(BASE_FILENAME)/
	tar -zcvf $(BASE_FILENAME).tar.gz $(BASE_FILENAME)/
	#zip -r $(BASE_FILENAME).zip $(BASE_FILENAME)/
	rm -R $(BASE_FILENAME)/
endif
