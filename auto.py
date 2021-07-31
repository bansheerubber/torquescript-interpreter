#!/bin/env python3

import sys
import pyinotify
import subprocess
import os
from time import time, sleep

class EventHandler(pyinotify.ProcessEvent):
	def process_IN_CREATE(self, event):
		make()
	
	def process_IN_MODIFY(self, event):
		make()

last_compile = 0
target = f"{sys.argv[1]} " if len(sys.argv) == 2 else ""

def make():
	global last_compile
	global target
	if time() - last_compile > 1:
		process = subprocess.Popen(["make", target, "-j", "8"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		error = False
		for line in process.stderr:
			print(line.decode("utf-8").strip())
			error = True
		
		for line in process.stdout:
			print(line.decode("utf-8").strip())

		if error:
			print("\n\u001b[31mERROR IN MAKE\u001b[0m")
		
		print("---------------------------------------------------------------------------------")

		last_compile = time()

os.system("make clean")
os.system(f"make {target}-j 8")
print("---------------------------------------------------------------------------------")

handler = EventHandler()
watch_manager = pyinotify.WatchManager()
notifier = pyinotify.Notifier(watch_manager, handler)
wdd = watch_manager.add_watch("./src/", pyinotify.IN_CREATE | pyinotify.IN_MODIFY, rec=True)
notifier.loop()