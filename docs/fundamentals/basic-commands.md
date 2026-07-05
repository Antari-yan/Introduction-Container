# Basic commands
Most commands are identical between Docker and Podman, which makes switching between them straightforward:

| Description                    | Podman                | Docker               |
| ------------------------------ | --------------------- | -------------------- |
| Help                           | `podman help`         | `docker help`        |
| CLI Version                    | `podman version`      | `docker version`     |
| System information             | `podman system info`  | `docker system info` |
| Show disk usage                | `podman system df`    | `docker system df`   |
| List running container (short) | `podman ps`           | `docker ps`          |
| List running container (long)  | `podman container ls` | `docker container ls`|

Set the `CR` variable depending on what is installed:
  - Podman: `CR=podman`
  - Docker: `CR=docker`

Check if it works:
```sh
$CR help
```

Running the first container:
```sh
# Run the "hostname" command with an alpine linux based container
$CR container run alpine hostname
# List all running container
$CR container ls
# List all container
$CR container ls --all
# Remove the last started container
$CR container rm $($CR container ls -lq)
```

Run a container outputting it's IP:
```sh
# Adding the "--rm" option, when the container stops/exits, removes it automatically
$CR container run --rm alpine ip a s
```
> [!IMPORTANT]  
> The Loopback IP 127.0.0.1 is always available and points to the container itself.  
> Avoid using `localhost`, depending on the setup it may be unavailable or point to the host system.  

Checking the OS information in the container and naming the container:
```sh
# For an alpine linux container
$CR run --rm --name alpine-container alpine cat /etc/os-release
# For a debian container
$CR run --rm --name debian-container debian:stable-slim cat /etc/os-release
```
> [!IMPORTANT]  
> The naming of a container has to be unique.

Containers often contain a fully fledged OS, but do they have to?

```sh
$CR image pull docker.io/hello-world:latest
$CR image history docker.io/hello-world:latest
```
The "hello-world" container image is just a singular binary and a `CMD` as start command for that binary.  
Usually, container images are build with a predefined `CMD` or `ENTRYPOINT`, which define what is executed when the container starts.
- `ENTRYPOINT` specifies the main executable that always runs (none by default)
- `CMD` specifies arguments that will be fed to the `ENTRYPOINT`
Both can be overwritten, and it is not unusual that only one of them is used.

Stopping and removing a container:
```sh
# A container can't be removed unless stopped
$CR run -d --name alpine-container alpine sleep 600
# "-d" for running the container in the background
$CR container rm alpine-container  # Should throw an error
$CR container stop alpine-container
# Alternatively list running container, filter by name and only return the ID:
# $CR container stop $($CR container ls --filter name=alpine-container --quiet)
```


Retrieve container logs:
```sh
$CR container logs <container-name>
# use "-n x", where x is the number of total last lines from the logs should be shown
```

To filter the logs with `grep`, pipe the log output to the hosts `stdout`:
```sh
$CR container logs <container-name> 2>&1 | grep "some text"
```

By default all output to `stdout/stderr` in the container is captured by the container runtime as logs.  
They are stored together with the container data by default as `json-file` (Docker) and `k8s-file` (Podman, `json-file` aliased to it).  
But there are also other logging driver like `systemd`/`journald` and more.
