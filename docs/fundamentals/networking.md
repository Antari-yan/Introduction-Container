# Networking
Containers are isolated not only in terms of filesystem and processes, but also in terms of networking.  
By default, each container runs in its own network namespace.

Additionally container are also capable for outbound communication (e.g. access the internet).  
That is because the container runtime (Podman/Docker) sets up NAT (Network Address Translation) rules using `iptables`/ `nftables`:
  - Containers get an IP from the internal bridge network.
  - Outgoing packets are masqueraded as if they came from the host's IP.
  - Replies from the internet are routed back to the container.
Analogy: It's similar to how multiple devices share one home router IP:
  - Containers = PCs/laptops/phones in a home network
  - Docker bridge = home Wi-Fi router
  - Host system = the internet gateway

The default IP Address Ranges used to provide internal IPs to the containers are:
  - Podman: `10.88.0.0/16`
  - Docker: `172.17.0.0/16`

To make communication with container from the host possible, it is required to expose/map the desired ports in a container to ports on the host system.  
For that the option `-p <host-port>:<container-port>` or `--publish <host-port>:<container-port>` can be used
(this option can be used any number of times, as long as `<host-port>` is always different).  
After making container ports available on the host, the host ports can be made accessible from beyond the host,
by adding corresponding rules on the firewall/router.

> [!IMPORTANT]  
> Podman is not running under root, so the host port has to be `>=1024`.  
> Docker is running under root so any port can do.  
> Host ports can't be used twice at the same time.  
> In Linux you can check all used host ports with: `ss -tulpn`

> [!IMPORTANT]  
> Podman is using `nftables` for NAT, should it not be installed, it is possible to switch to `iptables`,
> by adding the following to `/etc/containers/containers.conf`:
> ```ini
> [network]
> firewall_driver="iptables"
> ```

> [!IMPORTANT]  
> If the IP Address Range of the container runtime overlaps with e.g. the company IP Address Range,
> it is possible to encounter unexpected networking issues.  
> Therefore sometimes it is needed to overwrite the default IP Address Pool.  
> For docker this can be done in the [daemon configuration](https://docs.docker.com/reference/cli/dockerd/#daemon-configuration-file).
> For podman it can be defined in [/etc/cni/net.d/](https://github.com/containers/common/blob/main/docs/containers.conf.5.md#network-table)

Docker and Podman provide different networking modes that determine how containers communicate with each other and with the outside world.  
With `--network host` the network of the host system is used directly and with `--network none` networking can be completely disabled, but both of these options are rarely used.  
Additionally container can be in multiple networks.


## Default Bridge Network
- In podman the default bridge network needs to be attached explicitly with `--network podman`.
- In docker when you run a container without specifying a network, it's attached to the default bridge network.
  - It can also be added manually with `--network bridge`.
- All container on this bridge can talk to each other using their private IP addresses.
- To expose services to the host, port mapping needs to be used.

Start a webserver:
```sh
podman container run --rm -d --name webserver --network podman -p 8080:80 -p 8443:443 docker.io/nginx:1-alpine
```
```sh
docker container run --rm -d --name webserver -p 8080:80 -p 8443:443 docker.io/nginx:1-alpine
```

Retrieve the private IP address of the webserver:
```sh
WEBSERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=webserver --quiet))
```

Access the webserver start page from a new container:
```sh
podman container run -it --rm --network podman busybox wget -qO- http://${WEBSERVER_IP}
```
```sh
docker container run -it --rm busybox wget -qO- http://${WEBSERVER_IP}
```

> [!IMPORTANT]  
> While the default bridge allows connectivity, it also allows any container to directly communicate with each other.  
> This can introduce potential security issues.  


## Custom Networks and DNS
For actual network separation user-defined networks need to be created, with each container being added to it directly.  

In many cases, it is desired to isolate groups of containers so that only containers that belong together can communicate with each other.  
For example:
  - A database should only be reachable by the application container, not by unrelated containers running on the same host, like a frontend webserver.
  - Different projects should each have their own isolated networks, preventing accidental cross-communication.

```sh
$CR network create mynet
$CR container run --rm -d --name webserver --network mynet docker.io/nginx:1-alpine
WEBSERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=webserver --quiet))
```

```sh
podman run --rm --cap-add=NET_RAW busybox ping ${WEBSERVER_IP}
# this should fail, abort with ctrl+c

podman run --rm --cap-add=NET_RAW --network mynet busybox ping ${WEBSERVER_IP}
podman run --rm --cap-add=NET_RAW --network mynet busybox ping webserver
# this should succeed

# ping needs the ability to open raw sockets,
# which normally requires either root privileges or the Linux capability CAP_NET_RAW
```
```sh
docker run --rm busybox ping ${WEBSERVER_IP}
# this should fail, abort with ctrl+c

docker run --rm --network mynet busybox ping ${WEBSERVER_IP}
docker run --rm --network mynet busybox ping webserver
# this should succeed
```

```sh
$CR container stop webserver
$CR network rm mynet
```

> [!NOTE]  
>  capabilities are explained in detail in the [Limit capabilities](../compose-files/recommendations.md#limit-capabilities) section.

> [!IMPORTANT]  
> Only when using a custom network will a container receive a DNS name.  
> The DNS name is always the name set when running the container and the hostname of the container.  
> This can especially lead to issues when running multiple instances of a container,
> which is relevant for deployments with `compose files`.  
> Therefore it is advised to either use a different hostname from the deployment name.  
> most of the time the hostname isn't needed, so not explicitly setting it is better.  
> Checkout the [Docker Swarm DNS](../docker-swarm/docker-swarm-networking.md#overlay-vs-bridge-network) example for more details.

> [!IMPORTANT]  
> For podman version 3.4.4 which contains a bug for the network CNI plugin:  
> https://bugs.launchpad.net/ubuntu/+source/libpod/+bug/2024394  
> A workaround is to change the `cniVersion` for a network from `1.0.0` to `0.4.0`  
> The network config can be found in `~/.config/cni/net.d/<networkname>.conflist`
> Upgrading podman is possible but a bit tricky: https://github.com/containers/podman/discussions/25582

> [!NOTE]  
> Manually created networks aren't cleaned up automatically and take up disk space.  
> `$CR network prune` or `$CR network prune -f` can be used to remove all unused networks.


## Connecting to a host service from inside a container
Sometimes a container needs to reach a service running on the host (e.g. a reverse proxy, database, or local dev server).
The special `host-gateway` alias resolves to the host's IP as seen from the container:
```sh
$CR run --rm --add-host=myservice.localhost:host-gateway \
  alpine wget -qO- http://myservice.localhost:<port>
```
