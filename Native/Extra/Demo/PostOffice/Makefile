COPTS	= -Wall -c

OBJS	= postoffice.o main.o
EXE	= postoffice

CC	= gcc
LL	= gcc

$(EXE) : $(OBJS)
	$(LL) $(OBJS) -latheos -lstdc++ -o $@

clean:
	-rm $(OBJS)
#.c.o :
#	$(CC) $(COPTS) $< -o $@
.cpp.o:
	$(CC) $(COPTS) $< -o $@

postoffice.o:		postoffice.cpp postoffice.h
main.o:			main.cpp postoffice.h



