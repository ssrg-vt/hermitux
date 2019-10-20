#!/usr/bin/env perl
BEGIN {
	# add current source dir to the include-path
	# we need this for make distcheck
	(my $srcdir = $0) =~ s,/[^/]+$,/,;
	unshift @INC, $srcdir;
}

use strict;
use IO::Socket;
use Test::More tests => 52;
use LightyTest;

my $tf = LightyTest->new();
my $t;

ok($tf->start_proc == 0, "Starting lighttpd") or die();

## Basic Request-Handling

$t->{REQUEST}  = ( <<EOF
GET /foobar HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404 } ];
ok($tf->handle_http($t) == 0, 'file not found');

$t->{REQUEST}  = ( <<EOF
GET /foobar?foobar HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404 } ];
ok($tf->handle_http($t) == 0, 'file not found + querystring');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain' } ];
ok($tf->handle_http($t) == 0, 'GET, content == 12345, mimetype text/plain');

$t->{REQUEST}  = ( <<EOF
GET /12345.html HTTP/1.0
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/html' } ];
ok($tf->handle_http($t) == 0, 'GET, content == 12345, mimetype text/html');

$t->{REQUEST}  = ( <<EOF
GET /dummyfile.bla HTTP/1.0
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'application/octet-stream' } ];
ok($tf->handle_http($t) == 0, 'GET, content == 12345, mimetype application/octet-stream');


$t->{REQUEST}  = ( <<EOF
POST / HTTP/1.0
Content-type: application/x-www-form-urlencoded
Content-length: 0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST request, empty request-body');

$t->{REQUEST}  = ( <<EOF
HEAD / HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '-HTTP-Content' => ''} ];
ok($tf->handle_http($t) == 0, 'HEAD request, no content');

$t->{REQUEST}  = ( <<EOF
HEAD /12345.html HTTP/1.0
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '-HTTP-Content' => '', 'Content-Type' => 'text/html', 'Content-Length' => '6'} ];
ok($tf->handle_http($t) == 0, 'HEAD request, mimetype text/html, content-length');

$t->{REQUEST}  = ( <<EOF
HEAD http://123.example.org/12345.html HTTP/1.1
Connection: close
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, '-HTTP-Content' => '', 'Content-Type' => 'text/html', 'Content-Length' => '6'} ];
ok($tf->handle_http($t) == 0, 'Hostname in first line, HTTP/1.1');

$t->{REQUEST}  = ( <<EOF
HEAD https://123.example.org/12345.html HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '-HTTP-Content' => '', 'Content-Type' => 'text/html', 'Content-Length' => '6'} ];
ok($tf->handle_http($t) == 0, 'Hostname in first line as https url');

$t->{REQUEST}  = ( <<EOF
HEAD /foobar?foobar HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404, '-HTTP-Content' => '' } ];
ok($tf->handle_http($t) == 0, 'HEAD request, file-not-found, query-string');

# (expect 200 OK instead of 100 Continue since request body sent with request)
# (if we waited to send request body, would expect 100 Continue, first)
$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Content-Length: 4
Expect: 100-continue

123
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'Continue, Expect');

# note Transfer-Encoding: chunked tests will fail with 411 Length Required if
#   server.stream-request-body != 0 in lighttpd.conf
$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

a
0123456789
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked, lc hex');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

A
0123456789
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked, uc hex');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

10
0123456789abcdef
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked, two hex');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

a
0123456789
0
Test-Trailer: testing

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked, with trailer');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

a; comment
0123456789
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked, chunked header comment');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

az
0123456789
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 400 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked; bad chunked header');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

a
0123456789xxxxxxxx
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 400 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked; mismatch chunked header size and chunked data size');

$t->{REQUEST}  = ( <<EOF
POST /get-post-len.pl HTTP/1.1
Host: www.example.org
Connection: close
Content-Type: application/x-www-form-urlencoded
Transfer-Encoding: chunked

a ; xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
0123456789
0

EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 400 } ];
ok($tf->handle_http($t) == 0, 'POST via Transfer-Encoding: chunked; chunked header too long');

## ranges

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=0-3
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206, 'HTTP-Content' => '1234' } ];
ok($tf->handle_http($t) == 0, 'GET, Range 0-3');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=-3
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206, 'HTTP-Content' => '45'."\n" } ];
ok($tf->handle_http($t) == 0, 'GET, Range -3');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=3-
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206, 'HTTP-Content' => '45'."\n" } ];
ok($tf->handle_http($t) == 0, 'GET, Range 3-');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=0-1,3-4
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206, 'HTTP-Content' => <<EOF
\r
--fkj49sn38dcn3\r
Content-Range: bytes 0-1/6\r
Content-Type: text/plain\r
\r
12\r
--fkj49sn38dcn3\r
Content-Range: bytes 3-4/6\r
Content-Type: text/plain\r
\r
45\r
--fkj49sn38dcn3--\r
EOF
 } ];
ok($tf->handle_http($t) == 0, 'GET, Range 0-1,3-4');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=0--
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'GET, Range 0--');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=-2-3
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'GET, Range -2-3');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=-0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 416, 'HTTP-Content' => <<EOF
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
 <head>
  <title>416 Requested Range Not Satisfiable</title>
 </head>
 <body>
  <h1>416 Requested Range Not Satisfiable</h1>
 </body>
</html>
EOF
 } ];
ok($tf->handle_http($t) == 0, 'GET, Range -0');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=25-
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 416, 'HTTP-Content' => <<EOF
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
 <head>
  <title>416 Requested Range Not Satisfiable</title>
 </head>
 <body>
  <h1>416 Requested Range Not Satisfiable</h1>
 </body>
</html>
EOF
 } ];

ok($tf->handle_http($t) == 0, 'GET, Range start out of range');


$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
Hsgfsdjf: asdfhdf
hdhd: shdfhfdasd
hfhr: jfghsdfg
jfuuehdmn: sfdgjfdg
jvcbzufdg: sgfdfg
hrnvcnd: jfjdfg
jfusfdngmd: gfjgfdusdfg
nfj: jgfdjdfg
jfue: jfdfdg
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'larger headers');


$t->{REQUEST}  = ( <<EOF
GET /range.pdf HTTP/1.0
Range: bytes=0-
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'GET, Range with range-requests-disabled');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: 0
Range: bytes=0-3
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => "12345\n" } ];
ok($tf->handle_http($t) == 0, 'GET, Range invalid range-unit (first)');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
Range: bytes=0-3
Range: 0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206 } ];
ok($tf->handle_http($t) == 0, 'GET, Range ignore invalid range (second)');

$t->{REQUEST}  = ( <<EOF
OPTIONS / HTTP/1.0
Content-Length: 4

1234
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'OPTIONS with Content-Length');

$t->{REQUEST}  = ( <<EOF
OPTIONS rtsp://221.192.134.146:80 RTSP/1.1
Host: 221.192.134.146:80
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 400 } ];
ok($tf->handle_http($t) == 0, 'OPTIONS for RTSP');

$t->{REQUEST}  = ( <<EOF
GET /index.html HTTP/1.0
If-Modified-Since: Sun, 01 Jan 2036 00:00:02 GMT
If-Modified-Since: Sun, 01 Jan 2036 00:00:02 GMT
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Duplicate If-Mod-Since, with equal timestamps');

$t->{REQUEST}  = ( "GET / HTTP/1.0\r\nIf-Modified-Since: \r\n\r\n" );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'empty If-Modified-Since');

$t->{REQUEST}  = ( "GET / HTTP/1.0\r\nIf-Modified-Since: foobar\r\n\r\n" );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'broken If-Modified-Since');

$t->{REQUEST}  = ( "GET / HTTP/1.0\r\nIf-Modified-Since: this string is too long to be a valid timestamp\r\n\r\n" );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'broken If-Modified-Since');


$t->{REQUEST}  = ( <<EOF
GET /index.html HTTP/1.0
If-Modified-Since2: Sun, 01 Jan 2036 00:00:03 GMT
If-Modified-Since: Sun, 01 Jan 2036 00:00:02 GMT
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Similar Headers (bug #1287)');

$t->{REQUEST}  = ( <<EOF
GET /index.html HTTP/1.0
If-Modified-Since: Sun, 01 Jan 2036 00:00:02 GMT
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304, 'Content-Type' => 'text/html' } ];
ok($tf->handle_http($t) == 0, 'If-Modified-Since');

$t->{REQUEST}  = ( <<EOF
GET /index.html HTTP/1.0
If-Modified-Since: Sun, 01 Jan 2036 00:00:02 GMT
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304, '-Content-Length' => '' } ];
ok($tf->handle_http($t) == 0, 'Status 304 has no Content-Length (#1002)');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.0
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain' } ];
$t->{SLOWREQUEST} = 1;
ok($tf->handle_http($t) == 0, 'GET, slow \\r\\n\\r\\n (#2105)');
undef $t->{SLOWREQUEST};

print "\nPathinfo for static files\n";
$t->{REQUEST}  = ( <<EOF
GET /image.jpg/index.php HTTP/1.0
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'Content-Type' => 'image/jpeg' } ];
ok($tf->handle_http($t) == 0, 'static file accepting pathinfo by default');

$t->{REQUEST}  = ( <<EOF
GET /image.jpg/index.php HTTP/1.0
Host: zzz.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 403 } ];
ok($tf->handle_http($t) == 0, 'static file with forbidden pathinfo');


print "\nConnection header\n";
$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection  : close
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, spaces before ":"');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection: ,close
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, leading comma');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection: close,,TE
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, no value between two commas');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection: close, ,TE
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, space between two commas');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection: close,
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, comma after value');

$t->{REQUEST}  = ( <<EOF
GET /12345.txt HTTP/1.1
Connection: close, 
Host: 123.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.1', 'HTTP-Status' => 200, 'HTTP-Content' => '12345'."\n", 'Content-Type' => 'text/plain', 'Connection' => 'close' } ];
ok($tf->handle_http($t) == 0, 'Connection-header, comma and space after value');

ok($tf->stop_proc == 0, "Stopping lighttpd");

