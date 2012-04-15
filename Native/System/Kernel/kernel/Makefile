dist: build inst doc web clean

build:
	make -f Makefile-compile all
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/libraries
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/drivers/net/if
	cp -f objs/kernel $(PYRO_BINARY_DESTINATION)/System/kernel
	cp -f objs/eth_if $(PYRO_BINARY_DESTINATION)/System/drivers/net/if/eth_if
	cp -f objs/libkernel.so $(PYRO_BINARY_DESTINATION)/System/libraries/libkernel.so
	strip --strip-all $(PYRO_BINARY_DESTINATION)/System/kernel
	addattrib $(PYRO_BINARY_DESTINATION)/System/drivers/net/if/eth_if "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/drivers/net/if/eth_if | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/libkernel.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/libraries/libkernel.so | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/libkernel.so "os::Icon" "/System/icons/logo.png"
	addattrib $(PYRO_BINARY_DESTINATION)/System/libraries/libkernel.so "os::MimeType" "pyro/library"
	addattrib $(PYRO_BINARY_DESTINATION)/System/kernel "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/kernel | cut -c1-32`
	addattrib $(PYRO_BINARY_DESTINATION)/System/kernel "os::Icon" "/System/icons/logo.png"
	sync

doc:
	mkdir -p docs/DriverAPI
	mkdir -p docs/Syscalls
	mkdir -p docs/Private
	doxygen driverapi.dox
	doxygen syscalls.dox
	doxygen private.dox
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/DriverAPI
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/SystemCalls
	mkdir -p $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/KernelInternals
	cp -rf docs/DriverAPI/html/* $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/DriverAPI/
	cp -rf docs/Syscalls/html/* $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/SystemCalls/
	cp -rf docs/Private/html/* $(PYRO_BINARY_DESTINATION)Native/Development/Documentation/API/KernelInternals/
	sync

web:
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Documentation/API/DriverAPI
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Documentation/API/SystemCalls
	mkdir -p $(PYRO_BINARY_DESTINATION)Web/Documentation/API/KernelInternals
	cp -rf docs/DriverAPI/html/* $(PYRO_BINARY_DESTINATION)Web/Documentation/API/DriverAPI/
	cp -rf docs/Syscalls/html/* $(PYRO_BINARY_DESTINATION)Web/Documentation/API/SystemCalls/
	cp -rf docs/Private/html/* $(PYRO_BINARY_DESTINATION)Web/Documentation/API/KernelInternals/
	sync

clean:
	rm -rf objs
	rm -rf docs
	sync
