- Rules:
# Fisrt: We Used Atoi To Convert Our NumberString So Make Some Logical Values For The Keys
# Except For Server KeyWord And Location KeyWord Don't Put Anything With The Brackets In the Same Line
# If U duplicate The keywords an override will be happen except for error_page
# don't put a server inside a server locations TOO
# put the root inside each location
# put all the indexes inside there locations
# one error page for each server
# LimitClientBodySize with megabytes
# don't write two lines in one
# feel free to do comments
# every line inside the server should end with ; except for locations
__________________________________________________________

- syntax :
# server = server { ... } --> VALID N
# port = listen number --> VALID N
# host = host hotsName --> VALID N
# serverName = server_name name --> VALID M
# BodySize = limit_client_body Number --> VALID N
# errorPage = error_page PATH --> X M

# locations = location /location{} --> VALID N
# root = root /path --> VALID N
# indexs = index .... --> VALID M
# allowedMethodes = allowed_methodes ... --> VALID M
# autoIndex = autoindex off | on --> VALID N
# return = return nbStatus website X M
# upload = upload on | off PATH --> VALID M
# cgi = cgi on | off address:port --> VALID M