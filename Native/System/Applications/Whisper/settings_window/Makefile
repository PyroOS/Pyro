include ../Makeconf

CXXFLAGS += -I. -I.. -I../include

all: $(OBJDIR) $(SETTINGS_OBJS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	-rm -f $(SETTINGS_OBJS)

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@g++ $(CXXFLAGS) $< -o $@

