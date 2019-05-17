# boswps-api
Apis:

/getAllProposals

/getProposal/<PROPOSAL NAME>

/getBPs



## Envirement

```shell
$ pip3 install -r requirements.txt
```

## init db
```shell
$ python3 ./init_db
```

### Setup supervisor for deamon

```ini
--open
$ vim /etc/supervisor/supervisord.conf

;it's easy to  provide the web portal with [inet_http_server] option
--settings(optional)   
[inet_http_server]         ; inet (TCP) server disabled by default
port=*:9001                ; ip_address:port specifier, *:port for all iface
username=xxxxx             ; default is no username (open server)
password=xxxxx             ; default is no password (open server)

--required
[include]
files = /path/to/supervisord.d/*.ini
```

### Import the app supervisord.d  config

```ini
[program:boswps]
directory = /path/to/boswps-api
command = gunicorn -w 4 api:app
stderr_logfile = /path/to/supervisord.d/log/boswps-stderr.log
stdout_logfile = /path/to/supervisord.d/log/boswps-stdout.log
autostart = false
autorestart = true
user = root
stopasgroup=true
killasgroup=true
```

### Set crontab
```shell
$ vim /etc/crontab

# add command
0 0 * * * curl localhost:8000/getJson >> /path/to/crontab.log 2>&1

```

### Set nginx  

```nginx
server {
    server_name xxxxxxx;
    access_log  /var/log/nginx/boswps.log;
    location / {
        proxy_pass http://127.0.0.1:8000;
        proxy_set_header Host $host;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
    listen 443 ssl; # managed by Certbot
    ssl_certificate /etc/letsencrypt/live/boswps.eosplay.me/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/boswps.eosplay.me/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot
}

server {
    if ($host = boswps.eosplay.me) {
        return 301 https://$host$request_uri;
    } # managed by Certbot
    listen 80;
    server_name boswps.eosplay.me;
    return 404; # managed by Certbot
}
```

### Start server

```shell
supervisord start boswps
```

Or:

Login  http://localhost:9001, operation it with UI.

