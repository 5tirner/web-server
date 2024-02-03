Config File Name: one
All Things
#example how the config file should be like:

server {
	listen 80;
	host zasabri;
	server_name makalili;
	limit_client_body 20;
	error_page /home/zasabri/Desktop/errors;
	location{
		allowed_methods GET;
		hi
	}
}

What We Need
server {
listen 80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location{
allowed_methods GET;
hi
}
}

-------------------------------------------
After Check ^ is a server
Server Number 0:
server {
listen 80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location{
allowed_methods GET;
hi
}
}

--------------------------------------------
Server Number 0 Informations
Not Location
-> listen 80;
-> host zasabri;
-> server_name makalili;
-> limit_client_body 20;
-> error_page /home/zasabri/Desktop/errors;
Location
-> location{allowed_methods GET;hi}
----------------------------------------
Port listen - 80;
Host host - zasabri;
ServerName server_name - makalili;
LimitClient limit_client_body - 20;
ErrorPage error_page - /home/zasabri/Desktop/errors;
--------------------------------------------
Content Number 0:
listen 80;
host zasabri;
server_name makalili;
limit_client_body 20;
error_page /home/zasabri/Desktop/errors;
location{
allowed_methods GET;
hi
}
