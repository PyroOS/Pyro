CXXFLAGS += -I. -I../../include -Wall -c

PLUGIN = mbox

OBJDIR = objs
OBJS = mbox
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

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

