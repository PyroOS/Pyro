# Makefile for non-compilable items.

dist: inst

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/config
	cp -r * $(PYRO_BINARY_DESTINATION)/System/config/
	rm $(PYRO_BINARY_DESTINATION)/System/config/Makefile
	sync

clean:
	sync
