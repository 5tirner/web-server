Config File Name: one
All Things
#example how the config file should be like:

server {
	      listen		80;
	host      zasabri;
	     server_name       		makalili;
	limit_client_body           20;
	      error_page                 /home/zasabri/Desktop/errors;
	location /test {
		root	/var/www/html;
		index	index.html index.php;
		allowed_methodes GET POST DELETE;
		autoindex on | off;
		return 301 www.zasabri.ma;
		upload	on | off /var/www/html;
		cgi on | off adress:port;
	}
}

What We Need
server {
listen	80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location /test {
root	/var/www/html;
index	index.html index.php;
allowed_methodes GET POST DELETE;
autoindex on | off;
return 301 www.zasabri.ma;
upload	on | off /var/www/html;
cgi on | off adress:port;
}
}

-------------------------------------------
After Check ^ is a server
Server Number 0:
server {
listen	80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location /test {
root	/var/www/html;
index	index.html index.php;
allowed_methodes GET POST DELETE;
autoindex on | off;
return 301 www.zasabri.ma;
upload	on | off /var/www/html;
cgi on | off adress:port;
}
}

--------------------------------------------
Server Number 0 Informations
Not Location
-> listen	80;
-> host zasabri;
-> server_name makalili;
-> limit_client_body 20;
-> error_page /home/zasabri/Desktop/errors;
Location
-> location /test {
root	/var/www/html;
index	index.html index.php;
allowed_methodes GET POST DELETE;
autoindex on | off;
return 301 www.zasabri.ma;
upload	on | off /var/www/html;
cgi on | off adress:port;
}

----------------------------------------
For Server
Port listen - 80;
Host host - zasabri;
ServerName server_name - makalili;
LimitClient limit_client_body - 20;
ErrorPage error_page - /home/zasabri/Desktop/errors;
For Locations
Location - /test {
root - /var/www/html;
index - index.html index.php;
methodes - GET POST DELETE;
autoidx - on | off;
return - 301 www.zasabri.ma;
upload - on | off /var/www/html;
cgi - on | off adress:port;
--------------------------------------------
Content Number 0:
listen	80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location /test {
root	/var/www/html;
index	index.html index.php;
allowed_methodes GET POST DELETE;
autoindex on | off;
return 301 www.zasabri.ma;
upload	on | off /var/www/html;
cgi on | off adress:port;
}
