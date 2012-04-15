COPTS	= -c -fexceptions -ffast-math

OBJS	= main.o tview.o settings.o swindow.o
EXE	= Terminal

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -lpyro -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) *.png
	addattrib $(EXE) "os::Category" "System Tools"

%.o : %.c
	$(CC) $(COPTS) $< -o $@

%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

settings.o:	settings.cpp settings.h
main.o:		main.cpp tview.h messages.h
tview.o:	tview.cpp tview.h swindow.h messages.h
swindow.o:	swindow.cpp swindow.h messages.h

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(EXE) inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/$(EXE)
	sync

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	sync
