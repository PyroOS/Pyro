OPTS   = -Wall -c

CC	= g++
EXE	= Icni

OBJDIR	:= objs

$(OBJDIR)/%.o : %.c
	$(CC) $(OPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(OPTS) $< -o $@

OBJS := ./objs/main.o ./objs/win.o ./objs/imageview.o ./objs/node.o ./objs/gameview.o

$(EXE): mkobjdir objs $(OBJS)
	g++ $(OBJS) -lstdc++ -lpyro -o $(EXE)
	rescopy -r $(EXE) ./resources/*.png
	addattrib $(EXE) os::Category Games

mkobjdir:
	mkdir -p $(OBJDIR)

deps:
	mkdir objs

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(EXE) inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f Icni
	sync
