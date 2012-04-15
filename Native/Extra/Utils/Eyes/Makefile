CC = g++
LL = g++
COPTS = -Wall -Wno-deprecated -c -O3
OBJS = 	main.o			\
		eyewidget.o
       

EXE = eyes

$(EXE): $(OBJS)
	$(LL) $(OBJS) -lsyllable -o $(EXE)

clean:
	-rm $(OBJS)
	-rm $(EXE)

.cpp.o:
	$(CC) $(COPTS) $< -o $@


