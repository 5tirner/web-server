Config File Name: otherconf/default
All Things
# one listen keyword for each server
# put the root inside each location
# put all the indexes inside there locations
# don't write two lines in one
# feel free to do comments

server {{


	listen 80 default_server;
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

server          {
        listen 80;
            
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

What We Need
server {{
listen 80 default_server;
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
server {
listen 80;
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
Server Number 0:
server {{
listen 80 default_server;
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

Server Number 1:
server {
listen 80;
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
