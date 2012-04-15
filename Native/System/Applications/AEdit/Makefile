COPTS = -Wno-deprecated -c

OBJSDIR = objs/

OBJS = $(OBJSDIR)main.o $(OBJSDIR)appwindow.o $(OBJSDIR)editview.o $(OBJSDIR)aboutdialog.o $(OBJSDIR)gotodialog.o $(OBJSDIR)finddialog.o $(OBJSDIR)replacedialog.o $(OBJSDIR)buffer.o $(OBJSDIR)mytabview.o $(OBJSDIR)tabview.o $(OBJSDIR)icons.o $(OBJSDIR)dialog.o
EXE = AEdit

all: $(OBJSDIR) $(EXE)

$(OBJSDIR):
	mkdir $(OBJSDIR)

$(EXE): $(OBJS)
	$(CXX) $(OBJS) -lpyro -o $(EXE)
	rescopy -r $(EXE) resources/AEdit.catalog
	rescopy -a $(EXE) stock_copy_16=pixmaps/stock_copy_16.png
	rescopy -a $(EXE) stock_copy_24=pixmaps/stock_copy_24.png
	rescopy -a $(EXE) stock_cut_16=pixmaps/stock_cut_16.png
	rescopy -a $(EXE) stock_cut_24=pixmaps/stock_cut_24.png
	rescopy -a $(EXE) stock_exit_16=pixmaps/stock_exit_16.png
	rescopy -a $(EXE) stock_jump_to_16=pixmaps/stock_jump_to_16.png
	rescopy -a $(EXE) stock_left_arrow_16=pixmaps/stock_left_arrow_16.png
	rescopy -a $(EXE) stock_new_16=pixmaps/stock_new_16.png
	rescopy -a $(EXE) stock_new_24=pixmaps/stock_new_24.png
	rescopy -a $(EXE) stock_open_16=pixmaps/stock_open_16.png
	rescopy -a $(EXE) stock_open_24=pixmaps/stock_open_24.png
	rescopy -a $(EXE) stock_paste_16=pixmaps/stock_paste_16.png
	rescopy -a $(EXE) stock_paste_24=pixmaps/stock_paste_24.png
	rescopy -a $(EXE) stock_redo_16=pixmaps/stock_redo_16.png
	rescopy -a $(EXE) stock_redo_24=pixmaps/stock_redo_24.png
	rescopy -a $(EXE) stock_right_arrow_16=pixmaps/stock_right_arrow_16.png
	rescopy -a $(EXE) stock_save_16=pixmaps/stock_save_16.png
	rescopy -a $(EXE) stock_save_24=pixmaps/stock_save_16.png
	rescopy -a $(EXE) stock_save_as_16=pixmaps/stock_save_as_16.png
	rescopy -a $(EXE) stock_search_16=pixmaps/stock_search_16.png
	rescopy -a $(EXE) stock_search_24=pixmaps/stock_search_24.png
	rescopy -a $(EXE) stock_search_replace_16=pixmaps/stock_search_replace_16.png
	rescopy -a $(EXE) stock_undo_16=pixmaps/stock_undo_16.png
	rescopy -a $(EXE) stock_undo_24=pixmaps/stock_undo_24.png
	rescopy -a $(EXE) stock_select_all_16=pixmaps/stock_select_all_16.png
	rescopy -a $(EXE) stock_close_16=pixmaps/stock_close_16.png
	rescopy -a $(EXE) stock_font_16=pixmaps/stock_font_16.png
	rescopy -a $(EXE) icon24x24.png=pixmaps/icon24x24.png
	rescopy -a $(EXE) icon48x48.png=pixmaps/icon48x48.png
	rescopy -a $(EXE) text_plain.png=pixmaps/text_plain.png
	rescopy -a $(EXE) about=pixmaps/about.png
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;

$(OBJSDIR)main.o: main.cpp main.h
	$(CXX) $(COPTS) main.cpp -o $@

$(OBJSDIR)appwindow.o: appwindow.cpp appwindow.h
	$(CXX) $(COPTS) appwindow.cpp -o $@

$(OBJSDIR)button_bar.o: button_bar.cpp button_bar.h
	$(CXX) $(COPTS) button_bar.cpp -o $@

$(OBJSDIR)editview.o: editview.cpp editview.h
	$(CXX) $(COPTS) editview.cpp -o $@

$(OBJSDIR)aboutdialog.o: aboutdialog.cpp aboutdialog.h
	$(CXX) $(COPTS) aboutdialog.cpp -o $@

$(OBJSDIR)gotodialog.o: gotodialog.cpp gotodialog.h
	$(CXX) $(COPTS) gotodialog.cpp -o $@

$(OBJSDIR)finddialog.o: finddialog.cpp finddialog.h
	$(CXX) $(COPTS) finddialog.cpp -o $@

$(OBJSDIR)replacedialog.o: replacedialog.cpp replacedialog.cpp
	$(CXX) $(COPTS) replacedialog.cpp -o $@

$(OBJSDIR)buffer.o: buffer.cpp buffer.h
	$(CXX) $(COPTS) buffer.cpp -o $@

$(OBJSDIR)status_bar.o: status_bar.cpp status_bar.h
	$(CXX) $(COPTS) status_bar.cpp -o $@

$(OBJSDIR)mytabview.o: mytabview.cpp mytabview.h
	$(CXX) $(COPTS) mytabview.cpp -o $@

$(OBJSDIR)tabview.o: tabview.cpp tabview.h
	$(CXX) $(COPTS) tabview.cpp -o $@

$(OBJSDIR)icons.o: icons.cpp icons.h
	$(CXX) $(COPTS) icons.cpp -o $@

$(OBJSDIR)dialog.o: dialog.cpp dialog.h
	$(CXX) $(COPTS) dialog.cpp -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all inst web clean

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
	strip --strip-all $(EXE)
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/binary
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)
	cp -pf $(EXE) $(PYRO_BINARY_DESTINATION)/Applications/$(EXE)/
	ln -sf /boot/Applications/$(EXE)/$(EXE) $(PYRO_BINARY_DESTINATION)/System/binary/aedit
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
	rm -f $(EXE)
	rm -rf $(OBJSDIR)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
