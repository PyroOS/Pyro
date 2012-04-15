CFLAGS   += -c -fexceptions
CXXFLAGS += -c -fexceptions
AOPTS   = -c

VPATH = ./

OBJS	= bitmap.o blur.o color.o config.o img.o loop.o paste.o render.o text.o blursk.o

OBJDIR	= objs/

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Blur

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Blur: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Blur" $(OBJS) -lpyro -o $(OBJDIR)/Blur

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins
	cp -pf objs/Blur $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/Blur "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/ColdFish/Plugins/Blur | cut -c1-32`
	sync

clean:
	rm -rf $(OBJS) $(OBJDIR) Blur
	sync
