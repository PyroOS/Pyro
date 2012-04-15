CFLAGS := -c -I.

OBJDIR := objs

OBJS := dlfcn.o
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(CFLAGS) $< -o $@

VER := 1
LIB := libdl.so.$(VER)

all: $(OBJDIR) $(OBJS)
	$(CC) -shared -Xlinker "-soname=$(LIB)" $(OBJS) -o $(OBJDIR)/$(LIB)

$(OBJDIR):
	-mkdir $(OBJDIR)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Include
	cp -f $(OBJDIR)/$(LIB) $(PYRO_BINARY_DESTINATION)/System/libraries/
	ln -sf $(LIB) $(PYRO_BINARY_DESTINATION)/System/libraries/libdl.so
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "os::MimeType" "pyro/library"
	cp -f dlfcn.h $(PYRO_BINARY_DESTINATION)Native/Development/Include/
	addattrib $(PYRO_BINARY_DESTINATION)Native/Development/Include/dlfcn.h "os::MimeType" "text/x-h"
	sync

clean:
	rm -rf $(OBJDIR)
	sync
