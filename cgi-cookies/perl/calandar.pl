#!/usr/bin/perl

use strict;
use warnings;
use Time::Local;

use CGI;

# Create CGI object
my $cgi = CGI->new;

# Get the month and year from the query parameters
my $month = $cgi->param('month') || (localtime)[4] + 1;
my $year = $cgi->param('year') || (localtime)[5] + 1900;

# Get the number of days in the specified month and year
my $days_in_month = days_in_month($month, $year);

# Get the weekday of the first day of the month
my $first_day_weekday = (localtime(timegm(0, 0, 0, 1, $month - 1, $year)))[6];

# Create the HTML page
print $cgi->header;
print $cgi->start_html(-title => "Calendar");
print $cgi->h1("Calendar - $month/$year");

# Create the table for the calendar
print $cgi->start_table;
print $cgi->Tr(
  $cgi->th("Sun"), $cgi->th("Mon"), $cgi->th("Tue"), $cgi->th("Wed"),
  $cgi->th("Thu"), $cgi->th("Fri"), $cgi->th("Sat")
);

# Calculate the number of rows needed for the calendar
my $num_rows = int(($days_in_month + $first_day_weekday) / 7) + 1;

# Generate the calendar
my $day = 1;
for (my $row = 1; $row <= $num_rows; $row++) {
  print $cgi->start_Tr;
  for (my $col = 0; $col < 7; $col++) {
    if (($row == 1 && $col < $first_day_weekday) || ($day > $days_in_month)) {
      print $cgi->td("");
    } else {
      print $cgi->td($day);
      $day++;
    }
  }
  print $cgi->end_Tr;
}

print $cgi->end_table;
print $cgi->end_html;

# Function to calculate the number of days in a month
sub days_in_month {
  my ($month, $year) = @_;
  my @days_in_month = (31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
  if ($month == 2 && is_leap_year($year)) {
    return 29;
  }
  return $days_in_month[$month - 1];
}

# Function to check if a year is a leap year
sub is_leap_year {
  my ($year) = @_;
  return ($year % 4 == 0 && $year % 100 != 0) || $year % 400 == 0;
}

