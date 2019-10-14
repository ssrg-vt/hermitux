#!/usr/bin/env perl
BEGIN {
	# add current source dir to the include-path
	# we need this for make distcheck
	(my $srcdir = $0) =~ s,/[^/]+$,/,;
	unshift @INC, $srcdir;
}

use strict;
use Test::More tests => 46;
use LightyTest;

my $tf = LightyTest->new();

my $t;

SKIP: {
	skip "no php binary found", 31 unless $LightyTest::HAVE_PHP;

	$tf->{CONFIGFILE} = 'fastcgi-10.conf';
	ok($tf->start_proc == 0, "Starting lighttpd") or goto cleanup;

	$t->{REQUEST} = ( <<EOF
GET /phpinfo.php HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'valid request');

	$t->{REQUEST}  = ( <<EOF
GET /phpinfofoobar.php HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404 } ];
	ok($tf->handle_http($t) == 0, 'file not found');

	$t->{REQUEST}  = ( <<EOF
GET /go/ HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'index-file handling');

	$t->{REQUEST}  = ( <<EOF
GET /redirect.php HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 302, 'Location' => 'http://www.example.org:2048/' } ];
	ok($tf->handle_http($t) == 0, 'Status + Location via FastCGI');

	$t->{REQUEST}  = ( <<EOF
GET /redirect.php/ HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 302, 'Location' => 'http://www.example.org:2048/' } ];
	ok($tf->handle_http($t) == 0, 'Trailing slash as path-info (#1989: workaround broken operating systems)');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=PHP_SELF HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, '$_SERVER["PHP_SELF"]');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php/foo?env=SCRIPT_NAME HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/get-server-env.php' } ];
	ok($tf->handle_http($t) == 0, '$_SERVER["SCRIPT_NAME"]');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php/foo?env=PATH_INFO HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE}  = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/foo' } ];
	ok($tf->handle_http($t) == 0, '$_SERVER["PATH_INFO"]');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=SERVER_NAME HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'www.example.org' } ];
	ok($tf->handle_http($t) == 0, 'SERVER_NAME');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=SERVER_NAME HTTP/1.0
Host: foo.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'www.example.org' } ];
	ok($tf->handle_http($t) == 0, 'SERVER_NAME');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=SERVER_NAME HTTP/1.0
Host: vvv.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'www.example.org' } ];
	ok($tf->handle_http($t) == 0, 'SERVER_NAME');

	$t->{REQUEST}  = ( <<EOF
GET /cgi.php/abc HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'PATHINFO');

    if ($^O ne "cygwin") {
	$t->{REQUEST}  = ( <<EOF
GET /cgi.php%20%20%20 HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404 } ];
	ok($tf->handle_http($t) == 0, 'No source retrieval');
    } else {
	ok(1, 'No source retrieval; skipped on cygwin; see response.c');
    }

	$t->{REQUEST}  = ( <<EOF
GET /www/abc/def HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404 } ];
	ok($tf->handle_http($t) == 0, 'PATHINFO on a directory');

	$t->{REQUEST}  = ( <<EOF
GET /indexfile/ HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/indexfile/index.php' } ];
	ok($tf->handle_http($t) == 0, 'PHP_SELF + Indexfile, Bug #3');

	$t->{REQUEST}  = ( <<EOF
GET /prefix.fcgi?var=SCRIPT_NAME HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/prefix.fcgi' } ];
	ok($tf->handle_http($t) == 0, 'PATH_INFO, check-local off');

	$t->{REQUEST}  = ( <<EOF
GET /prefix.fcgi/foo/bar?var=SCRIPT_NAME HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/prefix.fcgi' } ];
	ok($tf->handle_http($t) == 0, 'PATH_INFO, check-local off');

	$t->{REQUEST}  = ( <<EOF
GET /prefix.fcgi/foo/bar?var=PATH_INFO HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/foo/bar' } ];
	ok($tf->handle_http($t) == 0, 'PATH_INFO, check-local off');

	$t->{REQUEST}  = ( <<EOF
GET /sendfile.php?range=0- HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'Content-Length' => 4348 } ];
	ok($tf->handle_http($t) == 0, 'X-Sendfile2');

	$t->{REQUEST}  = ( <<EOF
GET /sendfile.php?range=0-4&range2=5- HTTP/1.0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'Content-Length' => 4348 } ];
	ok($tf->handle_http($t) == 0, 'X-Sendfile2');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=REMOTE_USER HTTP/1.0
Host: auth.example.org
Authorization: Basic ZGVzOmRlcw==
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'des' } ];
	ok($tf->handle_http($t) == 0, '$_SERVER["REMOTE_USER"]');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=AUTH_TYPE HTTP/1.0
Host: auth.example.org
Authorization: Basic ZGVzOmRlcw==
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'Basic' } ];
	ok($tf->handle_http($t) == 0, '$_SERVER["AUTH_TYPE"]');

	$t->{REQUEST}  = ( <<EOF
GET /get-server-env.php?env=SERVER_NAME HTTP/1.0
Host: zzz.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'zzz.example.org' } ];
	ok($tf->handle_http($t) == 0, 'FastCGI + Host');

	$t->{REQUEST}  = ( <<EOF
GET http://zzz.example.org/get-server-env.php?env=SERVER_NAME HTTP/1.0
Host: aaa.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'zzz.example.org' } ];
	ok($tf->handle_http($t) == 0, 'SERVER_NAME (absolute url in request line)');

	$t->{REQUEST}  = ( <<EOF
GET /indexfile/ HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/indexfile/index.php' } ];
	ok($tf->handle_http($t) == 0, 'Bug #6');

	$t->{REQUEST}  = ( <<EOF
POST /indexfile/abc HTTP/1.0
Host: www.example.org
Content-Length: 0
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 404, 'HTTP-Content' => '/indexfile/return-404.php' } ];
	ok($tf->handle_http($t) == 0, 'Bug #12');

	$t->{REQUEST}  = ( <<EOF
GET /indexfile/index.php HTTP/1.0
Host: bin-env.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'FastCGI + local spawning');

	$t->{REQUEST} = ( <<EOF
HEAD /indexfile/index.php HTTP/1.0
Host: bin-env.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, '-Content-Length' => '0' } ];
	# Of course a valid content-length != 0 would be ok, but we assume for now that such one is not generated.
	ok($tf->handle_http($t) == 0, 'Check for buggy content length with HEAD');

	$t->{REQUEST}  = ( <<EOF
GET /get-env.php?env=MAIL HTTP/1.0
Host: bin-env.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 , 'HTTP-Content' => '' } ];
	ok($tf->handle_http($t) == 0, 'FastCGI + bin-copy-environment');

	ok($tf->stop_proc == 0, "Stopping lighttpd");
}

SKIP: {
	skip "no fcgi-auth, fcgi-responder found", 15
	  unless (-x $tf->{BASEDIR}."/tests/fcgi-auth"      || -x $tf->{BASEDIR}."/tests/fcgi-auth.exe")
	      && (-x $tf->{BASEDIR}."/tests/fcgi-responder" || -x $tf->{BASEDIR}."/tests/fcgi-responder.exe");

	$tf->{CONFIGFILE} = 'fastcgi-responder.conf';
	ok($tf->start_proc == 0, "Starting lighttpd with $tf->{CONFIGFILE}") or die();
	$t->{REQUEST}  = ( <<EOF
GET /index.html?ok HTTP/1.0
Host: auth.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'FastCGI - Auth');

	$t->{REQUEST}  = ( <<EOF
GET /index.html?fail HTTP/1.0
Host: auth.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 403 } ];
	ok($tf->handle_http($t) == 0, 'FastCGI - Auth');

	$t->{REQUEST}  = ( <<EOF
GET /expire/access.txt?ok HTTP/1.0
Host: auth.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200 } ];
	ok($tf->handle_http($t) == 0, 'FastCGI - Auth in subdirectory');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?varfail HTTP/1.0
Host: auth.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 403 } ];
	ok($tf->handle_http($t) == 0, 'FastCGI - Auth Fail with FastCGI responder afterwards');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?var HTTP/1.0
Host: auth.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'LighttpdTestContent' } ];
	ok($tf->handle_http($t) == 0, 'FastCGI - Auth Success with Variable- to Env expansion');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?lf HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'line-ending \n\n');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?crlf HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'line-ending \r\n\r\n');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?slow-lf HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'line-ending \n + \n');

	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?slow-crlf HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'line-ending \r\n + \r\n');

	$t->{REQUEST}  = ( <<EOF
GET /abc/def/ghi?path_info HTTP/1.0
Host: wsgi.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '/abc/def/ghi' } ];
	ok($tf->handle_http($t) == 0, 'PATH_INFO (wsgi)');

	$t->{REQUEST}  = ( <<EOF
GET /abc/def/ghi?script_name HTTP/1.0
Host: wsgi.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => '' } ];
	ok($tf->handle_http($t) == 0, 'SCRIPT_NAME (wsgi)');


	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?die-at-end HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'killing fastcgi and wait for restart');

	# (might take lighttpd 1 sec to detect backend exit)
	select(undef, undef, undef, .5);
	for (my $c = 2*20; $c && 0 == $tf->listening_on(10000); --$c) {
		select(undef, undef, undef, 0.05);
	}
	$t->{REQUEST}  = ( <<EOF
GET /index.fcgi?crlf HTTP/1.0
Host: www.example.org
EOF
 );
	$t->{RESPONSE} = [ { 'HTTP-Protocol' => 'HTTP/1.0', 'HTTP-Status' => 200, 'HTTP-Content' => 'test123' } ];
	ok($tf->handle_http($t) == 0, 'regular response of after restart');


	ok($tf->stop_proc == 0, "Stopping lighttpd");
}

exit 0;

cleanup: ;

die();
