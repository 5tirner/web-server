Config File Name: default
All Things
     # one listen keyword for each server
# don't put a server inside a server
# put the root inside each location
     # put all the indexes inside there locations
#LimitClientBodySize with megabytes

    # don't write two lines in one
     #   feel free to do comments
# lowercase syntax please
#syntax :
# server = server { }
# porst = listen number
# serverName = server_name name
# locations = location /location{}
# indexs = index ....
# root = root /path

server 







		{#test;
	LimitClientBodySize                     1;   #test
	listen 80;;;;;
	server_name _;#;;;;;;
	location / {
		allowed_methods POST GET DELETE;
	}
	test -1;
	location /test
	{
	    root /var/www/html;
	    
	    index index.html index.htm index.nginx-debian.html;
	}
}
#hello




server           {
	listen 80;
	server_name _;
	 
	 test0
	location / {
		try_files $uri $uri/ =404;
	}
	location /test
	{
	    root /var/www/html;
	    
	    index index.html index.htm index.nginx-debian.html;
	}



					}
#bad
server   	
{
	listen 80;
	    test1
	server_name _;
	location / {
		try_files $uri $uri/ =404;
	}
	location /test
	{
	    root /var/www/html;
	    
	    index index.html index.htm index.nginx-debian.html;
	}
}
#end

What We Need
server 
{
LimitClientBodySize 1; 
listen 80;;;;;
server_name _;
location / {
allowed_methods POST GET DELETE;
}
test -1;
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}
server {
listen 80;
server_name _;
test0
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}
server 
{
listen 80;
test1
server_name _;
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}

-------------------------------------------
After Check ^ is a server
After Check ^ is a server
After Check ^ is a server
Server Number 0:
server 
{
LimitClientBodySize 1; 
listen 80;;;;;
server_name _;
location / {
allowed_methods POST GET DELETE;
}
test -1;
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}

Server Number 1:
server {
listen 80;
server_name _;
test0
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}

Server Number 2:
server 
{
listen 80;
test1
server_name _;
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
}

--------------------------------------------
Server Number 0 Informations
Not Location
-> LimitClientBodySize 1; 
-> listen 80;;;;;
-> server_name _;
-> test -1;
Location
-> location / {allowed_methods POST GET DELETE;}
-> location /test{root /var/www/html;index index.html index.htm index.nginx-debian.html;}
Server Number 1 Informations
Not Location
-> listen 80;
-> server_name _;
-> test0
Location
-> location / {try_files $uri $uri/ =404;}
-> location /test{root /var/www/html;index index.html index.htm index.nginx-debian.html;}
Server Number 2 Informations
Not Location
-> listen 80;
-> test1
-> server_name _;
Location
-> location / {try_files $uri $uri/ =404;}
-> location /test{root /var/www/html;index index.html index.htm index.nginx-debian.html;}
--------------------------------------------
Content Number 0:
LimitClientBodySize 1; 
listen 80;;;;;
server_name _;
location / {
allowed_methods POST GET DELETE;
}
test -1;
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
Content Number 1:
listen 80;
server_name _;
test0
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
Content Number 2:
listen 80;
test1
server_name _;
location / {
try_files $uri $uri/ =404;
}
location /test
{
root /var/www/html;
index index.html index.htm index.nginx-debian.html;
}
