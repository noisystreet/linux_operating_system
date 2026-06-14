TARGETS := fork_demo pipe_demo thread_demo mmap_shared

.PHONY: all clean

all: $(TARGETS)

fork_demo: fork_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

pipe_demo: pipe_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

thread_demo: thread_demo.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<

mmap_shared: mmap_shared.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
