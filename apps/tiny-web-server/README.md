A tiny web server in C
======================

I am reading
[Computer Systems: A Programmer's Perspective](http://csapp.cs.cmu.edu/).
It teachers me how to write a tiny web server in C.

I have written another
[tiny web server](https://github.com/shenfeng/nio-httpserver) in JAVA.

And another one [http-kit](https://github.com/http-kit/http-kit), http-kit is full featured, with websocket and async support

And few others on my github page.

Features
--------

1. Basic MIME mapping
2. Very basic directory listing
3. Low resource usage
4. [sendfile(2)](http://kernel.org/doc/man-pages/online/pages/man2/sendfile.2.html)
5. Support Accept-Ranges: bytes (for in browser MP4 playing)
6. Concurrency by pre-fork

Non-features
------------

1. No security check

Usage
-----

`tiny <port>`, opens a server in the current directory, port
default to 9999, just like `python -m SimpleHTTPServer`

I use it as a lightweight File Browser.


TODO
----

1. Write a epoll version


License
-------

The code is free to use under the terms of the MIT license.
