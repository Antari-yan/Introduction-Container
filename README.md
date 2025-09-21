# Introduction to Container
The Container technology revolutionized in how to develop, run and manage software, from small applications to huge interconnected clustered applications (e.g. Web-App, Database, ...).  
All of that is made possible through flexible configuration, Container Images and a short starting time.

This repository is intended as a short introduction into what containers are, how to run and manage them and the concept of packaging software and its dependencies into a single portable unit.  
It also covers tools like [Podman](https://podman.io/) and [Docker](https://www.docker.com/), as well as touching on [Kubernetes](https://kubernetes.io/)(K8s).

Anywhere the commands for Podman and Docker overlap the variable `CR` will be used, an easy way to set it is with `CR=$(command -v podman || command -v docker)`, which selects either `podman` or `docker`, depending on what is installed.

## Setup
All examples are provided for `Podman` and the `Docker Engine`(Docker CE).  
One of them should be installed, which can be done through their installation instructions:
- [Install Podman](https://podman.io/docs/installation)
- [Install Docker](https://docs.docker.com/engine/install/)

> [!IMPORTANT]  
> Ubuntu 22.04 uses podman version 3.4.4 which contains a bug for the network CNI plugin:  
> https://bugs.launchpad.net/ubuntu/+source/libpod/+bug/2024394  
> A workaround is to change the `cniVersion` for a network from `1.0.0` to `0.4.0`  
> The network config can be found in `~/.config/cni/net.d/<networkname>.conflist`
> Upgrading podman is possible but a bit tricky: https://github.com/containers/podman/discussions/25582  
> A general recommendation would be to use a newer distribution.

While it won't be covered, both also have a UI tool:
  - [Podman Desktop](https://podman-desktop.io/docs/installation) (No License required)
  - [Docker Desktop](https://www.docker.com/products/docker-desktop/) (Requires License for commercial use)
  - [VSCode Extension](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-containers)



## Motivation & Background of container
### Traditional deployment challenges
Before containers were invented, developers and IT teams struggled with complex software deployment challenges.  
Applications were tightly coupled with specific operating system configurations and software dependencies, which could lead to compatibility issues, dependency conflicts, and time-consuming setup processes.
Phrases like `It works on my machine` weren't always just jokes and running multiple applications on the same system could lead to clashes between libraries or runtime environments (e.g., two applications requiring different versions of Python or Java).  
Slight differences in operating system, dependencies, or configurations could lead to inconsistent or non-reproducible results.

While virtual machines (VMs) are great to isolate environments they come with at a cost: each VM requires its own operating system, making them large and slow to boot.
Running many VMs wastes system resources and complicate scaling.

### Virtual Machines vs. Containers
- Virtual Machines (VMs):
  - Each VM runs a full operating system, including its own kernel.
  - They are heavier, consuming more CPU, RAM, and disk space.
  - Startup times are relatively slow (tens of seconds to minutes).
  - Provide strong isolation, useful for running completely different OSes.
  - While scaling is possible, setting up a whole VM usually takes at least a couple of minutes.
- Containers:
  - Share the host operating system kernel but isolate processes, filesystems, and networking.
  - They are lightweight, often just a few megabytes or less.
  - Containers can start in seconds or less, because the host kernel is already available.
  - Simple at scaling up and down
  - Can run basically anywhere - on a laptop, a server, the cloud or on a RaspberryPi.

### Background
#### Docker
- Launched in 2013 by the `dotCloud` company (later renamed to `Docker, Inc.`), Docker popularized containers by making them easy to build, share, and run.
- Prior to Docker, Linux had features like cgroups and namespaces that enabled containers, but they were difficult to use. Docker wrapped these capabilities in a simple developer-friendly interface.
- Docker’s key innovation was the `Docker Image` and `Dockerfile`, which streamlined the process of defining environments and distributing them via `Docker Hub`.
- 2019 `Docker, Inc.` sold its enterprise business to `Mirantis`
- Today, Docker still exists as the developer-facing tool, but its role in orchestration has been eclipsed by Kubernetes.
- Uses the moby project as the upstream for the Docker Product: https://github.com/moby/moby
  - Moby is an open-source project created by Docker, containing:
    - container build tools
    - a container registry
    - orchestration tools
    - a runtime
    - and more

#### Docker Swarm
- Docker initially pushed `Docker Swarm` (launched in 2014) as its orchestration solution.
- Swarm is simpler to use than Kubernetes, but lacked the capabilities, flexibility, and community backing of Kubernetes.
- By 2017–2018, `Kubernetes` was more widely used, and `Docker Swarm` fell into minimal maintenance mode, but it is still available.
- Because of the minimal maintenance there are many bugs and missing configuration options that are otherwise available for a Standalone Docker. (e.g., `depends_on`, `deploy.mode: replicated-job`, ...)

#### Kubernetes
- At `Google`, developers needed to manage containers at massive scale.
- Google built an internal system called `Borg` in the early 2000s to run millions of containers across thousands of machines.
- In 2014, Google released `Kubernetes` (inspired by Borg) as an open-source orchestration system.
- Kubernetes quickly became the de facto industry standard for container orchestration due to:
  - Extensibility (Custom Resources, Operators)
  - Backing by the [Cloud Native Computing Foundation (CNCF)](https://www.cncf.io/).
  - Strong community support
- Kubernetes effectively replaced Docker Swarm in most serious production environments.
- The smallest managable unit in Kubernetes is a `pod`, which is a group of one or more container (checkout [Sharing namespaces between Containers](#sharing-namespaces-between-containers) for how `pods` basically work)

#### The Open Container Initiative (OCI)
- In 2015, with multiple runtimes and image formats emerging, the industry needed standardization.
- The `Open Container Initiative (OCI)` was founded by Docker, CoreOS, and others under the Linux Foundation.
- OCI maintains open standards for:
  - `Image Format` (how container images are packaged)
  - `Container Registries` (how container images are distributed)
  - `Runtime Specification` (how containers are executed on a host)
- Thanks to the OCI, tools like Docker, Kubernetes, etc. interoperate on a common foundation. This prevents fragmentation of the ecosystem.

#### Podman
- Developed by `Red Hat` as an open-source daemonless and rootless alternative to Docker.
- Unlike Docker, Podman doesn’t require a background service (`dockerd`) and can run entirely without root privileges, improving security.
- Podman intentionally mimics Docker’s CLI, so commands like `podman run` are nearly identical to `docker run`.
- It’s popular in enterprise and security-sensitive contexts and integrates tightly with `systemd` (the Linux system and service manager).
- Also muses the concept of `pods` like Kubernetes

#### Linux Namespaces
Linux namespaces provide isolation for system resources, meaning a process or a group of processes can operate in its own „namespace,“ unaware of other processes using the same resources in different namespaces. Namespaces isolate processes, allowing them to have their own view of system resources like process IDs (PIDs), file systems, network interfaces, user IDs, and more.

Each type of namespace provides isolation for a specific system resource:
  - PID namespace: Isolates the process IDs, meaning processes in one namespace can't see or affect processes in another namespace.
  - Mount namespace: Isolates the file system mount points.
  - Network namespace: Isolates network interfaces, IP addresses, routing tables, etc.
  - UTS namespace: Isolates hostname and domain name.
  - IPC namespace: Isolates inter-process communication resources such as shared memory and message queues.
  - User namespace: Isolates user and group IDs, allowing a process to have different user IDs inside and outside the namespace.
  - Cgroup namespace: Isolates control group (cgroup) resources, which are used to manage resource allocation (CPU, memory, etc.).

Namespaces are used to provide isolation for containers. Each container is placed in its own namespaces, giving the impression of a separate machine with its own processes, network interfaces, and file system.

#### Windows and Apple
Both Windows and Apple have developed their own approaches for container and container runtime.

There is [Windows Containers](https://github.com/microsoft/Windows-Containers), which are basically Windows based container, instead of Linux based.  
So far they one work with Azure and on Windows hosts.

Apple has [container](https://github.com/apple/container), which is basically their own version of a container runtime for Mac, that uses virtual machines to run container.

This repo won't go into detail of either, because they are more like niche products with highly specific restrictions to use them.



## What is a Container?
At its core, a container is a way to `package software` together with everything it needs to run and then execute it in an `isolated environment` on a host system.

### Definition
A container is:
  - A `lightweight`, `standalone`, `executable unit` of software.
  - It contains:
    - the application code
    - libraries and dependencies (optional)
    - system tools (optional)
    - runtime (e.g., Python interpreter, Java runtime) (optional)
  - Runs in `isolation` from other containers and from the host system, while still sharing the host’s operating system kernel.

This means that if your software runs inside a container on your laptop, the same container will run the same way on a server, a colleague’s workstation, or in the cloud.

### Analogy: Shipping Containers
The best way to think about containers is to compare them with shipping containers in logistics:
  - Before shipping containers, transporting goods was chaotic - each product needed special handling.
  - Shipping containers standardized the process: goods are packed into a box that can be moved by any ship, train, or truck.
  - Similarly, a software container “packs” code and dependencies into a standard format that can run anywhere a container engine exists.
  - Just like physical containers can be stacked and shipped efficiently, software containers can be deployed, scaled, and moved around easily.

### Key Concepts
To understand containers, we need to clarify some essential building blocks:
  - Image
      - A container image is a blueprint or template.
      - It is read-only and describes the filesystem and environment your application needs.
      - Example: An image might include Ubuntu, Python 3.11, and your machine learning code.
  - Container
      - A container is a running instance of an image.
      - Multiple containers can be created from the same image.
      - Containers can be short-lived (e.g., a data processing task) or long-running (e.g., a web server).
  - Registry
      - A registry is a storage and distribution system for container images.
      - Public: Docker Hub, GitHub Container Registry.
      - Private: self-hosted registries for labs or enterprises.



## Container Runtime
Under the hood, containers are made possible by `container runtimes`, which is specified by the OCI [here](https://github.com/opencontainers/runtime-spec).
These are the low-level components responsible for creating and running containers on the host operating system.

Most commonly used are:
  - [runc](https://github.com/opencontainers/runc)
    - A low-level container runtime that actually starts and runs containers, by directly interacting with the Linux kernel.
      - handling the low-level operations like namespace isolation, cgroups management, and file system mounts.
        - Namespaces: Provide isolation for processes, networking, and file systems. runc uses Linux namespaces (such as PID, UTS, IPC, MNT, and NET) to ensure that containers are isolated from each other and from the host system.
        - Cgroups (Control Groups): Manage resource allocation and limitation. runc leverages cgroups to control the amount of CPU, memory, I/O, and other resources that a container can use.
        - Capabilities: Allow fine-grained control over what processes inside the container can do. runc sets Linux capabilities to restrict the privileges of containerized processes, enhancing security.
        - Seccomp (Secure Computing Mode): Limits the system calls that a containerized process can execute, reducing the attack surface. runc applies seccomp profiles to containers, allowing only a subset of safe system calls.
        - Filesystem Mounts: runc handles setting up the container's filesystem, including root filesystem and additional mounts (such as volumes or bind mounts) as defined in the container's configuration.
      - doesn't include higher-level container management features like image distribution or networking orchestration, which are instead handled by higher-level tools like Docker or Kubernetes.
    - Originally developed in Go as part of Docker, later donated to the Open Container Initiative (OCI).
    - Implements the OCI runtime specification, ensuring interoperability across tools.
  - [containerd](https://github.com/containerd/containerd)
    - A higher-level runtime that manages the complete lifecycle of containers (pulling images, starting/stopping containers, managing storage and networking).
    - Used internally by Docker and (sometimes) Kubernetes.
  - [CRI-O](https://github.com/cri-o/cri-o)
    - A lightweight container runtime built specifically for Kubernetes.
    - Implements the Container Runtime Interface (CRI).

How it fits together:
  - Docker -> calls `containerd` -> which then uses `runc` to create and manage the actual container process.
  - Podman is using the `libpod` library to directly interact with `runc`, `crun`, or other OCI compliant container runtimes
  - This layered design means different container tools can share the same underlying runtime, thanks to OCI standards.

### Docker
- Released in 2013, Docker was the first tool to make containers accessible to developers.
- Provides a CLI (docker) and relies on a daemon process (`dockerd`) that runs in the background to manage containers.
- Widely adopted in industry; most tutorials and documentation still use Docker commands.
- Strong ecosystem: Docker Hub (public image registry), Docker Compose (multi-container applications).
- The Docker Engine, is a client-server application that includes:
  - Docker Daemon (`dockerd`): Runs in the background and manages Docker objects like containers, images, networks, and volumes. It listens for Docker API requests and is responsible for building, running, and distributing Docker containers.
  - Docker Client (`docker`): A command-line interface (CLI) tool that allows users to interact with the Docker Daemon. When you type commands like docker run or docker build, the client sends these commands to the Docker Daemon, which then carries them out.
  - `Containerd`
  - `runc`
  - Networking Components: Docker installs components to set up and manage container networks. This includes the Docker network bridge (docker0) and tools for setting up networking options like overlay networks for multi-host container networking.
  - Docker CLI Plugins: Additional command-line tools that extend the Docker CLI capabilities 

Pros
  - Most popular and widely supported container tool.
  - Large ecosystem of documentation, images, and community resources.
  - Easy to get started.

Cons
  - Requires a privileged background daemon (dockerd).
  - Containers often run as root by default, which can raise security concerns.
  - Orchestration tool (Swarm) is minimally maintained compared to Kubernetes.

> [!IMPORTANT]  
> Anything like `Images`, `Container`, `Volumes`, etc. are stored locally in `/var/lib/docker`.  


### Podman
- Introduced by Red Hat, Podman is a container engine designed as a drop-in replacement for Docker.
- Daemonless architecture: Podman does not rely on a background service.
- Supports rootless mode, allowing containers to run without administrator privileges - a significant security improvement.
- Integrates tightly with systemd, the init system used by most modern Linux distributions, making it suitable for managing long-running services.
- Uses the concept of `pods` like kubernetes
  - A `pod` is a group of one or more containers, with shared storage and network resources, and a specification for how to run the containers.

Pros
  - Secure by design (rootless, daemonless).
  - Docker-compatible CLI (podman run ≈ docker run).
  - Supported by enterprise Linux distributions (RHEL, Fedora).
  - Can run Kubernetes files (Helm charts need to be converted first with the `helm template` command)

Cons
  - Less common in tutorials and examples compared to Docker.

> [!IMPORTANT]  
> Anything like `Images`, `Container`, `Volumes`, etc. are stored locally in `~/.local/share/containers`.  
> Configuration can also be located in `/etc/containers/`.



## Fundamentals for working with container

### Basic commands
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

### Advanced Commands
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
```
$CR exec $(docker container ls --filter name=webserver --quiet) printenv
```

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



### Running and interacting with container
Let's run a webserver and expose ports to make it accessible (more details about this in [Networking](#networking)):
```sh
$CR container run -d --rm --name webserver --publish 8080:80 docker.io/nginx:1-alpine
# The "docker.io/" image prefix is only important for podman, more about that in [Container Registries](#container-registries)
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

#### Using the API
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


### Data persistence
By default, any data created inside a container is ephemeral, which means, it disappears when the container is removed.  
While this is useful for any application that doesn't store any data, storing data is crucial for applications like databases.

To solve this, usually `bind mounts` and/or `named volumes` are used to persist data outside of the container.  

When using the `run` command add `-v <source>:<target>` or `--volume <source>:<target>` to add files or directories into a container (this option can be used any number of times, as long as `<target>` is always different).  
`<source>` can be either a directory or file on the host system or a named volume.  
`<target>` can be any directory or file in the container.  
Missing directories an subdirectories will be created automatically.  
Optionally `:ro`/`:rw` can be appended after target to make the directory or file either readonly or writable in the container.

To create a named volume use `$CR volume create <volume-name>` and for removing `$CR volume rm <volume-name>`.

> [!NOTE]  
> Every named volume is stored locally and taking up disk space.  
> Unused named volumes are not automatically cleaned up and need to be removed manually.  
> All unused volumes can be removed with: `$CR volume prune`

It is also possible to use different storage drivers like `nfs` ([Network File System](https://en.wikipedia.org/wiki/Network_File_System)).  
For that the option `--mount` can be used.

> [!IMPORTANT]  
> For `Docker Swarm` any of these steps would make the directories and files only available on one host.  
> When using multiple hosts as a cluster a different approach is required, more on that in [Orchestration](#orchestration-docker-swarm--kubernetes).

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

#### Bind mount example
```sh
$CR container run --rm -d \
  --name webserver \
  -p 8080:80 \
  -v ./data:/usr/share/nginx/html:ro \
  docker.io/nginx:1-alpine
```

#### Named volume example
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


#### NFS mount example
Requires an NFS server.  
Change `<nfs-server-ip>` and `<path-to-nfs-shared>` and adjust mount options as needed.
```sh
$CR container run -d \
  --name nginx-nfs \
  -p 8080:80 \
  --mount 'type=volume,source=nfsvolume,target=/usr/share/nginx/html,volume-driver=local,volume-opt=type=nfs,"volume-opt=o=addr=<nfs-server-ip>,rw,nfsvers=4",volume-opt=device=:<path-to-nfs-shared>' \
  docker.io/nginx:1-alpine
```

#### NFS volume example
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


### Networking
Containers are isolated not only in terms of filesystem and processes, but also in terms of networking.  
By default, each container runs in its own network namespace.

Additionally container are also capable for outbound communication (e.g. access the internet).  
That is because the container runtime (Podman/Docker) sets up NAT (Network Address Translation) rules using `iptables`/ `nftables`:
  - Containers get an IP from the internal bridge network.
  - Outgoing packets are masqueraded as if they came from the host’s IP.
  - Replies from the internet are routed back to the container.
Analogy: It’s similar to how multiple devices share one home router IP:
  - Containers = PCs/laptops/phones in a home network
  - Docker bridge = home Wi-Fi router
  - Host system = the internet gateway

The default IP Address Ranges used to provide internal IPs to the containers are:
  - Podman: `10.88.0.0/16`
  - Docker: `172.17.0.0/16`

To make communication with container from the host possible, it is required to expose/map the desired ports in a container to ports on the host system.  
For that the option `-p <host-port>:<container-port>` or `--publish <host-port>:<container-port>` can be used (this option can be used any number of times, as long as `<host-port>` is always different).  
After making container ports available on the host, the host ports can be made accessible from beyond the host, by adding corresponding rules on the firewall/router.

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
> If the IP Address Range of the container runtime overlapps with e.g. the company IP Address Range,
> it is possible to encounter unexpected networking issues.  
> Therefore sometimes it is needed to overwrite the default IP Address Pool.  
> For docker this can be done in the [daemon configuration](https://docs.docker.com/reference/cli/dockerd/#daemon-configuration-file).
> For podman it can be defined in [/etc/cni/net.d/](https://github.com/containers/common/blob/main/docs/containers.conf.5.md#network-table)

Docker and Podman provide different networking modes that determine how containers communicate with each other and with the outside world.  
With `--network host` the network of the host system is used directly and with `--network none` networking can be completely disabled, but both of these options are rarely used.  
Additionally container can be in multiple networks.


#### Default Bridge Network
- In podman the default bridge network needs to be attached explicitly with `--network podman`.
- In docker when you run a container without specifying a network, it’s attached to the default bridge network.
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
WEB_SERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=webserver --quiet))
```

Access the webserver start page from a new container:
```sh
podman container run -it --rm --network podman busybox wget -qO- http://${WEB_SERVER_IP}
```
```sh
docker container run -it --rm busybox wget -qO- http://${WEB_SERVER_IP}
```

> [!IMPORTANT]  
> While the default bridge allows connectivity, it also allows any container to directly communicate with each other.  
> This can introduce potential security issues.  


#### Custom Networks and DNS
For actual network separation user-defined networks need to be created, with each container being added to it directly.  

In many cases, it is desired to isolate groups of containers so that only containers that belong together can communicate with each other.  
For example:
  - A database should only be reachable by the application container, not by unrelated containers running on the same host, like a frontend webserver.
  - Different projects should each have their own isolated networks, preventing accidental cross-communication.

```sh
$CR network create mynet
$CR container run --rm -d --name webserver --network mynet docker.io/nginx:1-alpine
WEB_SERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=webserver --quiet))
```

```sh
podman run --rm --cap-add=NET_RAW busybox ping ${WEB_SERVER_IP}
# this should fail, abort with ctrl+c

podman run --rm --cap-add=NET_RAW --network mynet busybox ping ${WEB_SERVER_IP}
podman run --rm --cap-add=NET_RAW --network mynet busybox ping webserver
# this should succeed

# ping needs the ability to open raw sockets,
# which normally requires either root privileges or the Linux capability CAP_NET_RAW
```
```sh
docker run --rm busybox ping ${WEB_SERVER_IP}
# this should fail, abort with ctrl+c

docker run --rm --network mynet busybox ping ${WEB_SERVER_IP}
docker run --rm --network mynet busybox ping webserver
# this should succeed
```

```sh
$CR container stop webserver
$CR network rm mynet
```

> [!NOTE]  
>  capabilities are explained in detail in the [Limit capabilities](#limit-capabilities) section.

> [!IMPORTANT]  
> Only when using a custom network will a container receive a DNS name.  
> The DNS name is always the name set when running the container and the hostname of the container.  
> This can especially lead to issues when running multiple instances of a container,
> which is relevant for deployments with `compose files`.  
> Therefore it is advised to either use a different hostname from the deployment name.  
> most of the time the hostname isn't needed, so not explicitly setting it is better.  
> Checkout the [Docker Swarm DNS](#dns) example for more details.

> [!IMPORTANT]  
> For podman version 3.4.4 which contains a bug for the network CNI plugin:  
> https://bugs.launchpad.net/ubuntu/+source/libpod/+bug/2024394  
> A workaround is to change the `cniVersion` for a network from `1.0.0` to `0.4.0`  
> The network config can be found in `~/.config/cni/net.d/<networkname>.conflist`
> Upgrading podman is possible but a bit tricky: https://github.com/containers/podman/discussions/25582

> [!NOTE]  
> Manually created networks aren't cleaned up automatically and take up disk space.  
> `$CR network prune` or `$CR network prune -f` can be used to remove all unused networks.


### Sharing namespaces between containers
Sometimes it is needed for two containers to communicate with each other directly using the same network namespace, instead of using a virtual network ot other types of namespaces.  
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


## Container Registries
A `Container Registry` is a centralized service for storing and distributing container images, which can be compared to App Stores or Package Registries (used by `apt`, etc.).

Registries are Configured in the following files:
  - Podman:
    - `etc/containers/registries.conf`
    - `/etc/containers/registries.conf.d/...`
  - Docker:
    - `/etc/docker/daemon.json`

The default `Container Registry` for Docker is [docker.io](https://hub.docker.com/) Podman has a set of predefined aliases which can be found in `/etc/containers/registries.conf.d/shortnames.conf`.  
The most common Container Registries are:
  - [docker.io](https://hub.docker.com) ([usage and limits](https://docs.docker.com/docker-hub/usage))
  - [quay.io](https://quay.io) ([rate limited](https://access.redhat.com/solutions/6218921))
  - `ghcr.io`  which is the Container Registry for GitHub Repositories, details can be found [here](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry)
  - GitLab Repositories can have a Container Registry similar to GitHub, details can be found [here](https://docs.gitlab.com/user/packages/container_registry)
  - Self Hosted Registries like:
    - [CNCF Distribution Registry](https://github.com/distribution/distribution)
    - [Harbor](https://goharbor.io)
    - ...

Sometimes a login is require into the desired `Container Registry` with `$CR login <registry>`.

> [!NOTE]  
> Every pulled container image, including individual versions, are stored locally and taking up disk space.  
> Unused container images are not automatically cleaned up and need to be removed manually.  
> All unused images can be removed with: `$CR container image prune`.  
> Or forceully wiht `$CR container image prune -a -f`.

> [!IMPORTANT]  
> Beware of the pull/rate limit of the publich container registries.  
> To check the current limit for `DockerHub` checkout the [Get DockerHub pull rate limit](#get-dockerhub-pull-rate-limit) section.

Shoutout to the [linuxserver](https://github.com/linuxserver) group for maintaining and providing many day-to-day applications as container images, via their own [lscr.io](https://docs.linuxserver.io/images-by-category/) registry.


### Example for a simple local Container Registry
https://distribution.github.io/distribution/

Setup requirements:
```sh
# Create directories
mkdir -p data/registry/{auth,data,certs}

# Create certificate for localhost
openssl req -x509 -new -nodes \
        -newkey rsa:4096 \
        -keyout data/registry/certs/server.key \
        -sha512 \
        -days 90 \
        -subj "/C=''/ST=''/L=''/O=''/OU=''/CN=localhost/emailAddress=''" \
        -out data/registry/certs/server.pem \
        -reqexts SAN \
        -config <(cat /etc/ssl/openssl.cnf <(printf "[SAN]\nsubjectAltName= DNS:localhost"))

# Create basic authentication credentials
$CR run \
  --entrypoint htpasswd \
  docker.io/httpd:2-alpine -Bbn testuser testpassword > data/registry/auth/htpasswd
```

> [!NOTE]  
> While `self-signed certificates` are fine for internal and development setups,
> in production setups proper valid certificates should be used.  
> One of the most well-known certificate provider would be [Let's Encrypt](https://letsencrypt.org/).

Add the certificate to trust store:
  - Podman:
    ```sh
    sudo mkdir -p /etc/containers/certs.d/localhost:5000
    sudo cp data/registry/certs/server.pem /etc/containers/certs.d/localhost:5000/ca.crt
    ```
  - Docker:
    ```sh
    sudo mkdir -p /etc/docker/certs.d/localhost:5000
    sudo cp data/registry/certs/server.pem /etc/docker/certs.d/localhost:5000/ca.crt
    ```

Running a simple registry locally:
```sh
$CR run -d \
  -p 5000:5000 \
  --restart=always \
  --name registry \
  -v "$(pwd)"/data/registry/certs:/certs:ro \
  -e REGISTRY_HTTP_TLS_CERTIFICATE=/certs/server.pem \
  -e REGISTRY_HTTP_TLS_KEY=/certs/server.key \
  -v "$(pwd)"/data/registry/auth:/auth:ro \
  -e "REGISTRY_AUTH=htpasswd" \
  -e "REGISTRY_AUTH_HTPASSWD_REALM=Registry Realm" \
  -e REGISTRY_AUTH_HTPASSWD_PATH=/auth/htpasswd \
  -v "$(pwd)"/data/registry/data:/var/lib/registry \
  registry:3

# $(pwd) -> current directory
```

Log in to the registry:
```sh
$CR login localhost:5000

# User: testuser
# Pass: testpassword
```

Pull an image:
```sh
$CR image pull alpine:3
```

Tag existing image to point to the local registry:
```sh
$CR image tag alpine:3 localhost:5000/myfirstimage:3
```

Upload (push) the image:
```sh
$CR image push localhost:5000/myfirstimage:3
```

Remove the locally stored image:
```sh
$CR image rm alpine:3
$CR image rm localhost:5000/myfirstimage:3
```

Download (pull) the images:
```sh
$CR image pull localhost:5000/myfirstimage:3
```

Stop the Registry:
```sh
$CR logout localhost:5000
$CR container stop registry
$CR container rm registry
```



## Container Images
An image is like a snapshot of an environment, it contains most importantly:
  - application code or a binary
  - packages (optional)
  - libraries (optional)
  - runtimes (Python, Java, ...) (optional)
  - entrypoint/command instructions

As mentioned before, a fully static binary is enough, but it is also possible to include an application as code with all dependencies.

Containers themselves are ephemeral and can be replaced, but images are reproducible artifacts that can be shared and reused.

The name of a `Container Image` consists of three parts:
  - `registry`: The server or service where the image is stored (can be omitted, if it points to the default registry or has an alias)
  - `repository`: The name of the repository where the image is stored
  - `tag`/`digest`: A label that identifies a specific version or variant of the image(if omitted, defaults to `latest`)

Combined it would be:
  - `<registry>/<repository>:<tag>`
  - `<registry>/<repository>@sha256:...`
  - sometimes registries use namespaces, which results in:
    - `<registry>/<namespace>/<repository>:<tag>`
e.g.: `docker.io/library/alpine:3`

> [!IMPORTANT]  
> Using `latest` should be avoided, as it can break deployment/application dependencies.

> [!IMPORTANT]  
> In some cases even specifying a Version may not suffice.  
> e.g.: `python` container images also use suffixes like `-trixie` to specify the underlying image OS.


### Container Image Specification
The `Open Container Initiative (OCI)` defined open standards for Container Images, which can be found [here](https://github.com/opencontainers/image-spec).

Key points:
  - An OCI image is essentially a `tar archive` of a root filesystem plus a `manifest` describing how to run it
  - Images usually consists of:
    - `Metadata`:
      - entrypoint
      - variables
      - work directory
      - ...
    - `Layers`:
      - Each layer represents filesystem changes (like adding a file or installing a package)
      - Layers are cached and reused between images, reducing storage and speeding up builds
    - `Manifests`:
      - Contain which layers make up the image
      - Multiple manifests can be contained within an image, each specifying e.g.:
        - For what `architecture` the image has been build for (amd64, arm64, ...)
        - For what `OS` the image has been build for (linux, windows, ...)

> [!IMPORTANT]  
> All layers are part of the final image,  
> so adding a file or package in one layer and removing it in the next doesn't reduce the final image size.  
> Only doing this in the same layer affects the final image size.

> [!IMPORTANT]  
> Images can take up a major part of disc space, because there is no automatic cleanup.  
> `$CR system df` can be used to check the current used disk space.  
> `$CR image prune` can be used to cleanup unused images.  
> Or forceully wiht `$CR container image prune -a -f`


### Pulling a Container Image
Pulling a `Container Image` will store it locally in the aforementioned directories:
```sh
$CR image pull alpine:3
```

### Inspecting a Container Image
Inspect detailed information about a `Container Image`, like the digest, the OS and architecture it is build for, it's default configuration (env vars, start command, etc.), Labels, Annotations and more.
```sh
$CR image inspect alpine:3
```

Show the layer history of a `Container Image`, which essentially includes the steps done to build it.
```sh
$CR image history alpine:3
```

Inspect the manifests of a `Container Image`:
```sh
$CR manifest inspect alpine:3
```
Like this Docker doesn't show the annotations and other details for each manifest, it only does so when either using `-v` or by specifically inspecting a manifest by using the `digest` instead of a `tag`.

### Managing local Container Images
List all locally available `Container Images`:
```sh
$CR image ls
```

Remove a `Container Image`:
```sh
$CR image rm alpine:3
```

Remove all unused (dangling) `Container Images`:
```sh
$CR image prune
# $CR image prune -a -f
```

Export a `Container Image` to an archive:
```sh
$CR image save alpine:3 -o alpine3.tar.gz
```

Import a `Container Image` to an archive:
```sh
$CR load < alpine3.tar.gz
```
```sh
$CR load --input alpine3.tar.gz
```



## Dockerfile/Containerfile and Container Image building
### Basics
While there are many prebuilt images, most real-world projects need custom images.  
The recipe for building an image is most commonly called a `Dockerfile`, which is also the default filename.  
`Containerfile` is part of the [container-libs](https://github.com/containers/container-libs/tree/main) and uses the same syntax, it is what `podman` and it's buildkit `buildah` defaults to.  
Docker hs it's own buildkit `buildx`.

The reference fo both can be found here:
  - [Dockerfile](https://docs.docker.com/reference/dockerfile)
  - [Containerfile](https://github.com/containers/container-libs/blob/main/common/docs/Containerfile.5.md)

For simplicity `Dockerfile` will be used as naming for the examples.

Here is an overview of the more common options:
```Dockerfile
# The base image that should be used as foundation.
# Checkout the different available container registries for fitting container images
# "scratch" is an empty image, that is always available
FROM alpine:3

# Copy static content into the container
COPY <dir-on-host>/<file-on-host> <dir-in-container>/<file-in-container>

# ADD functions similar to COPY but can be used to get files from URLs
ADD https://example.com/archive.zip <dir-in-container>/<file-in-container>
ADD git@github.com:user/repo.git <dir-in-container>/<file-in-container>

# Set the user to be used in the container image.
# The user and group need to be created first, e.g.:
#   RUN addgroup -S <group> && adduser -S <user> -G <group>
USER <user>:<group>

# The RUN instruction will execute any commands to create a new layer on top of the current image.
RUN && apk update \
    && apk upgrade \
    && apk add --no-cache --update bash \
    && rm -rf /var/cache/apk/*

# Mounted secrets (and the created files in '/run/secrets/') are only available in the layer they are added.
# This is the only way to securely add secrets into a Container Image without leaving any traces of it.
# In the build command add the "--secret" option, e.g.: -secret id=SECRET1,env=SECRET1
# Add '&&' only for second and later commands after '--mount', since the mount is not run as a command and filtered out.
RUN --mount=type=secret,id=SECRET1 \
    --mount=type=secret,id=SECRET2 \
    export SECRET1=$(cat /run/secrets/SECRET1) \
    && export SECRET2=$(cat /run/secrets/SECRET2) \
    && echo "Secret test: $SECRET1 & $SECRET2"


# Expose port, defaults to "/tcp", "/udp can also be specified.
# If both should be exposed, add a second entry.
# Doesn't actually publish the port, it functions as a type of documentation.
EXPOSE 80

# A healthcheck can be used to actively check if the container is still working (e.g.: run cURL against an API it is providing).
# While useful, configuring them in the Dockerfile is Docker specific and not in the OCI standard.
# The healthcheck should be defined in the compose or kubernetes file
HEALTHCHECK [OPTIONS] CMD <command>

# The executable and paramters run at the start of the container, can be overwritten with "--environment ''".
ENTRYPOINT ["ls", "-a"]
# ENTRYPOINT ["executable", "param1", "param2"]

# The instructions followed by ENTRYPOINT
CMD ["-l", "-h"]
# CMD ["executable","param1","param2"]
# CMD ["param1","param2"] # Default parameters when ENTRYPOINT is used
# For ENTRYPOINT and CMD it is not too uncommon that only one of them is used, often only CMD.
# ENTRYPOINT is sometimes also used to run a script like "docker-entrypoint.sh", which setups the environment and CMD running the actual application.
#   postgres (https://hub.docker.com/_/postgres) and mariadb (https://hub.docker.com/_/mariadb) are quite known for this.
# Use ENTRYPOINT if the same executable should be run every time, e.g.: when arguments are provided to the "docker run".
```

The basic build command is:
```sh
$CR build -t mycontainer .
# -t sets the tag of the image
```

During build each layer is cached to reduce build time.  
Sometimes this can lead to issues when some changes are not automatically found, but caching can be disabled with the `--no-cache` option.

For running the build container, the set tag can be used:
```sh
$CR run mycontainer
```

If deesired, it is also possible to build a container and output it into a local directory:
```sh
$CR build -t myimage:1 -f Dockerfile.slim  -o "type=local,dest=myimage-1" .
```

Checkout the [hello-world](dockerfiles/hello-world) for some different coding language examples in the [dockerfiles](dockerfiles/) directory.

> [!IMPORTANT]  
> For running some kind of API as application use `0.0.0.0:<port>`.  
> This makes the API properly available in the container and accessible when connected to the host or to other container.

> [!IMPORTANT]  
> When building images containing code, be careful what bas eimage is used.  
> Images like `alpine` for example use a different `c-compiler` than e.g. `debian`.  
> This can also be true for different compiler or packages.

> [!NOTE]  
> By default during build all directories and files in the location of the `Dockerfile` will be loaded before the actual build starts.
> Depending on the amount and size of the files, this can take a while and may also lead to unintended side effects/security issues.
> This can be prevented by adding a `.dockerignore` next to the `Dockerfile`

> [!NOTE]  
> On some setups there might be difficulties when accessing the internet when building a container.  
> One way to circumvent that is by using the `--network=host` option.  
> While finding and fixing the responsible issue is better, as a quick fix this also works.

It is possible to define multiple stages, where each stage has their own image to build a container image.  
The syntax for that is like this:
```Dockerfile
FROM <some-iamge> AS <first-stage-name>

FROM <another-iamge>
COPY --from=<first-stage-name> /<path-in-builder> /<path-in-second-stage>
```
For this the first stage will be build first and the second stage can copy things from the first stage, like a static binary, into it's own space.  
The final image will only contain the second stage and not the first stage.  
It is also possible to have more stages and also to use the `--target` option to specify which stage is the one to be build at the end.

> [!NOTE]  
> Each stage creates untagged images, which is nice for caching purposes,
> but can clogg up the image list.  
> Removing all images of a specific stage can be done with: `$CR image prune --filter label=stage=<first-stage-name>`

Checkout the [Dockerfile.multi-stage.yml](dockerfiles/Dockerfile.multi-stage.yml) as an example.


### Muli-arch Container Images
Modern images are often built for multiple architectures (e.g., amd64, arm64).  
When building for multiple architectures, it creates a manifest list that points to multiple images, one per architecture.  
Registries and runtimes then automatically pull the right one for the platform.
This is critical in environments that consist of mixed architectures (e.g., x86 servers, ARM-based laptops, Raspberry Pi clusters).  
Sadly the buildkits are limited to either directly push the build image into a container registry or output it into a directory.

To run different architectures locally the QEMU emulation support (binfmt_misc) needs to be enabled in the kernel.  
This can be done by running the `qemu-user-static` container:
```sh 
sudo $CR run --rm --privileged docker.io/multiarch/qemu-user-static --reset -p yes 
```
This works because the container shares the `kernel` with the host system and therefore if it changes things on the kernel,
like enabling the QEMU emulation support, the host system is also affected by it.  
Which is why especially for `podman` the container has to be run as root to have the required permissions.

Test it:
```sh
$CR run --rm --platform=linux/arm64 alpine:3 uname -m
# aarch64

$CR run --rm --platform=linux/amd64 alpine:3 uname -m
# x86_64
```

> [!NOTE]  
> It is also possible to build images for different architectures on separate hosts
> and combine them by creating a manifest.


#### Podman

Enable the QEMU emulation support (binfmt_misc):
```sh 
sudo $podman run --rm --privileged docker.io/multiarch/qemu-user-static --reset -p yes 
```

```sh
podman build \
  --platform linux/amd64,linux/arm64 \
  -t localhost:5000/mymultiatch:0.1 \
  .
```

Check for which architectures the image is available for:
```
podman manifest inspect localhost:5000/mymultiatch:0.1
```

#### Docker
Dockers `buildx` is capable to build multi-arch images with a custom builder, that uses individual container to build each architecture. 
Therefore it doesn't need the QEMU emulation support (binfmt_misc) installed on the host.  

Create a builder:
```sh
docker buildx create --name builder
docker buildx inspect builder --bootstrap
docker buildx use builder
```

Build the images and push it to the registry:
```sh
docker buildx build \
  --push \
  --platform linux/amd64,linux/arm64 \
  --output=type=image,push=true,registry.insecure=true \
  -f Dockerfile \
  --tag localhost:5000/mymultiatch:0.1 .
```

Check for which architectures the image is available for:
```sh
docker manifest inspect localhost:5000/mymultiatch:0.1 -v
```

Build the image and save it to the same directory:
```sh
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --output=type=local,dest=$PWD \
  -f Dockerfile \
  --tag mymultiatch:0.1 .
```

Stop the custom builder:
```bash
docker container stop buildx_buildkit_builder0
```


### Best practices

#### Use annotations during build
The [container image spec](https://github.com/opencontainers/image-spec) also specifies some `annotations` that are recommended to be added during the build process.  
These include information like who build the image, where can the source code be found, etc., an overview can be found [here](https://github.com/opencontainers/image-spec/blob/main/annotations.md).

They can be added during build with the `--annotation` option:
  - `--annotation <some-key>=<some-value>`
For general information about the image the `manifest-descriptor:` prefix should be used:
  -  `--annotation manifest-descriptor:org.opencontainers.image.url=<url-to-source>`

> [!NOTE]  
> Using labels for these information is possible and still recommended and used by some,
> but has been superseded by annotations.

> [!IMPORTANT]  
> For `GitLab` instances that don't have the [Container registry metadata database](https://docs.gitlab.com/administration/packages/container_registry_metadata_database/),
> using `manifest-descriptor` breakes some of the featuers in the Container Registry, like the pulish date and image size.
> Currenlty the Container registry metadata databaseis only mandatory in versions from the later halve of `2027`: [epics: #5521](https://gitlab.com/groups/gitlab-org/-/epics/5521)

#### Healthchecks
While the `HEALTHCHECK` option in the Dockerfile/Containerfile is `docker` specific it is good to add some form of capability that allows running a healthcheck in the deplyoment configuration.  
For APIs this is quite simple when tools like `curl` or `wget` are available in the container.  
For other types of applications it has to be implemented differently by adding options to e.g. trigger a function that checks if the application is stil responding
or something to check if the process is working.

#### Reduce image size
Reducing the image size helps save space and cost on the host system.  
It can be done easily by doing small adjustments when building the container image.  
Additionally there are also tools like [slim](https://github.com/slimtoolkit/slim), which can be useful for reducing image size.

##### Container image layer optimizastion
A simple way of reducing the container image size is by optimizing the individual layers.  

When installing a package via a package registry, the package list should always be updated, but this also leads to the list still being contained in the image:
```Dockerfile
RUN apt update
RUN apt upgrade -y \
  && apt install htop
```

Dont do it like this:
```Dockerfile
RUN apt update
RUN apt upgrade -y \
RUN apt install htop \
RUN apt upgrade -y \
RUN apt autoclean -y \
RUN apt autoremove -y \
RUN apt clean -y \
RUN rm -rf /var/lib/apt/lists* /tmp/* /var/tmp/*
```
The layers stack on top of each other, so while the end result might look good, the individual layers still contain the things that are removed at a later step.

Do it like this:
```Dockerfile
RUN apt update \
  && apt upgrade -y \
  && apt install htop \
  && apt autoclean -y \
  && apt autoremove -y \
  && apt clean -y \
  && rm -rf /var/lib/apt/lists* /tmp/* /var/tmp/*
```
Here the unneeded files are immediately removed and no space is wasted.

Checkout these example:
  - [Dockerfile.clean-apk](dockerfiles/Dockerfile.clean-apk)
  - [Dockerfile.clean-apt](dockerfiles/Dockerfile.clean-apt)
  - [Dockerfile.clean-python](dockerfiles/Dockerfile.clean-python)


##### Use multi-stage builds
With multi-stage builds it is easily possible to reduce the image size, because only the final stage is included in the image.  
This is quite useful if, e.g.: an application that is build in one stage doesn't need all the dependencies to run compared to when it is build.

Checkout these example:
  - [Dockerfile.multi-stage.yml](dockerfiles/Dockerfile.multi-stage)
  - [Dockerfile.non-root](dockerfiles/Dockerfile.non-root)
  - [Dockerfile.non-root-scratch](dockerfiles/Dockerfile.non-root-scratch)

> [!NOTE]  
> For applications with outgoing connections like `HTTPS`,
> It is usually required to have the `public certificates` of the target URL:
>   - For normal valid certificates it usually suffices to install the `ca-certificates` package
>   - For `self-signed` certificates they need to be added directly
> Checkout the [Dockerfile.scratch-with-certificates](Dockerfile.scratch-with-certificates) example.

> [!NOTE]  
> Each stage results in a stored untagged image.  
> They can be cleaned up with: `$CR image prune --filter label=stage=builder`


##### Use small base images
While it might seem intuitive for some to use images like `debian`, there are often smaller base images available, like:
  - Instead of `debian:13` use `debian:13-slim`
  - Instead of `python:3.13` (based on debian) use `python:3.13-slim` or switch to an `alpine` base image: `python:3.13-alpine`

`alpine` is one of the smallest Linux distributions and great to use in a container environmet.  
But be careful that things like the `c-compiler` can differ compared to other distributions.

There are also the [distroless](https://github.com/GoogleContainerTools/distroless) images which can be used for a wide varity.

But nothing beats using an empty `scratch` image that only contains a static binary, though that is not always easily achievable.  
For applications with outgoing `HTTPS` connections add the `ca-certificates`, like in the [Dockerfile.scratch-with-certificates](Dockerfile.scratch-with-certificates) example.

Checkout the examples in the [Use multi-stage builds](#use-multi-stage-builds) section.



#### Use non-root
Using a non-root user in a container improves security because:
  - In some cases, having root as user can be escalated to the root on the host (e.g., through kernel or runtime escapes)
  - A non-root process inside a container limits what can be read, written, or executed
  - It can prevent accidental (or malicious) interference with other workloads (e.g. when using mounts)
  - It follows the `Least Privilege Principle`:
    - Most applications don’t need root access to run
    - Running as a less-privileged user means even if compromised, the attacker has fewer permissions

Checkout these example:
  - [Dockerfile.non-root](dockerfiles/Dockerfile.non-root)
  - [Dockerfile.non-root-scratch](dockerfiles/Dockerfile.non-root-scratch)

#### Version pinning
Wherever possible use version pinning:
  - the base container image
  - the installed packages
  - the installed dependencies
  - ...

Using `latest` can always lead to breaking changes when rebuilding a container image.

Most images use semantic versioning and for these at least the used `major` or `minor` version should be specified.  
For images like `python` who additionally have the os version as suffix like `-trixie` or `-alpine`,
it can also lead to breaking changes if these are not specifically specified.

The same goeas for packages and dependencies.  
Package manager like `apt` and `apk` support version pining with `package==<version>`.  
For `apk` it is also possible to use `~=` to not have to define `patch` or `minor` versions.

Check for available packages here:
  - [Packages for Linux and Unix](https://pkgs.org/)
  - [alpine package list](https://pkgs.alpinelinux.org/packages)

Additionally for dependencies like in `python` or `go` should have their version pinned:
  - [Python pypi](https://pypi.org/)
  - [Go packages](https://pkg.go.dev/)

Proper version pinning wherever possible adds better reproducability of container images
and simplifies fixing security issues, by exactly knowing what package versions are used.


#### Application profiling
It is possible to limit the provided `CPU` and `RAM` when running container.  
This is especially important for applications that scale horizontaly.  
Only by doing so is it possible to properly calculate how big the host system or cluster is needed to be.

For that there are many application profiling tools like [scalene](https://pypi.org/project/scalene/) or [cProfile](https://docs.python.org/3/library/profile.html) for `python`.  
Additionaly with `$CR stats` it is possible to check the currently used `CPU` and `RAM` for each running container.

For `kubernetes` the `top` command can be used:
  - `kubectl top pod <POD_NAME> --containers`
  - `kubectl top node <NODE_NAME>`

> [!NOTE]  
> The `kubectl` command is comparable to the `podman` and `docker` commands.  
> Just that is used to interact with Kubernetes.

Lastly, when developing an application that is `multi-processing/-treading` it might happen that dangling processes or threads are not removed properly.  
For that checkout the [docker_process_count.bash](scripts/docker_process_count.bash) script which can output the total amount of threads and processes for each running container.

#### Security
There are tools like [trivy](https://github.com/aquasecurity/trivy) and [osv-scanner] (https://github.com/google/osv-scanner) that should be used to check container images for vulnerabilities.

> [!IMPORTANT]  
> The vulnerability checks are always a snapshot at a specific point in time.  
> Just because there was no vulnerability found during the initial check,
> That doesn't mean that there will not be any vulnerability found in the future.  
> Vulnerability checks should be done on a regular and continuous basis.

#### Sign the container images
With tools like [cosign](https://github.com/sigstore/cosign) have the possibility to sign contaier images.

```sh
BINARY_NAME="cosign-linux-amd64"
GIT_PROJECT=sigstore/cosign

curl -L -o cosign $(curl -IkLs -o /dev/null -w %{url_effective} https://github.com/$GIT_PROJECT/releases/latest/download)/$BINARY_NAME
chmod +x cosign

# Generate Keypair
./cosign generate-key-pair

# Sign image
./cosign sign -y --key cosign.key localhost:5000/myfirstimage:3

# Verify
./cosign verify --key cosign.pub localhost:5000/myfirstimage:3
```

#### Automate the build process
Following all best practices can make the developement process tedious, which is why automating the process is highly recommended:
  - For `GitHub` there are a varity of [GitHub Actions](https://docs.github.com/en/actions) available.
  - For `GitLab` there are also a varity oc [CI Templates](https://docs.gitlab.com/ci/examples/) available.

> [!NOTE]  
> Checkout this GitLab CI Component:  
> [Container CI Component](https://jugit.fz-juelich.de/iek-10/public/developer-tools/gitlab-ci-components/container)



## Compose files
While using `docker run` is fine for testing and development,
when it comes to managing multiple containers it gets out of hand quickly and becomes unmanageable.  
There are many pieces that can go wrong,
a missing container or network,
a typo when entering the command,
incomplete copying from a documented command and so forth.

This is where `Compose files` come in.  
A `Compose file` is a `YAML` configuration file that describes how to run a set of containers as services.  
The default filename is `docker-compose.yml`, but can differe if needed.

It can be used to define `services`, `networks`, `volumes`, and more in a declarative way.  
Like with CLI commands there are short and long versions that can be used in a compose file.  
To give a better detailed overview the long versions are used in the examples.

> [!NOTE]  
> For older distributiins like `Ubuntu 22.04` it might be required for `podman` to manually install the compose plugin.
> Installation instructions can be found [here](https://github.com/containers/podman-compose).  
> In that case it might use the `podman-compose` command instead of `podman compose`.

A minimal webserver example can look like this:
```yaml
# The services section can contain one or more service definitions
# Each individual service can contain one or more container
services: 
  web:  # This is the name of the service, which can be set to anything
    image: docker.io/nginx:1-alpine # The image used for the container of the service "web"
    ports:  # A list of ports to be exposed
      - target: 8080  # Port on the host system
        published: 80 # Port in the container
        protocol: tcp # The used protocol
        mode: ingress
        # The mode is important for a Docker Swarm setup.
        # "ingress" mode (default) means the port is exposed on one host and Docker Swarm uses loadbalancing
        # "host" mode publishes the port on all nodes
    volumes:  # A list of volume mounts simliar to "docker run"
      - source: ./data
        target: /usr/share/nginx/html
        type: bind
        read_only: true
        # For the volume type multiple options exist,
        # but the most common are "bind" and "volume"
        # "bind" is for direct mounts of directories and files from the host system into the container
        # "volume" is for named volumes, they can be setup and declared on the same level as "services"
    environment:  # "environment" contains a mapping for all env variables to be set in the container
      SOME_KEY: <some-value>
    healthcheck:  # A healthcheck can be used to run a continuous test if the container is still functioning
      test: wget -q --spider http://127.0.0.1:80
```

By default if in the same directory as the compose file a `.env` variable exists, it will be read automatically.
The read environment variables can than be set as values for most options in the compose file with `$var-name`.  
They work similar to `shell variables` and can have defaults defined `${var-name:-default-value}`

Further options can be found in the [Docker Compose Reference](https://docs.docker.com/reference/compose-file/).

Checkout the following examples:
  - [docker-compose.registry.yml](compose-files/docker-compose.registry.yml)
  - [docker-compose.sidecar.yml](compose-files/docker-compose.sidecar.yml)
  - [docker-compose.webapp-and-storage.yml](compose-files/docker-compose.webapp-and-storage.yml)

Starting the services in the compose file can be done with `$CR compose up -d` or:  
```sh
$CR compose -f compose-files/docker-compose.registry.yml up -d
```

Stop them with:
```sh
$CR compose -f compose-files/docker-compose.registry.yml up -d
```

### Networking
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
$CR container run --rm -it --net=container:$($CR container ls --filter name=webserver-1 --quiet) busybox nslookup webserver-1
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
> Checkout the [Docker Swarm DNS](#dns) example for more details.


### Best practices

#### YAML anchor
Since `compose files` use the `YAML` syntax it is possible to use `YAML anchor` to reduce repetitive code.

Checkout the [docker-compose.yaml-anchor.yml](compose-files/docker-compose.yaml-anchor.yml) example.  
For further details check the [YAML reference](https://yaml.org/).

### Update config in parallel
When deploying multiple replicas of a service use the `update_config` option
to define how many services are allowed to be updated at the same time:
```yml
services:
  redis:
    image: valkey/valkey
    deploy:
      replicas: 6
      # placement:
      #   max_replicas_per_node: 1
      update_config:
        parallelism: 2
        delay: 10s
      restart_policy:
        condition: on-failure
```

### Healthchecks
Use healthchecks whenever possible to ensure that deployments are properly functional.

Get the Healthcheck logs:
```sh
$CR inspect $($CR container ls --filter name=webserver --quiet) --format "{{json .State.Health }}" |
grep -Eo '"[^"]*" *(: *([0-9]*|"[^"]*")[^{}\["]*|,)?|[^"\]\[\}\{]*|\{|\},?|\[|\],?|[0-9 ]*,?' |
awk '{if ($0 ~ /^[}\]]/ ) offset-=4; printf "%*c%s\n", offset, " ", $0; if ($0 ~ /^[{\[]/) offset+=4}'
```
- The `inspect` section extracts the `.State.Health` field from a running container named `webserver`
- The `grep` section takes the returned `JSON` output from the `inspect` and breaks it into multiple lines
- The `awk` section basically makes a pretty-print of the content
For simplicity the package `jq` can also be used instead of `grep` and `awk`.


### Limit logging
In the logging section of a service it is possible to configure how logs from the container should be handeld.  
By default it doesn't limit the amount of logs which can completly fill the host disk space.  
Therefore limiting logs is always recommended:
```yaml
    logging:
      driver: "json-file"
      options:
        max-file: "5"
        max-size: "10m"
```

### Limit capabilities
Capabilities are distinct units of privilege in the kernel.  
They are things like the ability to send raw IP packets, or bind to ports below 1024.
Most capabilities are required to manipulate the kernel/system, and these are used by the container runtime, but seldom used by the processes running inside the container.

Therefore to increase security it is recommended to enable as few capabilities as needed when running a container.

Get default Capabilites:
```sh
$CR run --rm fedora capsh --print
# $CR  run --network=host --rm alpine sh -c 'apk add -U libcap; capsh --print'
```

Drop all capabilities with `--cap-drop=ALL`:
```sh
$CR run --rm --cap-drop=ALL fedora capsh --print
```

Only add necessary capabilities:
```sh
$CR run --rm --cap-drop=ALL --cap-add=setuid --cap-add=setgid fedora capsh --print
```

For compose files use:
```yml
services:
  webserver:
    image: docker.io/nginx:1-alpine
    cap_add:
      - ...
    cap_drop:
      - ...
```

More information about capabilites can be found here:
  - [Linux capabilities man page](https://man7.org/linux/man-pages/man7/capabilities.7.html)
  - [Docker runtime privilege and linux capabilities](https://docs.docker.com/engine/containers/run/#runtime-privilege-and-linux-capabilities)
  - [Docker compsoe capabilites reference](https://docs.docker.com/reference/compose-file/services/#cap_add)

### Set resource limits
By default, containers can use as much CPU and memory as the host allows. This can lead to:
  - One misbehaving container consuming all RAM and causing the host to swap/crash.
  - One container monopolizing CPU and starving others.
  - Scaling of a service leading to resource exhaustion on the host

The limits can be defined like this:
```yaml
services:
  webserver:
    image: docker.io/nginx:1-alpine
    deploy:
      resources:
        reservations: # Soft guarantee of the ressources if available
          cpus: '0.25'  # Percent of a Core
          memory: 20M
        limits: # Hard limit of ressources that can be used, exceeding it can lead to the service being killed
          cpus: '0.50'
          memory: 50M
```
Often it is not easy to properly determine how many ressources an applicaiton needs to properly run.  
For applications with a predefined load this can be done easily,
but for applications like web services the load can differ vastly making it more difficult.  
Since exceeding the ressource limits lead to the service being killed, this option should be used carefully.


## Orchestration: Docker Swarm & Kubernetes
Running containers with `docker run` or `compose files` is fine for local development, but production environments need:
- Scalability - automatically running more replicas when demand grows.
- Fault tolerance - restarting containers on failure, redistributing workloads if a node goes down.
- Service discovery & networking - assigning DNS names and load balancing between replicas.
- Configuration & secrets management - distributing environment variables, certificates, and keys.
- Rolling updates & rollbacks – upgrading without downtime.

This is where orchestration platforms come in.

### Docker Swarm
Docker Swarm is Docker’s built-in orchestrator. It’s simpler than Kubernetes and good for small to medium clusters.
  - Cluster setup: A Swarm is initialized with `docker swarm init`, then other nodes join as managers or workers.
  - Deployment: Applications are deployed with `docker stack deploy -c docker-compose.yml myapp`.
  - Scaling: Each service can be scaled dynamically (`docker service scale myservice=5`), but these changes don't persist if a stack iss stopped and restarted.
  - Networking: Uses overlay networks that span all nodes; services are reachable by DNS names.
  - State management: `Swarm managers` maintain desired state, automatically reconciling failed tasks.
  - Replicas of a `service` are called `tasks`

Limitations:
  - Development of Swarm has slowed significantly.
    - Many bugs have accumulated over the years.
    - some `compose` features are not available or are broken in Swarm.
  - Lacks advanced features like namespaces, CRDs, or mature ecosystem integration.
  - Best for small/simple setups or training, but not the industry standard today.

With `docker stack ps <stack-name>` it is possible to check the state of a stack.  
This also shows `tasks` that failed previously, which can get lengthy for some deployments.  
To only see the running `tasks`:
```sh
docker stack ps <stack-name> --format "table {{.ID}}\t{{.Name}}\t{{.CurrentState}}\t{{.Node}}" --filter "desired-state=running"
```

> [!Important]  
> Docker Swarm doesn't automatically read `.env` files, a workaround is to load that file beforehand so that it gets passed into the deployment:  
> `export $(grep -v '^#' .env | xargs) > /dev/null 2>&1; docker stack deploy <stack-name> && unset $(grep -v '^#' .env | sed -E 's/(.*)=.*/\1/' | xargs)`



#### Logging
There are two main options when checking logs of a container:
  - `docker service logs <service-name>`
  - `docker logs <task-name>`

The `service` logs combine the logs of all the underlying `tasks`.  
Important to note is that the service logs can be quite difficcult to read, becasue they are just lumped together how they are received and not ordered.

Reading the logs on one node can often be easier to do:
`docker logs $(docker container ls --filter status=running --filter name=<task-name> --quiet) -n 50 2>&1 | grep -i "error"`


#### Docker Swarm data persistence
When considering data persistence in Docker Swarm it is important to node that `mounts` and `named volumes`
are only done on one specific node.  
While `named volumes` are created automatically in case of a `failover`, the data is not transfered.  
THis can lead to inconsitent data and other issues and should be considered carefully.

To replicate small things like configuration or password files the `config` and `secret` optionms can be used.  
But both of them can not be changed as long as the stack is running,
which makes them not a good solution for e.g. certificates that rotate regularly.

As a workaround trick for general files, it is possible to add a `service` that is deployed in `global` mode (on all nodes),
and than creates files to a `named volume` where that volume is also attached to the actual service.  
This concept is an `init container`.

A better option is to use network storage like `NFS` or `S3` or cluster-aware storage like `GlusterFS`.

For clustered services (recommended for `databases`) it would be recommended to deploy them on all nodes and make them store their data on each node.

> [!NOTE]  
> The volume type `cluster` was added in the [Docker Engine API v1.42](https://docs.docker.com/reference/api/engine/version-history/#v142-api-changes)
> and is listed as option in the [Compose Reference](https://docs.docker.com/reference/compose-file/services/#long-syntax-6),
> but so far is barely explained and poorly documented.

> [!NOTE]  
> `NFS` only supports authentication from versions `4` onwards.  
> For older versions IP whitelisting and firewall rules are the only option.


#### Docker Swarm Networking
In Swarm mode networks are usualy of the `overlay` type making them availavble on all nodes.  
Important to note that manually attaching a container to a network is not possible by default.  
To make this possible the option `--attachable` has to be added.

##### Overlay vs Bridge network
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



### Kubernetes
Kubernetes (k8s) is the de facto industry standard for orchestration.
  - Originated at Google as Borg, later open-sourced.
  - Highly extensible, modular and powerful, but more complex than Swarm.
  - [6 month livecycle](https://endoflife.date/kubernetes) for a kubernetes distribution
  - Supports:
    - Declarative YAML manifests
    - Self-healing workloads
    - Advanced networking, storage, and scaling
    - Operator for autoamated deployment and livecycle manager
    - Huge ecosystem ([CNCF landscape](https://landscape.cncf.io/))
      - [HELM](https://helm.sh/) to manage Kubernetes applications
      - [ArtifactHUB](https://artifacthub.io/) as central marektplace for `HELM` templates
      - [OperatorHub](https://operatorhub.io/) as central marketplace for operator

Lightweight distributions variants:
  - [rke2](https://github.com/rancher/rke2) – Hardened single binary Kubernetes distribution from Rancher.
  - [k3s](https://github.com/k3s-io/k3s) – Minimal single binary Kubernetes, great for edge, IoT, CI and developement.
  - [k3d](https://github.com/k3d-io/k3d) – Run k3s inside Docker, great for dev/test.


The modularity and extensiblity of Kubernetes adds a high level of komplexity to maintenace and developement.  
With a 6 month livecycle it would be foolish to do things manually.  
There are many tools like the [System Upgrade Controller](https://github.com/rancher/system-upgrade-controller)
that help automatizing things, but they also add complexity and need to be maintained.

The lightweight k8s distibutions take a way a lot of complexity,
but it still requires a lot of preparation work to get a well functioning system with minimal maintenance overhead.


#### Running with Podman
While limited it is possible to run Kuberentes files and Helm charts with podman:

Create a kuebrentes file form Helm template:
```sh
helm template webserver ./helm-files > ./data/webserver-k8s.yaml
```

Run the deployment:
```sh
podman kube play --replace data/webserver-k8s.yaml
```

Check state:
```sh
podman pod ps
podman ps
```

Stop the deployment:
```sh
podman kube play --down data/webserver-k8s.yaml
```

> [!NOTE]  
> Older Podman Version (like 3.4.4) use different commands:
>   - `podman play kube data/webserver-k8s.yaml`
>   - `podman pod stop webserver-pod-0`
>   - `podman pod rm webserver-pod-0`


## General Best practices

#### File descriptor limits
Many Linux distributions limit open files per process to `1024`.  
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

#### DNS responses for outgoing connections
For outgoing connections when using `DNS names` repeated DNS querries are done,
because the responses usually are not stored properly.  
This can flood `DNS servers` which should be avoided.

here is an example for `Docker` to use the hosts `systemd-resolved`:
  - Get the `docker0` network ip: `ip a s` (default: `172.17.0.1`)
  - Create the following configuration in`/etc/systemd/resolved.conf` (owner `root`, file permission `644`):
    ```ini
    [Resolve]
      # Some examples of DNS servers which may be used for DNS= and FallbackDNS=:
      # Cloudflare: 1.1.1.1#cloudflare-dns.com 1.0.0.1#cloudflare-dns.com 2606:4700:4700::1111#cloudflare-dns.com 2606:4700:4700::1001#cloudflare-dns.com
      # Google:     8.8.8.8#dns.google 8.8.4.4#dns.google 2001:4860:4860::8888#dns.google 2001:4860:4860::8844#dns.google
      # Quad9:      9.9.9.9#dns.quad9.net 149.112.112.112#dns.quad9.net 2620:fe::fe#dns.quad9.net 2620:fe::9#dns.quad9.net
      #DNS=
      #FallbackDNS=
      #Domains=
      #DNSSEC=no
      #DNSOverTLS=no
      #MulticastDNS=no
      #LLMNR=no
      #Cache=no-negative
      Cache=yes
      #CacheFromLocalhost=no
      #DNSStubListener=yes
      #DNSStubListenerExtra=
      DNSStubListenerExtra=<docker0-IP>
      #ReadEtcHosts=yes
      #ResolveUnicastSingleLabel=no
      #StaleRetentionSec=0
    ```
  - Restart the `systemd-resolved` service:
    ```sh
    sudo systemctl restart systemd-resolved.service
    ```



## Misc
Some additional noteworthy things:
  - [The Twelve-Factor Manifesto](https://github.com/twelve-factor/twelve-factor) - examples and guidelines for shedding outdated concepts and focusing on core application design for cloud deployment
  - [sysbox](https://github.com/nestybox/sysbox) empowers rootless containers to run workloads such as Systemd, Docker, Kubernetes, just like VMs.


### Docker placeholder templates
In docker there are some so callled `placeholder templates` available.  
Checkout [docker-compose.placeholder-templates.yml](compose-files/docker-compose.placeholder-templates.yml) for eaxamples.


### Get DockerHub pull rate limit
This is how to retreive the poull rate limit for DockerHub (requires curl, grep and optionally jq):

Get a token without authentication:
```sh
# Without jq:
TOKEN=$(curl -s "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```
```sh
# With jq:
TOKEN=$(curl -s "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | jq -r .token)
```
```sh
# With wget:
TOKEN=$(wget -qO- "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```

Get token with authentication:
```sh
# Without jq:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```
```sh
# With jq:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | jq -r .token)
```
```sh
# With wget:
TOKEN=$(curl -s --user 'username:password' "https://auth.docker.io/token?service=registry.docker.io&scope=repository:ratelimitpreview/test:pull" | awk -F'"' '/token/ {print $4}')
```

Get rate limit:
```sh
curl -s --head -H "Authorization: Bearer $TOKEN" https://registry-1.docker.io/v2/ratelimitpreview/test/manifests/latest
```
```sh
# With wget:
wget --server-response --spider --header="Authorization: Bearer $TOKEN" https://registry-1.docker.io/v2/ratelimitpreview/test/manifests/latest
```


### Running a container with runc
Since `runc` is used to actually run container, it is possible to use it directly without any higher-level container runtime:
  - switch into the `runc_container` directory
  - Make `hello.c` a static executable binary (requires the `gcc` and `musl-dev` packages): `gcc -o ./rootfs/helloworld -static-libgcc -static hello.c`
  - Create specification file for a rootless container (or use existing):
    - `runc spec --rootless`
    - Set `.process.terminal: false` and `.process.args["./helloworld"]`
  - Create a container: `runc create helloworld-container`
  - Run the container (afterwards press Enter): `runc start helloworld-container`
  - Remove the container: `runc delete helloworld-container`

While this container only contains a single binary, it is absolutely possible to add a full Linux distribution by adding its root filesystem into the `rootfs` directory.  
For example the [Alpine Linux mini root filesystem](https://alpinelinux.org/downloads/).

### Change Docker/Pdoman container runtime
It is possible to change the container runtime used in `Docker` and `Podman`.

Here is an example to use [crun](https://github.com/containers/crun/):
  - A simple way of installing `crun` is by using just the binary from the [release page](https://github.com/containers/crun/releases)
  - Add it to `/usr/bin/crun` or `/usr/local/bin/crun`
  - Use it in `Docker`:
    - Adjust `/etc/docker/daemon.json`:
      ```json
      {
        "default-runtime": "runc",
        "runtimes": {
          "crun": {
            "path": "/usr/bin/crun"
          }
        }
      }
      ```
    - Check:
      ```sh
      docker info | grep -A3 Runtimes
      ```
    - Use it:
      ```sh
      docker run --runtime=crun --rm alpine echo it works
      ```
  - Use it in `Podman`:
    ```sh
    podman run --runtime=crun --rm --memory 512k alpine echo it works
    podman run --runtime=runc --rm --memory 3M alpine echo it works
    ```
    ```sh
    podman run --runtime /usr/bin/crun --rm --memory 512k alpine echo it works
    podman run --runtime /usr/bin/runc --rm --memory 3M alpine echo it works
    ```

details can be found here:
  - [Docker Engine alternative container runtimes](https://docs.docker.com/engine/daemon/alternative-runtimes/)
  - [Docker Compose runtime specification](https://docs.docker.com/reference/compose-file/services/#runtime)
