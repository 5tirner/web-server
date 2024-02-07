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
# server = server { ... } --> VALID
# port = listen number --> VALID
# host = host hotsName --> VALID
# serverName = server_name name --> VALID
# LimitClientBodySize = limit_client_body Number --> VALID
# errorPage = error_page PATH --> X
# locations = location /location{} --> VALID
# root = root /path --> VALID
# autoIndex = autoindex off | on --> VALID
# allowedMethodes = allowed_methodes ... --> VALID
# indexs = index .... --> VALID
# return = return nbStatus website X
# upload = upload on | off PATH X
# cgi = cgi on | off address:port X