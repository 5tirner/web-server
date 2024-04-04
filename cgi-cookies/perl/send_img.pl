#!/usr/bin/env perl
use strict;
use warnings;
use CGI;
my $cgi = CGI->new();
print $cgi->header( -type => 'image/svg' );
open my $img, '<', 'file-1453.svg';
while (<$img>) { print }
