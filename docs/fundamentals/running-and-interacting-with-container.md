# Running and interacting with container
Let's run a webserver and expose ports to make it accessible (more details about this in [Networking](networking.md)):
```sh
$CR container run -d --rm --name webserver --publish 8080:80 docker.io/nginx:1-alpine
# The "docker.io/" image prefix is only important for podman, more about that in [Container Registries](../container-registries/overview.md)
# 8080 -> the host port
# 80 -> the container port
```
> [!IMPORTANT]  
> Podman is not running under root, so the host port has to be `>=1024`.  
> Docker is running under root so any port can do.  
> Host ports can't be used twice at the same time.  
> In Linux you can check all used host ports with: `ss -tulpn`

Open the nginx start page:
  - In a browser: http://localhost:8080
  - In the CLI: `curl http://localhost:8080`

Get detailed information about the running container:
```sh
$CR container inspect webserver
```

Filter out specific information:
```sh
$CR container inspect --format='{{.Id}}' webserver
```
> [!NOTE]  
> Podman and Docker can have different information stored for container.  
> e.g.: Podman has the `ImageName` key, but docker doesn't.

Execute a command in a running container:
```sh
$CR container exec $($CR container ls --filter name=webserver --quiet) cat /etc/os-release
```

Open an interactive shell in a running container:
```sh
# 
$CR container exec -it $($CR container ls --filter name=webserver --quiet) /bin/sh
```
Run the following commands in the container:
```sh
ls -al
ps    # one worker per CPU core
exit
```
> [!NOTE]  
> Not all container have an interactive shell, or use a different one like `bash` instead of `sh`.

Stop the webserver:
```sh
$CR container stop webserver
```

## Using the API
Just to show that all the commands are just API interactions, the following example start/stops/removes a container in Docker using `curl`.
[Docker Engine API reference](https://docs.docker.com/reference/api/engine/) (Example is using v1.51).

Create Container:
```sh
curl -XPOST --unix-socket /var/run/docker.sock -d '{"Image":"nginx:1-alpine"}' \
  -H 'Content-Type: application/json' \
  http://localhost/containers/create?name=webserver
```

Start Container:
```sh
curl -XPOST --unix-socket /var/run/docker.sock \
  -H 'Content-Type: application/json' \
  http://localhost/containers/$(docker container ls --all --filter name=webserver --quiet)/start
```

Show running Containers:
```sh
# With jq
curl -qs --unix-socket /var/run/docker.sock http://localhost/containers/json | jq .

# With Python json.tool
curl -qs --unix-socket /var/run/docker.sock http://localhost/containers/json | python3 -mjson.tool

# Otherwise
docker ps
```

Stop Container:
```sh
curl -XPOST --unix-socket /var/run/docker.sock \
  -H 'Content-Type: application/json' \
  http://localhost/containers/$(docker container ls --all --filter name=webserver --quiet)/stop

docker ps --all
```

Delete Container:
```sh
curl -XDELETE --unix-socket /var/run/docker.sock \
  -H 'Content-Type: application/json' \
  http://localhost/containers/$(docker container ls --all --filter name=webserver --quiet)

docker ps --all
```
