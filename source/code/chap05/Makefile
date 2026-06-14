TARGETS := io_demo sync_demo seek_demo o_direct_demo

.PHONY: all clean

all: $(TARGETS)

io_demo: io_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

sync_demo: sync_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

seek_demo: seek_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

o_direct_demo: o_direct_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
