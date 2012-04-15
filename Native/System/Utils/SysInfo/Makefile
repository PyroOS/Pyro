COPTS	= -c

OBJS = main.o sysinfopanel.o drivespanel.o processpanel.o memview.o cpuview.o performancepanel.o devicespanel.o
EXE	= "SystemInformation"

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -lpyro -o "SystemInformation"
	strip --strip-all $(EXE)
	rescopy -r $(EXE) icons/*.png
	rescopy -a $(EXE) resources/SysInfo.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/SysInfo.catalog=resources/$$CATALOG/SysInfo.catalog; \
		fi \
	done;
	addattrib $(EXE) "os::Category" "System Tools"

.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o : main.cpp
	$(CXX) $(COPTS) $< -o $@
	
sysinfopanel.o : sysinfopanel.cpp
	$(CXX) $(COPTS) $< -o $@

drivespanel.o : drivespanel.cpp
	$(CXX) $(COPTS) $< -o $@

processpanel.o : processpanel.cpp 
	$(CXX) $(COPTS) $< -o $@

memview.o : memview.cpp
	$(CXX) $(COPTS) $< -o $@
	
cpuview.o : cpuview.cpp
	$(CXX) $(COPTS) $< -o $@

devicespanel.o : devicespanel.cpp
	$(CXX) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs $(EXE) inst web clean

catalogs:
	cd resources; \
	catcomp -c SysInfo.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t SysInfo.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/binary/$(EXE) | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/SysInfo
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/SysInfo/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Utils/SysInfo/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/SysInfo.catalog resources/SysInfo.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/SysInfo.catalog; \
	done;
	sync
