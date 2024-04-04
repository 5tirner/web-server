#!/usr/bin/env ruby

require 'cgi'

cgi = CGI.new

puts "Content-type: text/html\n\n"
puts "<html>"
puts "<head>"
puts "<title>CGI Example</title>"
puts "</head>"
puts "<body>"
puts "<h1>Hello, CGI!</h1>"
puts "</body>"
puts "</html>"

