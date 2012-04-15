CFLAGS   += -c -fexceptions -ffast-math
CXXFLAGS += -c -fexceptions -ffast-math
AOPTS   = -c
VPATH = ./
OBJDIR	:= objs
OBJS = CpuSpeed.o 

all :	$(OBJDIR) $(OBJS) $(OBJDIR)/CpuSpeed.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/CpuSpeed.so: $(OBJS)
	$(CXX) -shared -Xlinker "-soname=CpuSpeed.so" $(OBJS) -lpyro -o $(OBJDIR)/CpuSpeed.so

deps: $(OBJDIR)

-include $(OBJDIR)/*.d

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/dock
	cp -f $(OBJDIR)/CpuSpeed.so $(PYRO_BINARY_DESTINATION)/System/extensions/dock/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/dock/CpuSpeed.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/dock/CpuSpeed.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f $(OBJS)
	sync
