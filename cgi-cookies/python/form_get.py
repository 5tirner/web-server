#!/usr/bin/env python

# Generate the response
# print("Content-type: text/html")
# print("")
print("<html>")
print("<head>")
print("<title>CGI Script</title>")
print('<link rel="stylesheet" type="text/css" href="style.css">')
print("</head>")
print("<body>")
print("<div>")
print('<form action="script.py" method="get">')
print('<label for="name">Name:</label>')
print('<input type="text" id="name" name="name" required><br>')
print('<label for="Email">Email:</label>')
print('<input type="email" id="email" name="email" required><br>')
print('<input type="submit" value="Login">')
print('</form>')
print("</div>")
print("</body>")
print("</html>")

