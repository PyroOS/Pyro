CXXFLAGS += -c -I.
OBJS = main.o users_window.o user_propertiesdlg.o changepwddlg.o group_propertiesdlg.o
EXE = Users
TARGET_EXE = UsersAndGroups

#Define TEST if you're testing the Preferences; doesn't overwrite system files
#CXXFLAGS += -DTEST

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -lpyro -lcrypt -o $(EXE)
	strip --strip-all $(EXE)
	rescopy $(EXE) -r *.png
	rescopy -a $(EXE) resources/UsersAndGroups.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/UsersAndGroups.catalog=resources/$$CATALOG/UsersAndGroups.catalog; \
		fi \
	done;

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs $(EXE) inst web clean

catalogs:
	cd resources; \
	catcomp -c UsersAndGroups.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t UsersAndGroups.cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Preferences
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Preferences/$(TARGET_EXE)
	addattrib "$(PYRO_BINARY_DESTINATION)/Preferences/$(TARGET_EXE)" "CheckSum::MD5" `md5sum "$(PYRO_BINARY_DESTINATION)/Preferences/$(TARGET_EXE)" | cut -c1-32`
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/LIST
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(TARGET_EXE)
	cd resources; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/LIST/$$CATALOG; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(TARGET_EXE)/int; \
			touch $(PYRO_BINARY_DESTINATION)Web/Languages/Preferences/$(TARGET_EXE)/$$CATALOG; \
		fi \
	done;
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/UsersAndGroups.catalog resources/UsersAndGroups.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/UsersAndGroups.catalog; \
	done;
	sync
