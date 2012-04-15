CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions
AOPTS   = -c

VPATH = ./

OBJS	= Remote.o

OBJDIR	= objs/

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Remote

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Remote: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Remote" $(OBJS) -lpyro -o $(OBJDIR)/Remote

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins
	cp -pf objs/Remote $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/Remote "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/Remote | cut -c1-32`
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) Remote
	sync
