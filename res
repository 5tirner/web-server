Config File Name: default
All Things
     # one listen keyword for each server
# don't put a server inside a server
# put the root inside each location
     # put all the indexes inside there locations


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
{{
        listen 80;;;;;
        server_name _;;;;;;
        location / {
                try_files $uri $uri/ =404;
        }}
        location /test
        {
            root /var/www/html;
            
            index index.html index.htm index.nginx-debian.html;
        }
}
server           {
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

server     



{        
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
server 
{{
listen 80;;;;;
server_name _;;;;;;
location / {
try_files $uri $uri/ =404;
}}
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
server 
{ 
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
After Check ^ is a server
Server Number 0:
server 
{{
listen 80;;;;;
server_name _;;;;;;
location / {
try_files $uri $uri/ =404;
}}
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

Server Number 2:
server 
{ 
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
