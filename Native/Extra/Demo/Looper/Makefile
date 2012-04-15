COPTS = -c -fexceptions -ffast-math

OBJS = main.o appwindow.o commthread.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : objs loop

objs:
	@mkdir -p objs

loop: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o DemoApp -lpyro

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Looper is an example how you can use threads inside your Pyro application.

dist: all inst checksum package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper/
	cp -rf ./* $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper/
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper/objs
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Demo && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper.zip ./Looper && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Looper && \
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f DemoApp
	sync
