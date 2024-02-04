Config File Name: one
All Things
#example how the config file should be like:
#server1
server 

{
	      listen		80;     
	host      zasabri;
	     server_name       		makalili;     
	limit_client_body           20;
	      error_page                 /home/zasabri/Desktop/errors;
location /test {
		root	/var/www/html;   #hi 
		index	index.html index.php;
			allowed_methodes GET POST DELETE;
		autoindex on |      off;  
return 301 	www.zasabri.ma;
					upload	on | off /var/www/html;
			cgi on | off adress:port;  
	}   
}        
#server2
server 

{
	      listen		443;     
	host      ysabr;
	     server_name       		sade_manio;     
	limit_client_body           100;
	      error_page                 /home/ysabr/errors;
location /nottest {
		root	/root;   #efgru 
		index	index.black index.brawn;
			allowed_methodes  DELETE;
		autoindex on;  
return 303 ysabr.ma;
					upload	on | off /var/www/html;
			cgi off adress:port;  
	}   
}  

What We Need
server 
{
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
server 
{
listen	443; 
host ysabr;
server_name sade_manio; 
limit_client_body 100;
error_page /home/ysabr/errors;
location /nottest {
root	/root; 
index	index.black index.brawn;
allowed_methodes DELETE;
autoindex on; 
return 303 ysabr.ma;
upload	on | off /var/www/html;
cgi off adress:port; 
} 
} 

-------------------------------------------
After Check ^ is a server
After Check ^ is a server
Server Number 0:
server 
{
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

Server Number 1:
server 
{
listen	443; 
host ysabr;
server_name sade_manio; 
limit_client_body 100;
error_page /home/ysabr/errors;
location /nottest {
root	/root; 
index	index.black index.brawn;
allowed_methodes DELETE;
autoindex on; 
return 303 ysabr.ma;
upload	on | off /var/www/html;
cgi off adress:port; 
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
------------------------------------
Server Number 1 Informations
Not Location
-> listen	443; 
-> host ysabr;
-> server_name sade_manio; 
-> limit_client_body 100;
-> error_page /home/ysabr/errors;
Location
-> location /nottest {
root	/root; 
index	index.black index.brawn;
allowed_methodes DELETE;
autoindex on; 
return 303 ysabr.ma;
upload	on | off /var/www/html;
cgi off adress:port; 
} 

----------------------------------------
For Server
Port listen - 443; 
Host host - ysabr;
ServerName server_name - sade_manio; 
LimitClient limit_client_body - 100;
ErrorPage error_page - /home/ysabr/errors;
For Locations
Location - /nottest {
root - /root; 
index - index.black index.brawn;
methodes - DELETE;
autoidx - on; 
return - 303 ysabr.ma;
upload - on | off /var/www/html;
cgi - off adress:port; 
------------------------------------
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
Content Number 1:
listen	443; 
host ysabr;
server_name sade_manio; 
limit_client_body 100;
error_page /home/ysabr/errors;
location /nottest {
root	/root; 
index	index.black index.brawn;
allowed_methodes DELETE;
autoindex on; 
return 303 ysabr.ma;
upload	on | off /var/www/html;
cgi off adress:port; 
} 
