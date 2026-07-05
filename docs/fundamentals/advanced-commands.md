# Advanced Commands
Format the output of list of running Containers:
```sh
$CR container ls --format "{{.ID}}: {{.Names}}"
$CR container ls --format "table {{.ID}}\t{{.Names}}"
$CR container ls --format "table {{.ID}}\t{{.Names}}\t{{.State}}\t{{.Status}}\t{{.Size}}"
```
> [!TIP]  
> For `docker` it is possible to overwrite the default list output:
> ```json
> {
>   "psFormat": "table {{.ID}}\\t{{.Image}}\\t{{.Status}}\\t{{.Names}}"
> }
> ```

Get ID of last created Container:
```sh
$CR container ls -lq
```

Get ID of a Container by name:
```sh
$CR container ls --filter name=webserver --quiet
```

Get Filesystem diff of a Container:
```sh
$CR diff $($CR container ls --filter name=webserver --quiet)
```

Get IP of a container:
```sh
$CR container inspect --format \
  '{{.NetworkSettings.Networks.bridge.IPAddress}}' \
  $($CR container ls --filter name=webserver --quiet)
```

Get Env Vars of a Container:
```sh
$CR container inspect $($CR container ls --filter name=webserver --quiet) --format='{{range .Config.Env}}{{println .}}{{end}}'
```
- `range`: Iterate over an array -> Env array under config
- `println .`: Print each item in the range
- `end`: Closes range
Sometimes `printenv` and `printenv <var>` are also available:
```sh
$CR exec $($CR container ls --filter status=running --filter name=webserver --quiet) printenv
```

> [!NOTE]
> Always add `--filter status=running` when filtering containers by name in scripts
> to avoid accidentally matching stopped containers that share the same name prefix.

Get all network names connected to a container:
```sh
$CR inspect $($CR container ls --filter name=webserver --quiet) -f '{{range $k, $v := .NetworkSettings.Networks}}{{printf "%s\n" $k}}{{end}}'
```

Get log path of a container in `docker`:
```sh
sudo ls -alh /var/lib/docker/containers/$(docker inspect $(docker container ls --filter name=webserver --quiet) -f '{{.Id}}')
sudo ls -alh $(docker inspect $(docker container ls --filter name=webserver --quiet) -f '{{.LogPath}}')
```

More information about formatting the output can be found here:
  - [podman inspect](https://docs.podman.io/en/latest/markdown/podman-inspect.1.html)
  - [docker formatting](https://docs.docker.com/engine/cli/formatting/)
