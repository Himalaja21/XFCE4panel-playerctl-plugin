# Panel Player - XFCE4 Panel Plugin
# A media player controller using playerctl

.PHONY: all clean install uninstall

all: build/Makefile
	@$(MAKE) -C build

build/Makefile: CMakeLists.txt
	@mkdir -p build
	@cd build && cmake ..

clean:
	@rm -rf build

install: all
	@$(MAKE) -C build install

uninstall:
	@if [ -d build ]; then \
		$(MAKE) -C build uninstall; \
	else \
		echo "Build directory not found. Run 'make' first."; \
	fi
