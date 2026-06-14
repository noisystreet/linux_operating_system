TARGETS := echo_server echo_client

.PHONY: all clean

all: $(TARGETS)

echo_server: echo_server.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

echo_client: echo_client.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
