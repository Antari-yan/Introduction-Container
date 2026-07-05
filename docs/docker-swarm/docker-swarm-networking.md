# Docker Swarm Networking
In Swarm mode networks are usualy of the `overlay` type making them availavble on all nodes.  
Important to note that manually attaching a container to a network is not possible by default.  
To make this possible the option `--attachable` has to be added:
```bash
docker network create proxy_network --driver overlay --attachable
```

Services in other stacks can join this network by name in their compose file.
```yaml
services:
  web-net:
  networks:
    proxy_network:

networks:
  proxy_network:
    external: true
```


### Overlay vs Bridge network
In an `overlay` network the Docker DNS behaves differently compared to `bridge` networks.  
One difference that might not seem like much is that the `service name` is treated differently and
gets an `IP` that uses `round-robin` to connect to the individual underlying services.

Build the `dns-utils` container and test it:
```sh
docker build -t dns-utils -f ./dockerfiles/Dockerfile.dns-utils .
docker run --rm busybox ping google.de
docker run --rm dns-utils dig google.de
docker run --rm busybox nslookup google.de
```

Deploy the Swarm `stack`:
```sh
docker stack deploy -c compose-files/docker-compose.networking-swarm.yml net-test
```

The service `webserver-1` consists of 3 replicas where each has `webhost` set as hostname.  
When resolving the DNS entries to IPs the result is:
  - `webserver-1` has one entry
  - `tasks.webserver-1`has three entries
  - `webhost` has three entries
```sh
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils nslookup webserver-1
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils dig webserver-1
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils ping webserver-1

docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils nslookup webhost
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils dig webhost
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils ping webhost
```

The service `webserver-2` is similar to `webserver-1` except the `tasks` don't have an explicit hostname defined.  
The results are the same as before:
```sh
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils nslookup webserver-2
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils dig webserver-2
```

For `webserver-3` the `tasks` have the same hostname defined as the service name, which results in:
  - `webserver-3` has 4 entries!!! The `service name` + 3x the tasks
  - `tasks.webserver-3` contains the `three tasks`
```sh
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils nslookup webserver-3
docker container run --rm -it --net=$(docker network ls --filter name=web-net --quiet) dns-utils dig webserver-3
```

Remove the `stack`:
```sh
docker stack rm net-test
```

> [!IMPORTANT]  
> Do `not` set the hostname for `tasks` to be the same as any `service name` or any hostname in other services.  
> The Docker DNS will always do `round-robin` and when `service` and `task` IPs are grouped together like in `webserver-3`,
> it is not predictable how connections are forwarded.  
> While technically, most of the time, this is not an issue, it at least makes network communication worse to trace.
