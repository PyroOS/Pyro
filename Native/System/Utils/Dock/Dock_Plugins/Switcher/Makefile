CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = Switcher.o

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Switcher.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Switcher.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Switcher.so" $(OBJS) -lpyro -o $(OBJDIR)/Switcher.so
	rescopy -r $(OBJDIR)/Switcher.so images/*.png

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/Switcher.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Switcher.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Switcher.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
