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

def preprocess(filename, contents):
	global total_lines
	
	if "include" not in filename:
		total_lines = total_lines + len(contents)
	
	pattern = r'^[\s]*?##(.+)$'
	new_contents = []
	for line in contents:
		if match := re.match(pattern, line):
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