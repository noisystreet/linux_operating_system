obj-m += hello_netfilter.o

KDIR ?= /lib/modules/$(shell uname -r)/build

TARGETS := echo_server echo_client

.PHONY: all modules clean user

all: user

user: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

modules:
	@if [ ! -d "$(KDIR)" ]; then \
		echo "错误: 未找到内核头文件 $(KDIR)"; \
		echo "请安装: sudo apt install linux-headers-$$(uname -r)"; \
		exit 1; \
	fi
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean 2>/dev/null || true
	rm -f $(TARGETS)
