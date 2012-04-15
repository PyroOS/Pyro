CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS    = -c

VPATH = ./
OBJS	= mixerchannel.o mixerview.o oss_output.o
OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/oss.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/oss.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=oss.so" $(OBJS) -lpyro -lmedia -o $(OBJDIR)/oss.so

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/media
	cp -f $(OBJDIR)/oss.so $(PYRO_BINARY_DESTINATION)/System/extensions/media/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/media/oss.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/media/oss.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
