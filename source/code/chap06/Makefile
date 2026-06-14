obj-m += hello_chardev.o

KDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: all modules clean

all: modules

modules:
	@if [ ! -d "$(KDIR)" ]; then \
		echo "错误: 未找到内核头文件 $(KDIR)"; \
		echo "请安装: sudo apt install linux-headers-$$(uname -r)"; \
		exit 1; \
	fi
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

chardev_test: chardev_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean 2>/dev/null || true
	rm -f chardev_test
