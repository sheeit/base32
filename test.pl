#!/usr/bin/perl -T

use 5.006_001;
use strict;
use warnings;

use File::Temp qw(tempfile);

my $PROGNAME = './b32.exe';

my %tests = (
	q()       => q(),
	q(f)      => q(MY======),
	q(fo)     => q(MZXQ====),
	q(foo)    => q(MZXW6===),
	q(foob)   => q(MZXW6YQ=),
	q(fooba)  => q(MZXW6YTB),
	q(foobar) => q(MZXW6YTBOI======)
);

local $ENV{'PATH'} = join q(:), qw( /sbin /usr/sbin /bin /usr/bin /mingw64/bin
	/c/Windows/System32 /c/Windows /c/Windows/System32/Wbem
	/c/Windows/System32/WindowsPowerShell/v1.0
	/usr/bin/site_perl /usr/bin/vendor_perl /usr/bin/core_perl);

my $testnum;

TEST:
while (my ($test, $expected) = each %tests) {
	++$testnum;
	my ($fh, $filename) = tempfile();
	binmode $fh, q(:raw);
	print { $fh } $test;
	close $fh;
	chomp (my $got = qx($PROGNAME "$filename"));
	unlink $filename;
	$got =~ s/\r\z//gmsox;
	if ($got ne $expected) {
		print "Test $testnum failed:\n",
			"\tExpected <$expected>\n",
			"\tGot      <$got>\n";
		next TEST;
	}
	print "Test $testnum: OK.\n";
}
