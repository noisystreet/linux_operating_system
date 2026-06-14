TARGETS := print_ids setuid_demo keepcaps_demo secure_file seccomp_demo

.PHONY: all clean

all: $(TARGETS)

print_ids: print_ids.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

setuid_demo: setuid_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

keepcaps_demo: keepcaps_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

secure_file: secure_file.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

seccomp_demo: seccomp_demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
