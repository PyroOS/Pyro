#Makefile for iview
COPTS = -c -fexceptions -Wall -o2
LOPTS = -lpyro

OBJS = 	main.o appwindow.o iview.o imagekeeper.o iconbar.o common.o layouter.o iconbutton.o

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

all : objs ImageViewer

objs:
	@echo Creating subdirectories...
	@mkdir -p objs

ImageViewer: $(OBJS)
	@echo Linking...
	@$(CXX)   $(OBJS) -o ImageViewer $(LOPTS)
	@echo Adding resources...	
	@rescopy -r ImageViewer images/*
	rescopy -a ImageViewer resources/ImageViewer.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a ImageViewer $$CATALOG/ImageViewer.catalog=resources/$$CATALOG/ImageViewer.catalog; \
		fi \
	done;

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

catalogs:
	cd resources; \
	catcomp -c ImageViewer.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t ImageViewer.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/ImageViewer
	cp -pf ImageViewer $(PYRO_BINARY_DESTINATION)/Applications/ImageViewer/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/ImageViewer/ImageViewer "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/ImageViewer/ImageViewer | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/ImageViewer
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/ImageViewer/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/ImageViewer/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f ImageViewer
	rm -rf $(OBJDIR)
	sync

catalog-clean:
	rm -f resources/ImageViewer.catalog resources/ImageViewer.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/ImageViewer.catalog; \
	done;
	sync
