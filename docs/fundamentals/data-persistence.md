# Data persistence
By default, any data created inside a container is ephemeral, which means, it disappears when the container is removed.  
While this is useful for any application that doesn't store any data, storing data is crucial for applications like databases.

To solve this, usually `bind mounts` and/or `named volumes` are used to persist data outside of the container.  

When using the `run` command add `-v <source>:<target>` or `--volume <source>:<target>` to add files or directories into a container
(this option can be used any number of times, as long as `<target>` is always different).  
`<source>` can be either a directory or file on the host system or a named volume.  
`<target>` can be any directory or file in the container.  
Missing directories an subdirectories will be created automatically.  
Optionally `:ro`/`:rw` can be appended after target to make the directory or file either readonly or writeable in the container.

To create a named volume use `$CR volume create <volume-name>` and for removing `$CR volume rm <volume-name>`.

> [!NOTE]  
> Every named volume is stored locally and taking up disk space.  
> Unused named volumes are not automatically cleaned up and need to be removed manually.  
> All unused volumes can be removed with: `$CR volume prune`

It is also possible to use different storage drivers like `nfs` ([Network File System](https://en.wikipedia.org/wiki/Network_File_System)).  
For that the option `--mount` can be used.

> [!IMPORTANT]  
> For `Docker Swarm` any of these steps would make the directories and files only available on one host.  
> When using multiple hosts as a cluster a different approach is required, more on that in [Orchestration](../docker-swarm/overview.md).

> [!IMPORTANT]  
> When running on `WSL`, the way hard drives are provided (depends on how it is configured) may fail.  
> File permissions differ between Windows and Linux and if a container writes to a directory on the Windows System (like the User dir on the `C` drive),
> can lead to the case that the file permissions break during writing.


Create a directory with a new start page:
```sh
mkdir data
echo '<h1>Hello from a persistent volume!</h1>' > data/index.html
```

Use any of the following examples and open the nginx start page:
  - in a browser: http://localhost:8080
  - in CLI: curl http://localhost:8080
Stopping (`$CR stop webserver `) and restarting the container should always show the Updated start page.

## Bind mount example
```sh
$CR container run --rm -d \
  --name webserver \
  -p 8080:80 \
  -v ./data:/usr/share/nginx/html:ro \
  docker.io/nginx:1-alpine
```

## Named volume example
Create a named volume:
```sh
$CR volume create nginx-data
```

Add an `index.html` into the volume:
```sh
$CR container run --rm \
  -v "nginx-data:/volume-data" \
  -v ./data:/mount-data:ro \
  alpine cp /mount-data/index.html /volume-data/index.html
```

Run the webserver with the volume attached:
```sh
$CR container run --rm -d \
  --name webserver \
  -p 8080:80 \
  -v nginx-data:/usr/share/nginx/html \
  nginx
```

Removing the named volume (only when the container using it is removed):
```sh
$CR volume rm nginx-data
```


## NFS mount example
Requires an NFS server.  
Change `<nfs-server-ip>` and `<path-to-nfs-shared>` and adjust mount options as needed.
```sh
$CR container run -d \
  --name nginx-nfs \
  -p 8080:80 \
  --mount 'type=volume,source=nfsvolume,target=/usr/share/nginx/html,volume-driver=local,volume-opt=type=nfs,"volume-opt=o=addr=<nfs-server-ip>,rw,nfsvers=4",volume-opt=device=:<path-to-nfs-shared>' \
  docker.io/nginx:1-alpine
```

## NFS volume example
Requires an NFS server.  
Change `<nfs-server-ip>` and `<path-to-nfs-shared>` and adjust mount options as needed.
```sh
$CR volume create --driver local \
  -o type=nfs \
  -o o=addr="<nfs-server-ip>,rw,nfsvers=4" \
  -o device=:<path-to-nfs-shared> \
  nfsvolume
```

Add an `index.html` into the volume:
```sh
$CR container run --rm \
  -v "nginx-data:/volume-data" \
  -v ./data:/mount-data:ro \
  alpine cp /mount-data/index.html /volume-data/index.html
```

Run the webserver with the volume attached:
```sh
$CR container run --rm -d \
  --name webserver \
  -p 8080:80 \
  -v nginx-data:/usr/share/nginx/html \
  nginx
```

Removing the named volume (only when the container using it is removed):
```sh
$CR volume rm nginx-data
```
