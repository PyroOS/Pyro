CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = Camera.o

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Camera.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Camera.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Camera.so" $(OBJS) -lpyro -o $(OBJDIR)/Camera.so
	rescopy -r $(OBJDIR)/Camera.so images/*.png

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/Camera.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Camera.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Camera.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
