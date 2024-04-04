#!/usr/bin/perl
use CGI;
my $cgi = CGI->new();
print $cgi->header;


print "<h1>Environment</h1>";
foreach (sort keys %ENV) {
   print "<b>$_</b>: $ENV{$_}<br>\n";
}


