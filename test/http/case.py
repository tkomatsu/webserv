#!/usr/bin/env python3
# coding: UTF-8

import requests
import subprocess
import time
import run
import re

index = 0

class Color:
    BLACK     = '\033[30m'
    RED       = '\033[31m'
    GREEN     = '\033[32m'
    YELLOW    = '\033[33m'
    BLUE      = '\033[34m'
    PURPLE    = '\033[35m'
    CYAN      = '\033[36m'
    WHITE     = '\033[37m'
    END       = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'
    INVISIBLE = '\033[08m'
    REVERCE   = '\033[07m'


class Case():
	def __init__(self, name, method, uri, headers=None, body=None):
		method = method.lower()
		global index
		index += 1

		try:
			if method == "get":
				self.r = requests.get(uri, headers=headers, allow_redirects=False)
			elif method == "post":
				self.r = requests.post(uri, headers=headers, data=body, allow_redirects=False)
			elif method == "delete":
				self.r = requests.delete(uri, headers=headers, allow_redirects=False)

			self.r.encoding = self.r.apparent_encoding

			print("\n" + Color.UNDERLINE + str(index) + ". " + run.config_name + ": " + name + Color.END + "\n\n" + method.upper(), self.r.request.path_url, "HTTP/1.1")
			if self.r.request.headers:
				for header in self.r.request.headers:
					print(header + ": " + self.r.request.headers[header])
			if self.r.request.body:
				print("\n" + self.r.request.body)
			print()
		except:
			print(Color.RED + "ERROR in requests" + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)

	# status_codeが期待と同じか
	def status_code(self, expected):
		if expected == self.r.status_code:
			print(Color.GREEN + "status_code is " + str(expected) + Color.END)
		else:
			print(Color.RED + "status_code is " + str(self.r.status_code) + "\nBUT expected was " + str(expected) + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)

	# bodyが期待の正規表現と同じか
	def body_is(self, expected):
		if re.match(expected, self.r.text):
			print(Color.GREEN + "body is " + expected + Color.END)
		else:
			print(Color.RED + "body DIDN'T match the regex, " + expected + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)

	# bodyが期待の文字列を含むか
	def body_has(self, expected):
		if expected in self.r.text:
			print(Color.GREEN + "body has \"" + expected + "\"" + Color.END)
		else:
			print(Color.RED + "body DIDN'T have \"" + expected + "\"" + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)

	# headersに期待のkeyと同じ かつ 期待のvalueが同じものがあるか
	def header(self, expected_key, expected_val):
		if expected_key in self.r.headers and self.r.headers[expected_key] == expected_val:
			print(Color.GREEN + "header has " + expected_key + ": " + expected_val + Color.END)
		else:
			print(Color.RED + "header DIDN'T have " + expected_key + ": " + expected_val + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)

	# headerに期待のkey含まれるか
	def header_has(self, expected):
		if expected in self.r.headers:
			print(Color.GREEN + "header has the key, " + expected + Color.END)
		else:
			print(Color.RED + "header DIDN'T have the key, " + expected + Color.END)
			subprocess.run(('pkill', 'webserv'))
			exit(1)
