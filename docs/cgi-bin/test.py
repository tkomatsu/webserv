import os

print("Content-Type: text/html")
print()

print("<TITLE>CGI script output</TITLE>")
print("<H1>This is my first CGI script</H1>")
for param in os.environ.keys():
  print("<b>%20s</b>: %s</br>" % (param, os.environ[param]))