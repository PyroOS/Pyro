CFLAGS   += -c
CXXFLAGS += -c

EXE = rescopy

OBJS = rescopy.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/$(EXE)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(EXE): $(OBJS) Makefile
	$(CXX) $(OBJS) -o $(OBJDIR)/$(EXE) -lpyro

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/System/binary
	cp -f $(OBJDIR)/$(EXE) $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Development/System/binary/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Development/$(EXE)
	sync

clean:
	rm -rf $(OBJDIR)
	sync
