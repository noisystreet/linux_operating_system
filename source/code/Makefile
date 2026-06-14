# Linux 操作系统教程 — 示例代码顶层 Makefile

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra

USER_DIRS := chap01 chap02 chap03 chap04 chap05 chap07 chap08 chap09

.PHONY: all user kernel clean test

all: user

user:
	@for d in $(USER_DIRS); do \
		echo "==> building $$d"; \
		$(MAKE) -C $$d CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" || exit 1; \
	done
	@echo "==> building chap06 (chardev_test)"
	@$(MAKE) -C chap06 chardev_test CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"

kernel:
	@$(MAKE) -C chap06 modules
	@$(MAKE) -C chap07 modules

clean:
	@for d in $(USER_DIRS) chap06; do \
		$(MAKE) -C $$d clean 2>/dev/null || true; \
	done

# 编译全部用户态示例（跳过需交互或 root 的运行时测试）
test: user
	@echo "==> smoke test"
	./chap01/sysinfo
	./chap03/fork_demo
	./chap03/pipe_demo
	./chap03/thread_demo
	./chap04/malloc_demo
	./chap04/mmap_cow
	./chap04/maps_dump | head -3
	./chap05/io_demo
	./chap05/seek_demo
	./chap05/sync_demo
	./chap05/o_direct_demo
	./chap08/raw_syscall
	./chap09/print_ids
	./chap09/secure_file
	@echo "==> network echo test"
	@./chap07/echo_server & \
		SERVER_PID=$$!; \
		sleep 0.5; \
		./chap07/echo_client 127.0.0.1 "ping" | grep -q ping; \
		kill $$SERVER_PID 2>/dev/null || true; \
		wait $$SERVER_PID 2>/dev/null || true
	@echo "==> all smoke tests passed"
