![sequence dialog](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/tkomatsu/webserv/uml/docs/webserb.pu?token=ALGGLOOAFDRRCAADZ3Q7IDTBBKEF4)

```
$ make
$ ./webserv
```

別プロセスにて…

```
$ curl localhost:4200
$ curl localhost:4201
$ curl localhost:4202
$ curl localhost:4200 localhost:4200 -v
$ curl localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202 localhost:4200 localhost:4201 localhost:4202
```
