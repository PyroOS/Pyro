COPTS   = -c -fexceptions

OBJS = mixer.o

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

EXE := Mixer

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all :	objs $(EXE) $(PLUGIN)

objs:
	mkdir objs

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) -lpyro -lmedia
	strip --strip-all $(EXE)
	rescopy $(EXE) -r *.png
	addattrib $(EXE) os::Category "System Tools"

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Graphical soundlevel monitor.

dist: all inst package web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE)
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE) | cut -c1-32`
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Extra/$(EXE)
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(EXE) $(PLUGIN)
	sync
