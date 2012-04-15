#Makefile for TableView test app
COPTS = -c -fexceptions -ffast-math

OBJS = main.o tableview.o

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

all : objs tabletest

objs:
	@mkdir -p objs

tabletest: $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o tabletest -lsyllable
	@echo Adding resources...

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f tabletest
