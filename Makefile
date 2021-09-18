target = torquescript
library_target = libtorquescript.a
library_include_c_target = include.c/ts.h
library_include_cpp_target = include.cpp/ts.h
cclibs = -lpthread -lreadline
CC = g++
CPPFLAGS = -g -Wall -Wno-switch -Wno-class-memaccess -Wno-delete-incomplete -Wno-attributes -Bsymbolic -fno-semantic-interposition -std=c++17
soflags =
ldflags =

cpp_source = $(shell find src -type f -name "*.cc" ! -path "src/include*")
cpp_source_tmp = $(subst src, tmp, $(cpp_source))
cpp_source_without = $(subst src\/, , $(cpp_source))

cpp_headers = $(shell find src -type f -name "*.h" ! -path "src/include*")
cpp_headers_tmp = $(subst src, tmp, $(cpp_headers))

cpp_objects = $(patsubst %.cc, %.o, $(cpp_source))
cpp_objects_tmp = $(patsubst %.cc, %.o, $(cpp_source_tmp))
cpp_objects_without = $(patsubst src\/, , $(cpp_source))

.PHONY: default clean

# force synchronization for preprocessor
default:
	@"$(MAKE)" preprocessor --no-print-directory
	@"$(MAKE)" dist/$(target) --no-print-directory

library:
	@"$(MAKE)" preprocessor --no-print-directory
	@"$(MAKE)" dist/$(library_target) --no-print-directory soflags="-fPIC" ldflags="-Wl,--version-script=libtorquescript.map"

preprocessor:
	@echo -e "   PY      tools/preprocessor.py"
	@python3 tools/preprocessor.py

$(cpp_objects_tmp) : %.o : %.h
$(cpp_objects_tmp) : %.o : %.cc
	@mkdir -p $(dir $@)
	@echo -e "   CC      $<"
	@$(CC) $(CPPFLAGS) $(soflags) -c $< -o $@

dist/$(target): $(cpp_objects_tmp)
	@mkdir -p $(dir dist/$(target))
	@echo -e "   CC      $@"
	@$(CC) $(cpp_objects_tmp) -Wall $(cclibs) -o $@

dist/$(library_target): $(cpp_objects_tmp)
	@mkdir -p $(dir dist/$(library_target))
	@echo -e "   CC      $@"
	@$(CC) $(cpp_objects_tmp) -Wall $(cclibs) -shared $(ldflags) -o $@
	@ar rcs $(library_target) $(cpp_objects_tmp)
	@mv $(library_target) $@

test: dist/$(target)
	cd dist && ./$(target) --test

build-tests: dist/$(target)
	cd dist && ./$(target) --test --overwrite-results

clean:
	@echo -e "   RM      tmp"
	@rm -Rf tmp

	@echo -e "   RM      dist/$(library_include_c_target)"
	@rm -Rf dist/$(library_include_c_target)

	@echo -e "   RM      dist/$(library_include_cpp_target)"
	@rm -Rf dist/$(library_include_cpp_target)

	@echo -e "   RM      dist/$(target)"
	@rm -f dist/$(target)

	@echo -e "   RM      dist/$(library_target)"
	@rm -f dist/$(library_target)