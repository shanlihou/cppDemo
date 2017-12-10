ifeq ($(PLAT), X86)
CFLAGS= -D __X86__
CPP=g++
else
CPP=arm-linux-gnueabihf-4.9.1-g++
endif
LIBS=FTPServer.o FTPProtol.o myEpoll.o FileInfo.o Record.o
FTPServer:$(LIBS)

	@echo $(PLAT)
	$(CPP) $(CFLAGS) -o $@ $(LIBS)
FTPServer.o:FTPServer.cpp FTPProtol.h
	$(CPP) $(CFLAGS) -c $<
FTPProtol.o:FTPProtol.cpp FTPProtol.h Record.h
	$(CPP) $(CFLAGS) -c $<
myEpoll.o:myEpoll.cpp myEpoll.h FTPProtol.h Record.h
	$(CPP) $(CFLAGS) -c $<
FileInfo.o:FileInfo.cpp FileInfo.h
	$(CPP) $(CFLAGS) -c $<
Record.o:Record.cpp Record.h
	$(CPP) $(CFLAGS) -c $<

clean:
	rm *.o
	rm FTPServer

	
