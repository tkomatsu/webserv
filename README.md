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
```
