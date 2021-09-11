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
target = sys.argv[1] if len(sys.argv) == 2 else ""

def make():
	global last_compile
	global target
	if time() - last_compile > 1:
		if target:
			os.system(f"make {target} -j 8")
		else:
			os.system(f"make -j 8")
		
		print("---------------------------------------------------------------------------------")

		last_compile = time()

os.system("make clean")

if target:
	os.system(f"make {target} -j 8")
else:
	os.system(f"make -j 8")

print("---------------------------------------------------------------------------------")

handler = EventHandler()
watch_manager = pyinotify.WatchManager()
notifier = pyinotify.Notifier(watch_manager, handler)
wdd = watch_manager.add_watch("./src/", pyinotify.IN_CREATE | pyinotify.IN_MODIFY, rec=True)
notifier.loop()