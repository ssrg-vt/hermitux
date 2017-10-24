#!/usr/bin/perl

@arch=(alpha,arm,i386,mips,sparc,ppc);
@vpath=();

open FILE,"Makefile" || die;
open OUT,">sMakefile" || die;
while (<FILE>) {
  if (m/^VPATH=(.*)\n$/) {
    @vpath=split /:/,$1;
#    foreach $m (@vpath) { print "vpath: $m\n"; }
    s/^/# /;
  } elsif (m/\$\(patsubst ([^,]*),([^,]*),\$\(wildcard ([^\/]*)\/([^\)\/]*)\)\)/) {
    my @list=();
    my ($src,$dest,$dir,$glob) = ($1,$2,$3,$4);
    foreach $i (glob("$dir/$glob")) {
# match "libcruft/%.c" to "libcruft/fnord.c"
      my $tmp=$i;
      my $j=$src;
#      print "comparing $i and $src\n";
      while (length($j) && (substr($j,0,1) eq substr($tmp,0,1))) {
	substr($j,0,1)="";
	substr($tmp,0,1)="";
      }
      while (length($j) && (substr($j,length($j)-1,1) eq substr($tmp,length($tmp)-1,1))) {
	chop $j;
	chop $tmp;
      }
      my $foo=$dest;
      die "unable to match" unless $j eq "%";
      $foo =~ s/\%/$tmp/;
      push @list,$foo;
#      print "foo = $foo\n";
      $total{$foo}="?";
#      print "$i, $src, $dest -> $foo\n";
    }
    s/\$\(patsubst.*\)\)/join(' ',@list)/e;
#    print "$a - $b - $c\n";
  }
  print OUT $_;
}
close FILE;

print OUT "\n\ninclude sMakefile.\$(ARCH)\n";
close OUT;
foreach $i (@arch) {
  my %dep;
  my @archvpath=@vpath;
  open SRC,"$i/Makefile.add" || die "no $i/Makefile.add?!\n";
  open OUT,">sMakefile.$i" || die "could not create sMakefile.$i\n";
  while (<SRC>) {
    if (m/^VPATH:?=(.*)\n$/) {
#      print;
      foreach $m (split /:/,$1) {
	unshift @archvpath,$m unless ($m eq "\$(VPATH)");
      }
      s/^/# /;
    }
  }
  close SRC;
#  foreach $m (@archvpath) { print "vpath: $m\n"; }
  foreach $I (keys %total) {
    my $j=$I;
    $j =~ s/^\$\(OBJDIR\)\///;
    $j =~ s/\.o$//;
#    print STDERR "looking for $j...\n";
    foreach $k (@archvpath) {
#      print STDERR "$k/$j.S\n";
      if (-f "$k/$j.S") {
	print OUT "$i-bin/$j.o: $k/$j.S\n";
	last;
      }
    }
  }
  close OUT;
}
