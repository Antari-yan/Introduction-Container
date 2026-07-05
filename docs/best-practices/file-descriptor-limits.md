# File descriptor limits
Many Linux distributions limit the open files per process to `1024`.  
Orchestrators handling many sockets (reverse proxies, monitoring, MQTT brokers) easily exceed this.  
Raising the limit is often a requriement, otherwise the operation can be compromised with some cntainer not properly functioning.

Check the current limit:
```sh
ulimit -n
```

Temporarily increase the limit (resets after reboot):
```sh
ulimit -n 1000000
```

Permanently increase the limit (requires reboot):
```sh
sudo bash -c " echo '* hard nofile 1000000' >> /etc/security/limits.conf"
sudo bash -c " echo '* soft nofile 1000000' >> /etc/security/limits.conf"
```

> [!IMPORTANT]
> [Docker Engine v29.0 (containerd v2.1.5)](https://docs.docker.com/engine/release-notes/29) changed the default open file descriptor limit for containers:  
> Containers now inherit the host's soft `nofile` limit by default, which is typically much lower (`1024`for Ubuntu) and
> may lead to issues with services that require a high limit (monitoring, proxies, databases).
