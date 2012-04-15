COPTS	= -c

OBJS	= calc.o calculator.o stack.o calcbuttons.o calcdisplay.o togglebutton.o remsa.o base-n.o postoffice.o aboutwin.o
EXE	= Calculator

$(EXE) : catalogs $(OBJS)
	$(CXX) $(OBJS) -lpyro -lm -o $@
	rescopy -r $(EXE) *.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;

#.c.o :
#	$(CC) $(COPTS) $< -o $@
.cpp.o:
	$(CXX) $(COPTS) $< -o $@

calc.o:		calc.cpp calculator.h stack.h calcbuttons.h calcdisplay.h calc.h togglebutton.h remsa.h base-n.h
calculator.o:	calculator.cpp calculator.h stack.h
stack.o:		stack.cpp stack.h
calcbuttons.o:	calcbuttons.cpp calcbuttons.h togglebutton.h
remsa.o:		remsa.cpp remsa.h
calcdisplay.o:	calcdisplay.cpp calcdisplay.h calc.h
togglebutton.o:	togglebutton.cpp togglebutton.h
base-n.o:			base-n.cpp base-n.h
postoffice.o:		postoffice.cpp postoffice.h
aboutwin.o:		aboutwin.cpp aboutwin.h

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs $(EXE) inst web clean

catalogs:
	cd resources; \
	catcomp -c $(EXE).cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t $(EXE).cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Applications/$(EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
