#!/usr/bin/env python3
# coding: UTF-8
import subprocess
import time
from case import Color

config_name = ""

class Server():
	def run(self, conf=None):
		global config_name
		if conf:
			print("\n" + Color.BOLD + Color.REVERCE + "./webserv ./conf/" + conf + Color.END + Color.END)
			subprocess.Popen(('./webserv', "./conf/" + conf), stdout=subprocess.DEVNULL)
			config_name = conf
		else:
			print("\n" + Color.BOLD + Color.REVERCE + "./webserv" + Color.END + Color.END)
			subprocess.Popen(('./webserv'), stdout=subprocess.DEVNULL)
			config_name = "basic.conf"
		time.sleep(2)

	def stop(self):
		subprocess.run(('pkill', 'webserv'))

