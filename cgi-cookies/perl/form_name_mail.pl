#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# Create a new CGI object
my $cgi = CGI->new;

# Check if form is submitted
if ($cgi->param('submit')) {
    # Retrieve the form data
    my $name = $cgi->param('name');
    my $email = $cgi->param('email');
    
    # Process the form data
    # ...
    
    # Generate the HTML response
    # print $cgi->header;
    print $cgi->start_html('Form Submission');
    print $cgi->h1('Thank you for submitting the form!');
    print "<p>Name: $name</p>";
    print "<p>Email: $email</p>";
    print $cgi->end_html;
} else {
    # Generate the HTML form
    # print $cgi->header;
    print $cgi->start_html('Test Form');
    print $cgi->h1('Test Form');
    print $cgi->start_form(-method => 'POST', -action => '');
    print $cgi->label('Name:');
    print $cgi->textfield(-name => 'name', -required => 'true');
    print "<br><br>";
    print $cgi->label('Email:');
    print $cgi->textfield(-name => 'email', -required => 'true');
    print "<br><br>";
    print $cgi->submit(-name => 'submit', -value => 'Submit');
    print $cgi->end_form;
    print $cgi->end_html;
}
