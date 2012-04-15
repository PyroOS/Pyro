CFLAGS += -I. -Wall -c
CXXFLAGS += -I. -I../../include -Wall -c

PLUGIN = pst

OBJDIR = objs
OBJS = libpst strfunc lzfu timeconv debug pst
OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(OBJS)))

all: $(OBJDIR) $(OBJS)
	g++ $(OBJS) -plugin -Xlinker -Bsymbolic -lpyro -o $(PLUGIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@gcc $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport
	cp -pf $(PLUGIN) $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/
	addattrib $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/$(PLUGIN) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/Applications/Whisper/plugins/ixport/$(PLUGIN) | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -f $(PLUGIN)
	rm -rf $(OBJDIR)
	sync
