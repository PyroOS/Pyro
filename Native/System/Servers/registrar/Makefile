CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions
AOPTS    = -c

VPATH = ./

OBJS = registrar.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/registrar

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/registrar: $(OBJS)
	$(CXX) $(OBJS) -lpyro -o registrar


deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/servers
	cp -pf registrar $(PYRO_BINARY_DESTINATION)/System/servers/
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/registrar "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/servers/registrar | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/registrar "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/servers/registrar "os::MimeType" "pyro/server"
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) registrar
	sync
