# Sharing namespaces between containers
Sometimes it is needed for two containers to communicate with each other directly using the same network namespace,
instead of using a virtual network ot other types of namespaces.  
This is also known as running a `sidecar`, which can be useful for monitoring or configuring another container.  
Or e.g. for debugging purpose to inspect network traffic with tools like [netshoot](https://github.com/nicolaka/netshoot).

Basically, `pods` use the concept of behind `sidecars` to group multiple containers together into a single manageable unit.

```sh
# Start nginx
$CR container run --rm -d --name webserver -v ./data:/usr/share/nginx/html:ro docker.io/nginx:1-alpine

# Start BusyBox, sharing nginx's network namespace
$CR container run --rm --network container:$($CR container ls --filter name=webserver --quiet) busybox wget -qO- http://127.0.0.1

# Start BusyBox, sharing nginx's PID namespace
$CR container run --rm --pid container:$($CR container ls --filter name=webserver --quiet) busybox ps aux
$CR container run  -it --rm --pid container:$($CR container ls --filter name=webserver --quiet) busybox cat /proc/1/environ

# Start BusyBox, sharing nginx's volumes
$CR container run --rm --volumes-from webserver busybox ls -al /usr/share/nginx

$CR container stop $($CR container ls --filter name=webserver --quiet)
```
Additionally there is the `--ipc` option to share inter-process communication like `/dev/shm`.

> [!IMPORTANT]  
> While all of these options to run `sidecars` are useful, they also contain risks:
>   - With shared processes the `sidecar` can list, kill and trace processes in the main container
>   - With shared network the `sidecar` can trace network communication in the main container
>   - With shared volumes the `sidecar` can manipulate mounted data used in the main container
> Which is why it is important to not add files, networks, etc. to a container that doesn't need them.  
> Or to at least add them with reduced permissions, like readonly mounts
