- Rules:
# If U duplicate The keywords an override will be happen
# don't put a server inside a server locations TOO
# put the root inside each location
# put all the indexes inside there locations
# one error page for each server
# LimitClientBodySize with megabytes
# don't write two lines in one
# feel free to do comments
# lowercase syntax please
# every line inside the server should end with ; except for locations

__________________________________________________________

- syntax :
# server = server { ... } V
# port = listen number V
# host = host hotsName V
# serverName = server_name name V
# LimitClientBodySize = limit_client_body Number V
# errorPage = error_page PATH V
# locations = location /location{} X
# root = root /path V
# indexs = index .... X
# allowedMethodes = allowed_methodes ... X
# autoIndex = autoindex off | on V
# return = return nbStatus website X
# upload = upload on | off PATH X
# cgi = cgi on | off address:port X