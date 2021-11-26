CC = g++
CFLAGS = -std=gnu++0x
LDFLAGS = -lpthread
HEADERFILES = utils.h structs.h


compile: $(HEADERFILES)
	$(CC)  $(CFLAGS) $(LDFLAGS)  main.cpp -o exec 

clean:
	rm *.o exec