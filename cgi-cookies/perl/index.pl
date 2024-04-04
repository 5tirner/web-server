#!/bin/perl
#
#  PROGRAM:	cookie-set.cgi
#
#  PURPOSE:	Demonstrate how to SET a cookie through a Perl/CGI program
#		uisng the CGI.pm module.
#
#  Copyright DevDaily Interactive, Inc., 1998. All Rights Reserved.
#

#------------------------------#
#  1. Create a new CGI object  #
#------------------------------#

use CGI;
$query = new CGI;


#------------------------------------------------------------------------------#
#  2. Create the desired cookie using the cookie() method.
#     Do this before calling the header() method, because the cookie must be
#     incorporated into the HTTP header.
#
#  Note: Valid parameters for the cookie() call include:
#  -----------------------------------------------------
#
#	-domain		a partial or complete domain name for which the
#				cookie is valid. Like '.devdaily.com'
#				for the entire domain (www.devdaily.com,
#				smtp.devdaily.com, etc.), or 'www.devdaily.com'
#				to refer to one server.
#	-expires	(optional) The expiration date for this cookie.
#				+60s	60 seconds from now
#				+20m	20 minutes from now
#				+5h	5 hours from now
#				+1d	1 day from now
#				-3d	3 days ago
#				now	immediately
#				+6M	6 months from now
#				+1y	1 year from now
#				Monday, 28-Dec-98 12:30:00 GMT
#					expire at this specific date & time
#	-name		the name of the cookie (req'd)
#	-path		(optional) If you specify this attribute, the 
#				browser will check it against your script's URL
#				before returning the cookie.
#				The default is set to '/'.
#	-secure		(optional) The cookie will only be sent if the CGI
#				request is occurring on a secure channel.
#	-value		(req'd) The value of your cookie. This can be a 
#				scalar value, array reference, or hash
#				reference.
#------------------------------------------------------------------------------#

$cookie = $query->cookie(-name=>'MY_COOKIE',
			 -value=>'BEST_COOKIE=chocolatechip',
			 -expires=>'+4h',
			 -path=>'/');


#--------------------------------------------------------------#
#  3. Create the HTTP header and print the doctype statement.  #
#--------------------------------------------------------------#

print $query->header(-cookie=>$cookie);


#--------------------------------------------------#
#  4. Give the page a title and a simple header.   #
#     (Not really needed in this simple example.)  #
#--------------------------------------------------#

print $query->start_html('My cookie-set.cgi program');
print $query->h3('The cookie has been set');


#-------------------------#
#  5. End the HTML page.  #
#-------------------------#

print $query->end_html;



