# Webserv
[![Unit test](https://github.com/tkomatsu/webserv/actions/workflows/unit-test.yml/badge.svg)](https://github.com/tkomatsu/webserv/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## About The Project
nginx like web server made from scratch in c++

![demo](./docs/images/screen_capture.png)

## Getting Started
```sh
$ git clone https://github.com/tkomatsu/webserv
$ cd webserv
$ make
```

## Usage
```sh
$ ./webserv <config file>
```

## Running Test
### test all
```sh
$ make check
```

### unit test
```sh
$ make unittest
```

### http test
```sh
$ make httptest
```

## License
Distributed under the MIT License. See [LICENSE](LICENSE) for more information.
