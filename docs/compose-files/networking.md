# Networking
By default, even if no network is explicitly defined,
a compose deplyoment will create a custom network and adds all services to it that doen't have a network defined.  
Because of this the DNS name resolution by service name is always available and should be used when communicating between services.

Start the networking example deployment:
```sh
# 
$CR compose -f compose-files/docker-compose.networking.yml up -d
```

Connect to the container by service name:
```sh
$CR container run --rm --net=container:$($CR container ls --filter name=webserver-1 --quiet) busybox nslookup webserver-1
```

Start the networking example deployment:
```sh
$CR compose -f compose-files/docker-compose.networking.yml down
```

> [!IMPORTANT]  
> The DNS name in a container is always the service name and the hostname of the container.  
> This can especially lead to issues when running multiple instances of a container using `replicas`.  
> Therefore it is advised to either use a different hostname from the service name.  
> Most of the time the hostname isn't needed, so not explicitly setting it is better.  
> Checkout the [Docker Swarm DNS](../docker-swarm/docker-swarm-networking.md#overlay-vs-bridge-network) example for more details.


## Injecting dynamic config files via environment variables
Some containers expect a config file.  
Usually the correct approach is to use a volume or add the config to the container durin built time,
but sometimes the config file needed is exceptionally small that wouldn't be worth the effort.
A possible solution is to add the config file as environment variable and writing it too a file before relevant start command:
```yaml
# In a compose file
services:
  traefik:
    environment:
      TLS_DYNAMIC: |
        tls:
          certificates:
            - certFile: /etc/ssl/certs/server.pem
              keyFile:  /etc/ssl/private/server.key
    entrypoint:
      - /bin/sh
      - -c
      - |
        echo "$$TLS_DYNAMIC" > /tmp/tls-dynamic.yaml
        exec /entrypoint.sh "$$@"
      - --
      # A single `$` would be interpreted by compose itself during variable substitution
      # `exec` replaces the shell process with the real entrypoint so that PID 1 remains the application
    command:
      - --configfile=/tmp/tls-dynamic.yaml
```
