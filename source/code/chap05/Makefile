TARGETS := io_demo sync_demo seek_demo

.PHONY: all clean

all: $(TARGETS)

io_demo: io_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

sync_demo: sync_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

seek_demo: seek_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
