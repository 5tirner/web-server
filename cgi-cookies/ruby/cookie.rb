#!/usr/bin/ruby

require "cgi"
cgi = CGI.new("html4")
cookie = cgi.cookies['body']
if cookie && !cookie.empty?
  cgi.out('cookie' => cookie) do
   cgi.head + cgi.body("style" => "background-color: " + cookie[0] + ";"){ "cookies : " + cookie[0] }
  end
  else
  cgi.out('cookie' => cookie) do
   cgi.head + cgi.body { "no cookie stored"}
  end
end

