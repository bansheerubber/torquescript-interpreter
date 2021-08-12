import os
import pathlib
import re
from shutil import copyfile
import subprocess
import sys

total_lines = 0

def write_file(filename, contents):
	file = open(filename, "w", encoding='utf-8')
	for line in contents:
		file.write(line)
	file.close()

def preprocess(filename, contents, directory = None):
	global total_lines
	
	if "include" not in filename:
		total_lines = total_lines + len(contents)
	
	pattern = r'^[\s]*?##(.+)$'
	new_contents = []
	for line in contents:
		if match := re.match(pattern, line):
			if directory == None:
				directory = pathlib.Path(filename).parent

			command = match.group(1).strip()

			if ".py" in command:
				command = f"cd {directory} && python3 {command}"
			else:
				command = f"cd {directory} && {command}"

			process = os.popen(command)
			output = process.read()
			if process.close() != None:
				print("Encountered preprocessor error:", file=sys.stderr)
				print(output, file=sys.stderr)
				exit(1)
			else:
				new_contents.append(output)
		else:
			new_contents.append(line)
	return new_contents

if __name__ == "__main__":
	include_file = open("./src/lib/libSymbols.h")
	functions = []
	for line in include_file:
		if match := re.search(r'[\w]+(?=\()', line):
			functions.append(match.group(0))

	include_file.close()

	# generate the map file
	global_symbols = ";".join(functions)
	map_contents = """torquescript {
		global: %%;
		local: *;
	};""".replace("%%", global_symbols)

	map_file = open("libtorquescript.map", "w")
	map_file.write(map_contents)
	map_file.close()

	# generate the include files
	os.makedirs("./dist/include.cpp", exist_ok=True)
	copyfile("./src/lib/libSymbols.h", "./dist/include.cpp/ts.h")

	os.makedirs("./dist/include.c", exist_ok=True)
	include_c_source = open("./src/lib/libSymbols.h")
	include_c_destination = open("./dist/include.c/ts.h", "w")
	include_c_lines = []
	for line in include_c_source:
		if 'extern "C"' not in line and line.strip() != "}":
			include_c_lines.append(line.strip() + "\n")

	include_c_destination.writelines(include_c_lines)
	include_c_destination.close()
	include_c_source.close()

	for root, subdirs, files in os.walk("./src"):
		files = [f"{root}/{file}" for file in files]
		for file in files:
			file = file.replace("\\", "/")
			file_object = pathlib.Path(file)
			tmp_file = file.replace("./src/", "./tmp/")
			tmp_file_object = pathlib.Path(tmp_file)
			
			extension = file_object.suffix
			parent = file_object.parent
			tmp_parent = tmp_file_object.parent

			if extension == ".cc" or extension == ".h":
				opened_file = open(file_object, "r", encoding='utf-8')
				file_contents = opened_file.readlines()
				opened_file.close()
				
				if os.path.exists(tmp_file):
					src_time = file_object.stat().st_mtime
					tmp_time = tmp_file_object.stat().st_mtime

					if src_time > tmp_time: # recopy file if source file is newer than tmp file
						write_file(tmp_file, preprocess(file, file_contents))
				else:
					os.makedirs(tmp_parent, exist_ok=True)
					write_file(tmp_file, preprocess(file, file_contents))