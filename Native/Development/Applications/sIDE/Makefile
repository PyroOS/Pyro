CC = g++ -O2
COPTS   = -I$(TOP)/src  -c -Wall

APPBIN = /Applications/sIDE
OBJS = AddWindow.o AddLibrary.o ProjectPrefs.o GroupWindow.o ProjectWindow.o NewProject.o project.o main.o 

OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))


#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CC) $(COPTS) $< -o $@


all :	objs sIDE

objs:
	mkdir objs

translations:
	catcomp -c resources/sIDE.cd
	catcomp -t resources/de/sIDE.cd

sIDE: translations $(OBJS)
	g++ $(OBJS) -o sIDE -lsyllable
	rescopy sIDE -r ./images/*.png
	rescopy -a sIDE resources/sIDE.catalog
	rescopy -a sIDE de/sIDE.catalog=resources/de/sIDE.catalog
	strip --strip-all sIDE
	addattrib sIDE os::Category Development
	build_templates.sh

install: sIDE
	mkdir -p $(APPBIN)
	cp -f sIDE $(APPBIN)/sIDE
	mkdir -p $(APPBIN)/Templates
	cp -f Templates/*.zip $(APPBIN)/Templates/

clean:
	rm -f $(OBJDIR)/* sIDE
	rm -f Templates/*.zip






















































