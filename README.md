### Project Name: webserv.
Created By : `Youssef Saber(ysabr)` | `Youssef Achaab(yachaab)` | `Zakaria Sabri(zasabri)`
___________________
### Description:
This repository contains the source code for a C++98 HTTP server named webserv.\
It is designed to be robust, efficient, and compliant with HTTP standards(HTTP1.1).
___________________
### Features:
Configuration File: Parses a configuration file to define Server settings, Locations settings,
Confige File Content Should Be Good To Run The Web Server:
  ### Inside The Server Block:
    Server Block Should Be Look Like: server{}
    All The Key Words Excepet For `listen & host & error_page` Can Be Overrided.
    Put every Keywords In A different Line.
    All Lines Ended By ;.
    Fell Free To Do Comments (#) And Add WhiteSpaces.
    - KEYWORDS
    `default_root` to add defualt root to work.
        ==>Syntax: default_root /var/www/html;
    `listen` to add your port.
        ==>Syntax: listen 1024;
    `host` to add your host ip.
        ==>Syntax: host 127.0.0.1;
    `limit_client_body` to add the size of the uploading files.
        ==>Syntax: limit_client_body 1000;(counting with megaBytes)
    `error_page` to add your errorPages.
        ==>Syntax: error_page 200 /var/www/html/myErrPage;
  ### You Can Add Locations Blocks To Your Server:
     Location Block Should Be Look Like: location nameOfTHeLocation{}
     All The Key Words Can Be Overrided.
     Put every Keywords In A different Line.
     All Lines Ended By ;.
     `root` to add a root to the location instead of defualt_root.
        ==>Syntax: root /root/zasabri/ysabr/yachaab;
     `index` to add indexs to serve them (You Can Add Multiple Indexs).
        ==>Syntax: index index.html index.py index.php index.perl;
     `allowed_methodes` to add the methodes to the location(If Not GET Seted By Default, All The Method Should Enter UpperCase).
        ==>Syntax: allowed_methodes GET POST DELETE;
     `autoindex` to give the location rights to list directoeis.
        ==>Syntax: autoindex on;(autoindex off ot missing mean no).
     `return` to allow redirection.
        ==>Syntax: return 322 www.voovle.com;
     `upload` to allow upload (if upload missing or setted off mean you can't upload in this location).
        ==>Syntax: upload on /root/upload;
     `cgi` to allow the common gateway interface in your location (if missing i
      or you turn it off it mean cgi is outservice).
        ==>Syntax: cgi on;
Non-Blocking I/O: Utilizes non-blocking I/O operations for scalability and responsiveness.\
Multiple Ports and Hosts: Supports listening on multiple ports and serving different hosts on the same port.\
Static Content Serving: Efficiently delivers static website content.\
HTTP Methods: Handles GET, POST, and DELETE requests.\
Error Handling: Generates accurate HTTP response status codes and provides default error pages.\
File Uploads: Enables clients to upload files to the server.\
Directory Listing: Optionally displays directory listings when requested.\
CGI Support: Executes CGI scripts based on file extensions, handling POST requests and uploaded files.\
Client Body Size Limitation: Restricts the maximum allowed size of client request bodies to prevent resource exhaustion.
_______________________________________________________________________
### Requirements:
C++ compiler supporting C++98 standard (flags: -Wall -Wextra -Werror)
______________________________________________________________________
### Build Instructions:
* Clone this repository.
* Navigate to the repository directory.
* Run make to compile the source code.
_____________________________________
### Usage:
Run the server: ./webserv OR You Can Add Your confige File Name And Run: `./webserv youConfigeFileName`.
_____________________________________
### Testing:
Thoroughly test The server with various web browsers and tools.\
Consider writing automated tests in another language (Python, Php, Ruby, Perl).\
Cookie and Session Management.\
Multiple CGI Support (handle execution of multiple CGI scripts).
_______________________________________________________________
### Important Notes:
* Aim for maximum resilience – the server should never crash.
* Refer to the HTTP RFC and NGINX behavior for guidance.
* HTTP RFC: https://datatracker.ietf.org/doc/rfc7230/
* NGINX documentation: https://nginx.org/en/docs/
