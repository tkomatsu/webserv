# coding: UTF-8
import requests

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
    BOLD      = '\038[1m'
    UNDERLINE = '\033[4m'
    INVISIBLE = '\033[08m'
    REVERCE   = '\033[07m'

class Case():
	def __init__(self, method, uri, params=None, headers=None, data=None):
		method = method.lower()
		global index
		index += 1

		if method == "get":
			self.r = requests.get(uri, params=params, headers=headers)
		elif method == "post":
			self.r = requests.post(uri, params=params, headers=headers, data=data)
		elif method == "delete":
			self.r = requests.delete(uri, params=params, headers=headers)

		self.r.encoding = self.r.apparent_encoding
		print("\n" + str(index) + ". " + method, uri, params, headers, data)

	# status_codeが期待と同じか
	def status_code(self, expected):
		if expected == self.r.status_code:
			print(Color.GREEN + "status_code OK!" + Color.END)
		else:
			print(Color.RED + "status_code KO!" + Color.END)
			exit(1)
	
	# bodyが期待と同じか
	def body(self, expected):
		if expected == self.r.text:
			print(Color.GREEN + "body OK!" + Color.END)
		else:
			print(Color.RED + "body KO!" + Color.END)
			exit(1)
	
	# bodyが期待の文字列を含むか
	def body_has(self, expected):
		if expected in self.r.text:
			print(Color.GREEN + "body_has OK!" + Color.END)
		else:
			print(Color.RED + "body_has KO!" + Color.END)
			exit(1)
	
	# headersに期待のkeyと同じ かつ 期待のvalueが同じものがあるか
	def header(self, expected_key, expected_val):
		if expected_key in self.r.headers:
			if self.r.headers[expected_key] == expected_val:
				print(Color.GREEN + "header OK!" + Color.END)
		else:
			print(Color.RED + "header KO!" + Color.END)
			exit(1)

	# headerに期待のkey含まれるか
	def header_has(self, expected):
		if expected in self.r.headers:
			print(Color.GREEN + "header_has OK!" + Color.END)
		else:
			print(Color.RED + "header_has KO!" + Color.END)
			exit(1)

################################################################

case = Case("get", "http://localhost:4200")
case.status_code(200)
case.body_has("mcgee")
case.header_has("Content-Length")
case.header("Content-Type", "text/html")

################################################################

payload = {"key1": "val1", "key2": "val2"}
case = Case("get", "http://localhost:4200", params=payload)
case.status_code(200)
case.body_has("mcgee")
case.header_has("Content-Length")
case.header("Content-Type", "text/html")


################################################################

payload = {"key1": "val1", "key2": "val2"}
case = Case("get", "http://localhost:4200", headers=payload)
case.status_code(200)
case.body_has("mcgee")
case.header_has("Content-Length")
case.header("Content-Type", "text/html")


################################################################

payload = {"key1": "val1", "key2": "val2"}
case = Case("post", "http://localhost:4200", data=payload)
case.status_code(200)
case.body_has("mcgee")
case.header_has("Content-Length")
case.header("Content-Type", "text/html")