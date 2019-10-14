#!/usr/bin/env perl
BEGIN {
	# add current source dir to the include-path
	# we need this for make distcheck
	(my $srcdir = $0) =~ s,/[^/]+$,/,;
	unshift @INC, $srcdir;
}

use strict;
use IO::Socket;
use Test::More tests => 25;
use LightyTest;

my $tf = LightyTest->new();
my $t;

ok($tf->start_proc == 0, "Starting lighttpd") or die();

## check if If-Modified-Since, If-None-Match works

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-Modified-Since: Sun, 01 Jan 1970 00:00:01 GMT
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - old If-Modified-Since');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-Modified-Since: Sun, 01 Jan 1970 00:00:01 GMT; foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '+Last-Modified' => ''} ];
ok($tf->handle_http($t) == 0, 'Conditional GET - old If-Modified-Since, comment');

my $now = $t->{date};

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-Modified-Since: $now
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - new If-Modified-Since');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-Modified-Since: $now; foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - new If-Modified-Since, comment');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '+ETag' => ''} ];
ok($tf->handle_http($t) == 0, 'Conditional GET - old If-None-Match');

my $etag = $t->{etag};

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $etag
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - old If-None-Match');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $etag
If-Modified-Since: Sun, 01 Jan 1970 00:00:01 GMT; foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - ETag + old Last-Modified (which should be ignored)');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $etag
If-Modified-Since: $now; foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - ETag, Last-Modified + comment (which should be ignored)');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: Foo
If-Modified-Since: Sun, 01 Jan 1970 00:00:01 GMT; foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - old ETAG + old Last-Modified');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $etag
If-Modified-Since: $now foo
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - ETag + Last-Modified + overlong timestamp (which should be ignored)');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $etag
Host: etag.example.org
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'Conditional GET - ETag + disabled etags on server side');

###############

ok($etag =~ /^\"(.*)\"$/, "The server must quote ETags");

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $1
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'The client must send a quoted ETag');

$etag =~ /^(\".*)\"$/;
$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: $1
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'The ETag must be surrounded by quotes');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: *
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'An unquoted star matches any ETag');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: "*"
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'A quoted star is just a regular ETag');

{
	$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: W/$etag
EOF
	 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
	ok($tf->handle_http($t) == 0, 'A weak etag matches like a regular ETag for HEAD and GET');
}

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: W/$etag
Range: bytes=0-0
EOF
);
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 206, 'HTTP-Content' => '<' } ];
ok($tf->handle_http($t) == 0, 'A weak etag does not match for ranged requests');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: W/"12345"
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
ok($tf->handle_http($t) == 0, 'However, a weak ETag is not *');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: "12345", $etag
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Client sent a list of ETags, the second matches');

{
	$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: "12345", W/$etag
EOF
	 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
	ok($tf->handle_http($t) == 0, 'The second provided ETag matches weakly');
}

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: "12345",, ,,  ,  $etag
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Broken client did get around to sending good data');

$t->{REQUEST}  = ( <<EOF
GET / HTTP/1.0
If-None-Match: "1234", $etag, "brokentrailing
EOF
 );
$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 304 } ];
ok($tf->handle_http($t) == 0, 'Bad syntax *after* a matching ETag doesn\'t matter');

ok($tf->stop_proc == 0, "Stopping lighttpd");

