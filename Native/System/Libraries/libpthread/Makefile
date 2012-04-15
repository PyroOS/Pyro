CFLAGS := -c -Iinc/

OBJDIR := objs

OBJS := attr.o cond.o mutex.o pthread.o rwlock.o bits.o

#DEBUG := y
ifdef DEBUG
 CFLAGS += -g -D__ENABLE_DEBUG__
 OBJS += debug.o
endif

include $(PYRO_COMMON)/makefile.rules

VER := 1
LIB := libpthread.so.$(VER)

all: $(OBJDIR) $(OBJS)
	$(CC) -shared -Xlinker "-soname=$(LIB)" $(OBJS) -o $(OBJDIR)/$(LIB)

$(OBJDIR):
	-mkdir $(OBJDIR)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	cp -f $(OBJDIR)/$(LIB) $(PYRO_BINARY_DESTINATION)/System/libraries/
	ln -sf $(LIB) $(PYRO_BINARY_DESTINATION)/System/libraries/libpthread.so
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/$(LIB) "os::MimeType" "pyro/library"
	sync

clean:
	rm -rf $(OBJDIR)
	sync
