CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

LIBNAME = libmedia.so.1

VPATH = ./

OBJS	= codec.o input.o output.o manager.o inputselector.o soundplayer.o stage.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/$(LIBNAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(LIBNAME): $(OBJS)
	$(CXX) -shared -Xlinker "-soname=$(LIBNAME)" $(OBJS) -lm -lpyro -o $(OBJDIR)/$(LIBNAME)

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst doc web clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	cp -f $(OBJDIR)/$(LIBNAME) $(PYRO_BINARY_DESTINATION)/System/libraries/
	ln -sf $(LIBNAME) $(PYRO_BINARY_DESTINATION)/System/libraries/libmedia.so
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIBNAME) "os::MimeType" "pyro/library"
	sync

doc:
	mkdir -p docs
	doxygen libmedia.dox
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/LibMedia
	cp -rf docs/html/* $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/LibMedia/
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Documentation/API/LibMedia
	cp -rf docs/html/* $(PYRO_BINARY_DESTINATION)Web/Documentation/API/LibMedia/
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Libraries/libmedia
	sync

clean:
	rm -rf $(OBJDIR)
	rm -rf docs
	sync
