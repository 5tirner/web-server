
use CGI;
my $cgi = CGI->new();

print "<h2> GET form for calandar </h2>";
print $cgi->start_form(
    -method => "get",
    -action => "calandar.pl"
);

# Add form inputs for month and year
print $cgi->textfield(-name => 'month', -size => 2);
print " / ";
print $cgi->textfield(-name => 'year', -size => 4);

# Add submit button
print $cgi->submit(-value => 'Submit');

print $cgi->end_form;


print "<h2> POST form calandar </h2>";
print $cgi->start_form(
    -method => "post",
    -action => "calandar.pl"
);

# Add form inputs for month and year
print $cgi->textfield(-name => 'month', -size => 2);
print " / ";
print $cgi->textfield(-name => 'year', -size => 4);

# Add submit button
print $cgi->submit(-value => 'Submit');

print $cgi->end_form;

print "<h2> GET form for env </h2>";
print $cgi->start_form(
    -method => "get",
    -action => "env.pl"
);

# Add form inputs for month and year
print $cgi->textfield(-name => 'month', -size => 2);
print " / ";
print $cgi->textfield(-name => 'year', -size => 4);

# Add submit button
print $cgi->submit(-value => 'Submit');

print $cgi->end_form;

print "<h2> POST form env </h2>";
print $cgi->start_form(
    -method => "post",
    -action => "env.pl"
);

# Add form inputs for month and year
print $cgi->textfield(-name => 'month', -size => 2);
print " / ";
print $cgi->textfield(-name => 'year', -size => 4);

# Add submit button
print $cgi->submit(-value => 'Submit');

print $cgi->end_form;
