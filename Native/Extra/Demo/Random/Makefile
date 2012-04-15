COPTS = -Wall -c -O2 -fexceptions

OBJS = main.o application.o mainwindow.o random.o view.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : objs "App"

objs:
	@mkdir -p objs

"App": $(OBJS)
	@echo Linking...
	@gcc $(OBJS) -o DemoApp  -lpyro -lstdc++

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Random class.

dist: all inst checksum package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random/
	cp -rf ./* $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random/
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random/objs
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Demo && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random.zip ./Random && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Demo/Random && \
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f DemoApp
	sync
