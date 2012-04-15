CC   = gcc
LL   = gcc

OBJS = AttribEdit.o etextview.o
EXE  = AttribEdit
COPTS = -c -Wall -O2

all: $(OBJS)
	$(LL) $(OBJS) -lpyro -lstdc++ -o $(EXE)
	rescopy -r $(EXE) ./icons/*
	strip --strip-all $(EXE)
	addattrib $(EXE) "os::Category" "System Tools"

%.o: %.cpp
	$(CC) $(COPTS) $< -o $@

AttribEdit.o: AttribEdit.cpp
etextview.o: etextview.cpp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

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
	rm -rf $(OBJS)
	rm -f $(EXE)
	sync
