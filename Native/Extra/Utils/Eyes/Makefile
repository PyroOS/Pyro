CC = g++
LL = g++
COPTS = -Wall -Wno-deprecated -c -O3
OBJS = 	main.o eyewidget.o

EXE = Eyes

$(EXE): $(OBJS)
	$(LL) $(OBJS) -lpyro -o $(EXE)

.cpp.o:
	$(CC) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Moving eyes.

dist: $(EXE) inst package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/Applications/$(EXE)
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/Applications/$(EXE)/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	sync

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	sync
