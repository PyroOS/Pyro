CFLAGS += -I. -Wall -c
CXXFLAGS += -I. -I../../include -Wall -c

PLUGIN = dbx

OBJDIR = objs
OBJS = dbx libdbx
OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

all: $(OBJDIR) $(OBJS)
	g++ $(OBJS) -plugin -Xlinker -Bsymbolic -lsyllable -o $(PLUGIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

install:
	mkdir -p /Applications/Whisper/plugins/ixport
	cp -pf $(PLUGIN) /Applications/Whisper/plugins/ixport/

clean:
	-rm $(OBJS)
	-rm $(PLUGIN)

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@gcc $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

