require "cgi"
cgi = CGI.new("html4")
cookie = CGI::Cookie.new('name' => 'body', 'value' => cgi['body'], 'expires' => Time.now + 3600)
cgi.out('cookie' => cookie) do
   cgi.head + cgi.body { "Cookie stored : " + cookie[0] }
end

puts "<!DOCTYPE html>"
puts "<html>"
puts "<head>"
puts "<title>Change Background Color</title>"
puts "<style>"
puts "body {"
puts "background-color: " + cgi['body'] + ";"
puts "}"
puts "</style>"
puts "</head>"
puts "<body>"
puts '<form action="bg_color_post.rb" method="post">'
puts '<label for="body">Background Color:</label>'
puts '<input type="color" id="body" name="body" value="' + cgi['body'] + '">'
puts '<input type="submit" value="Change Background Color">'
puts '</form>'
puts "</body>"
puts "</html>"
