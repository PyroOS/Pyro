
CFLAGS = -c -Wall
LFLAGS =
LIBS = -lsyllable
CC = g++

OBJS = main.o HexEditWindow.o HexView.o InnerView.o EditController.o FindDialogue.o

EXE = HexEditor

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(EXE) $(LIBS)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) resources/*
	addattrib $(EXE) os::Category Development

clean:
	rm $(OBJS) $(EXE)

%.o: %.cpp %.h
	$(CC) -c $< $(CFLAGS) -o $@


