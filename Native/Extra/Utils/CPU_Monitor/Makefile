COPTS	= -c -I../../Kernel/drivers/misc/w8378x/

OBJS	= main.o graphview.o barview.o
EXE	= CPU_Monitor

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -lpyro -o $(EXE)
	strip --strip-all $(EXE)
	rescopy -r $(EXE) *.png
	rescopy -a $(EXE) resources/CPU-Monitor.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/CPU-Monitor.catalog=resources/$$CATALOG/CPU-Monitor.catalog; \
		fi \
	done;
	addattrib $(EXE) os::Category "System Tools"

.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o : main.cpp
	$(CXX) $(COPTS) $< -o $@
barview.o : barview.cpp barview.h
	$(CXX) $(COPTS) $< -o $@
graphview.o : graphview.cpp graphview.h
	$(CXX) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Graphical CPU usage monitor.

dist: catalogs $(EXE) inst package web clean

catalogs:
	cd resources; \
	catcomp -c CPU-Monitor.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t CPU-Monitor.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE)
	addattrib $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE)/System/binary/$(EXE) | cut -c1-32`
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Native/Extra/Utils/$(EXE) && \
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Extra/$(EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Extra/$(EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Extra/$(EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/CPU-Monitor.catalog resources/CPU-Monitor.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/CPU-Monitor.catalog; \
	done;
	sync
