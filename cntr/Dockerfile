FROM    debian

RUN     apt update -y; apt install nginx -y; apt install nginx-module-upload -y

CMD     ["nginx", "-g", "daemon off;"]