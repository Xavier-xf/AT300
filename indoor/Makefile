TOOLCHAIN_PATH ?= /opt/arm-anykav500-linux-uclibcgnueabi/bin
PARTS ?= app

all:
	mkdir -p build
	PATH=$(TOOLCHAIN_PATH):$$PATH cmake -S . -B build
	cmake --build build -j16

sd:
	bash ./package_sd_upgrade.sh $(PARTS)

sd-no-build:
	bash ./package_sd_upgrade.sh --no-build $(PARTS)

clean:
	rm -rf build
