OPTS   = -Wall -c

CC	= g++
EXE	= Icni

OBJDIR	:= objs

$(OBJDIR)/%.o : %.c
	$(CC) $(OPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(OPTS) $< -o $@

OBJS := ./objs/main.o ./objs/win.o ./objs/imageview.o ./objs/node.o ./objs/gameview.o

$(EXE): objs $(OBJS)
	g++ $(OBJS) -lstdc++ -lsyllable -o $(EXE)
	rescopy -r $(EXE) ./resources/*.png
deps:
	mkdir objs

clean:
	-rm $(OBJS)

