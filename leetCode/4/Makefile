CPP = g++
CFLAGS = -g -Wall 
NAME = 4
$(NAME):$(NAME).o
	$(CPP) $(CFLAGS) -o $@ $<
$(NAME).o:$(NAME).cpp stdafx.h
	$(CPP) $(CFLAGS) -c $<
clean:
	rm 4
	rm *.o
