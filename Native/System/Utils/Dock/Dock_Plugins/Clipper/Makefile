CFLAGS   = -c -fexceptions -ffast-math
CXXFLAGS = $(CFLAGS)
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = Clipper.o

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Clipper.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Clipper.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Clipper.so" $(OBJS) -lpyro -o $(OBJDIR)/Clipper.so
	rescopy -r $(OBJDIR)/Clipper.so images/*.png

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/Clipper.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Clipper.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Clipper.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
