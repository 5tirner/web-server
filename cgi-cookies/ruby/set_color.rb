#!/usr/bin/env ruby

require 'cgi'
require 'cgi/session'

cgi = CGI.new
session = CGI::Session.new(cgi)

# Check if the 'color' parameter is passed
if cgi.params['color'] && !cgi.params['color'].empty?
  color = cgi.params['color'][0]

  # Set the 'color' cookie
  session['color'] = color
end

# Retrieve the 'color' cookie value
color_cookie = session['color']

# Output the HTML page with the background color
puts "Content-type: text/html\n\n"
puts "<html>"
puts "<head>"
puts "<title>Cookie Example</title>"
puts "</head>"
puts "<body style='background-color: #{color_cookie || 'white'};'>"
puts "<h1>Cookie Example</h1>"
puts "<form action='#{cgi.script_name}' method='get'>"
puts "<label for='color'>Choose a color:</label>"
puts "<input type='color' name='color'>"
puts "<input type='submit' value='Set Color'>"
puts "</form>"
puts "</body>"
puts "</html>"

