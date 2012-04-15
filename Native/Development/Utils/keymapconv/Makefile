COPTS	= -c

OBJS	= keymapconv.o
EXE	= keymapconv

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -lpyro -o $@

.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o : main.cpp
	$(CXX) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(EXE) inst web clean

inst:
	strip --strip-all $(EXE)
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/$(EXE)
	sync

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	sync
