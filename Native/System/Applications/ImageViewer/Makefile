#Makefile for iview
COPTS = -c -fexceptions -Wall -o2
LOPTS = -lsyllable

CATALOGS = sv

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
	@rescopy -r ImageViewer resources/lang.catalog
	@rescopy -a ImageViewer images/*
	for CATALOG in $(CATALOGS);do \
		echo $$CATALOG; \
		rescopy -a ImageViewer $$CATALOG/lang.catalog=resources/$$CATALOG/lang.catalog; \
	done;

clean:
	@echo Cleaning...
	@rm -r $(OBJDIR)
	@rm -f ImageViewer
