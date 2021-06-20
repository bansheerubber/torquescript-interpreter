target = dist/torquescript
cclibs = -lpthread -lfmt
cc = g++
ccflags = -O2 -Wall -Wno-switch -Bsymbolic -fno-semantic-interposition -std=c++17

cpp_source = $(shell find src -name "*.cc")
cpp_source_tmp = $(subst src, tmp, $(cpp_source))
cpp_source_without = $(subst src\/, , $(cpp_source))

cpp_headers = $(shell find src -name "*.h")
cpp_headers_tmp = $(subst src, tmp, $(cpp_headers))

cpp_objects = $(patsubst %.cc, %.o, $(cpp_source))
cpp_objects_tmp = $(patsubst %.cc, %.o, $(cpp_source_tmp))
cpp_objects_without = $(patsubst src\/, , $(cpp_source))

.PHONY: default clean

# force synchronization for preprocessor
default:
	$(MAKE) preprocessor
	$(MAKE) $(target)

preprocessor:
	python3 tools/preprocessor.py

$(cpp_objects_tmp) : %.o : %.h
$(cpp_objects_tmp) : %.o : %.cc
	mkdir -p $(dir $@)
	$(cc) $(ccflags) -c $< -o $@

$(target): $(cpp_objects_tmp)
	mkdir -p $(dir $(target))
	$(cc) $(cpp_objects_tmp) -Wall $(cclibs) -o $@

test: $(target)
	cd ./dist/ && ./$(target) --test

build-tests: $(target)
	cd ./dist/ && ./$(target) --test --overwrite-results

clean:
	rm -Rf tmp
	rm -f $(target)