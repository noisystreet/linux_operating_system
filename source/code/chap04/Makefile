TARGETS := malloc_demo mmap_demo maps_dump segv_demo mmap_cow

.PHONY: all clean

all: $(TARGETS)

malloc_demo: malloc_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

mmap_demo: mmap_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

maps_dump: maps_dump.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

segv_demo: segv_demo.cpp
	$(CXX) $(CXXFLAGS) -g -o $@ $<

mmap_cow: mmap_cow.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
