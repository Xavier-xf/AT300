TOOLCHAIN_PATH ?= /opt/arm-anykav500-linux-uclibcgnueabi/bin

all:
	mkdir -p build
	PATH=$(TOOLCHAIN_PATH):$$PATH cmake -S . -B build
	cmake --build build -j16
clean:
	rm -rf build
