CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = Address.o Settings.o

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/Address.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/Address.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=Address.so" $(OBJS) -lpyro -o $(OBJDIR)/Address.so
	rescopy -r $(OBJDIR)/Address.so resources/*.png resources/*.html

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/Address.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Address.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/Address.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
