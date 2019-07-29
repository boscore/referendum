# boswps-api
Apis:

/getAllProposals

/getProposal/<PROPOSAL NAME>

/getBPs

/getAllAuditors

/getAuditor/<AUDITOR NAME>


## Envirement

```shell
$ pip3 install -r requirements.txt
```

<!-- ## init db
```shell
$ python3 ./init_db
``` -->


## set google translation credentials
## copy the Google auth file in the api directory and rename it GOOGLE_APPLICATION_CREDENTIALS.json




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
$ crontab -e

# add command
0 0 * * * curl localhost:8000/getJson >> /path/to/crontab.log 2>&1
0 0 * * * curl localhost:8000/getAuditorJson >> /path/to/crontab.log 2>&1
# 3min call tally json files 
*/4 0 * * * ./minutely_get_tally.sh >> /path/to/crontab.log 2>&1
/root/referendum/api/minutely_get_tally.sh
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

Login  http:// localhost:9001, operation it with UI.

## init dataset
```shell
curl localhost:8000/getJson && curl localhost:8000/getAuditorJson
```

### Support Languages(Power By Google)

| 语言 | 	ISO-639-1 代码| 
|  ----  | ----  |
|南非荷兰语	|af|
|阿尔巴尼亚语	|sq|
|阿姆哈拉语	|am|
|阿拉伯语	|ar|
|亚美尼亚语	|hy|
|阿塞拜疆语	|az|
|巴斯克语	|eu|
|白俄罗斯语	|be|
|孟加拉语	|bn|
|波斯尼亚语	|bs|
|保加利亚语	|bg|
|加泰罗尼亚语	|ca|
|宿务语	ceb |(ISO-639-2)|
|中文（简体）	|zh-CN (BCP-47) 程序传入cn|
|中文（繁体）	|zh-TW (BCP-47) 程序传入tw|
|科西嘉语	|co|
|克罗地亚语	|hr|
|捷克语	|cs|
|丹麦语	|da|
|荷兰语	|nl|
|英语	|en|
|世界语	|eo|
|爱沙尼亚语	|et|
|芬兰语	|fi|
|法语	|fr|
|弗里斯兰语	|fy|
|加利西亚语	|gl|
|格鲁吉亚语	|ka|
|德语	|de|
|希腊语	|el|
|古吉拉特语	|gu|
|海地克里奥尔语	|ht|
|豪萨语	|ha|
|夏威夷语	|haw (ISO-639-2)|
|希伯来语	|he**|
|印地语	|hi|
|苗语	|hmn (ISO-639-2)|
|匈牙利语	|hu|
|冰岛语	|is|
|伊博语	|ig|
|印度尼西亚语	|id|
|爱尔兰语	|ga|
|意大利语	|it|
|日语	|ja|
|爪哇语	|jw|
|卡纳达语	|kn|
|哈萨克语	|kk|
|高棉语	|km|
|韩语	|ko|
|库尔德语	|ku|
|吉尔吉斯语	|ky|
|老挝语	|lo|
|拉丁语	|la|
|拉脱维亚语	|lv|
|立陶宛语	|lt|
|卢森堡语	|lb|
|马其顿语	|mk|
|马尔加什语	|mg|
|马来语	|ms|
|马拉雅拉姆语	|ml|
|马耳他语	|mt|
|毛利语	|mi|
|马拉地语	|mr|
|蒙古语	|mn|
|缅甸语	|my|
|尼泊尔语	|ne|
|挪威语	|no|
|尼杨扎语（齐切瓦语）	|ny|
|普什图语	|ps|
|波斯语	|fa|
|波兰语	|pl|
|葡萄牙语（葡萄牙、巴西）	|pt|
|旁遮普语	|pa|
|罗马尼亚语	|ro|
|俄语	|ru|
|萨摩亚语	|sm|
|苏格兰盖尔语	|gd|
|塞尔维亚语	|sr|
|塞索托语	|st|
|修纳语	|sn|
|信德语	|sd|
|僧伽罗语	|si|
|斯洛伐克语	|sk|
|斯洛文尼亚语	|sl|
|索马里语	|so|
|西班牙语	|es|
|巽他语	|su|
|斯瓦希里语	|sw|
|瑞典语	|sv|
|塔加路语（菲律宾语）	|tl|
|塔吉克语	|tg|
|泰米尔语	|ta|
|泰卢固语	|te|
|泰语	|th|
|土耳其语	|tr|
|乌克兰语	|uk|
|乌尔都语	|ur|
|乌兹别克语	|uz|
|越南语	|vi|
|威尔士语	|cy|
|班图语	|xh|
|意第绪语	|yi|
|约鲁巴语	|yo|
|祖鲁语	|zu|
