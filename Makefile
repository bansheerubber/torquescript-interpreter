target = dist/eggscript
cclibs = -lpthread
cc = g++
ccflags = -Os -Wall -std=c++17

cpp_source = $(shell find src -name "*.cc")
cpp_headers = $(shell find src -name "*.h")
cpp_objects = $(patsubst %.cc, %.o, $(cpp_source))

.PHONY: default all clean

default: $(target)
all: default

$(cpp_objects) : %.o : %.cc
	mkdir -p $(dir $@)
	$(cc) $(ccflags) -c $< -o $@

$(target): $(cpp_objects)
	mkdir -p $(dir $(target))
	$(cc) $(cpp_objects) -Wall $(cclibs) -o $@

clean:
	rm -f $(cpp_objects)
	rm -f $(target)