#!/usr/bin/env python3
# coding: UTF-8
import subprocess
import time

config_name = ""

class Server():
	def run(self, conf=None):
		global config_name
		if conf:
			subprocess.Popen(('./webserv', "./conf/" + conf), stdout=subprocess.DEVNULL)
			config_name = conf
		else:
			subprocess.Popen(('./webserv'), stdout=subprocess.DEVNULL)
			config_name = "basic.conf"
		time.sleep(1)

	def stop(self):
		subprocess.run(('pkill', 'webserv'))
		
