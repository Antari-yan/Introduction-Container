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
- It’s popular in enterprise and security-sensitive contexts and integrates tightly with `systemd` (the Linux init system).
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

Apple has [container](https://github.com/apple/container), which is basically their own version of a container runtime for Mac.

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
- Provides a CLI (docker) and relies on a daemon process (dockerd) that runs in the background to manage containers.
- Widely adopted in industry; most tutorials and documentation still use Docker commands.
- Strong ecosystem: Docker Hub (public image registry), Docker Compose (multi-container applications).
- The Docker Engine, is a client-server application that includes:
  - Docker Daemon (dockerd): Runs in the background and manages Docker objects like containers, images, networks, and volumes. It listens for Docker API requests and is responsible for building, running, and distributing Docker containers.
  - Docker Client (docker): A command-line interface (CLI) tool that allows users to interact with the Docker Daemon. When you type commands like docker run or docker build, the client sends these commands to the Docker Daemon, which then carries them out.
  - Containerd: A core container runtime component used by Docker to manage container lifecycle operations. It handles downloading and storing container images, container execution, and storage management.
  - runc: A lightweight, low-level container runtime that creates and runs containers based on the OCI (Open Container Initiative) standards. runc is the tool that Docker uses under the hood to start containers.
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

To simplify set an env var with either `podman` or `docker` (whatever is installed)
```sh
# Use "podman" if available or use "docker"
CR=$(command -v podman || command -v docker)
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
```bash
$CR container inspect --format \
  '{{.NetworkSettings.Networks.bridge.IPAddress}}' \
  $($CR container ls --filter name=webserver --quiet)
```

Get Env Vars of a Container:
```bash
$CR container inspect $($CR container ls --filter name=webserver --quiet) --format='{{range .Config.Env}}{{println .}}{{end}}'
```
- `range`: Iterate over an array -> Env array under config
- `println .`: Print each item in the range
- `end`: Closes range

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
$CR container run -d --rm --name web-server --publish 8080:80 docker.io/nginx:1-alpine
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
$CR container inspect web-server
```

Filter out specific information:
```sh
$CR container inspect --format='{{.Id}}' web-server
```
> [!NOTE]  
> Podman and Docker can have different information stored for container.  
> e.g.: Podman has the `ImageName` key, but docker doesn't.

Execute a command in a running container:
```sh
$CR container exec $($CR container ls --filter name=web-server --quiet) cat /etc/os-release
```

Open an interactive shell in a running container:
```sh
# 
$CR container exec -it $($CR container ls --filter name=web-server --quiet) /bin/sh
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
$CR container stop web-server
```

#### Using the API
Just to show that all the commands are just API interactions, the following example start/stops/removes a container in Docker using `curl`.
[Docker Engine API reference](https://docs.docker.com/reference/api/engine/) (Example is using v1.51).

Create Container:
```sh
curl -XPOST --unix-socket /var/run/docker.sock -d '{"Image":"nginx:1-alpine"}' \
  -H 'Content-Type: application/json' \
  http://localhost/containers/create?name=web-server
```

Start Container:
```sh
curl -XPOST --unix-socket /var/run/docker.sock \
  -H 'Content-Type: application/json' \
  http://localhost/containers/$(docker container ls --all --filter name=web-server --quiet)/start
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
  http://localhost/containers/$(docker container ls --all --filter name=web-server --quiet)/stop

docker ps --all
```

Delete Container:
```sh
curl -XDELETE --unix-socket /var/run/docker.sock \
  -H 'Content-Type: application/json' \
  http://localhost/containers/$(docker container ls --all --filter name=web-server --quiet)

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

It is also possible to use different storage drivers like `nfs`.  
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
Stopping (`$CR stop web-server `) and restarting the container should always show the Updated start page.

#### Bind mount example
```sh
$CR container run --rm -d \
  --name web-server \
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
  --name web-server \
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
  --name web-server \
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
podman container run --rm -d --name web-server --network podman -p 8080:80 -p 8443:443 docker.io/nginx:1-alpine
```
```sh
docker container run --rm -d --name web-server -p 8080:80 -p 8443:443 docker.io/nginx:1-alpine
```

Retrieve the private IP address of the webserver:
```sh
WEB_SERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=web-server --quiet))
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
$CR container run --rm -d --name web-server --network mynet docker.io/nginx:1-alpine
WEB_SERVER_IP=$($CR inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $($CR container ls --filter name=web-server --quiet))
```

```sh
podman run --rm --cap-add=NET_RAW busybox ping ${WEB_SERVER_IP}
# this should fail, abort with ctrl+c

podman run --rm --cap-add=NET_RAW --network mynet busybox ping ${WEB_SERVER_IP}
# this should succeed

# ping needs the ability to open raw sockets,
# which normally requires either root privileges or the Linux capability CAP_NET_RAW
```
```sh
docker run --rm busybox ping ${WEB_SERVER_IP}
# this should fail, abort with ctrl+c

docker run --rm --network mynet busybox ping ${WEB_SERVER_IP}
# this should succeed
```

```sh
$CR container stop web-server
$CR network rm mynet
```

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
$CR container run --rm -d --name web-server -v ./data:/usr/share/nginx/html:ro docker.io/nginx:1-alpine

# Start BusyBox, sharing nginx's network namespace
$CR container run --rm --network container:$($CR container ls --filter name=web-server --quiet) busybox wget -qO- http://127.0.0.1

# Start BusyBox, sharing nginx's PID namespace
$CR container run --rm --pid container:$($CR container ls --filter name=web-server --quiet) busybox ps aux
$CR container run  -it --rm --pid container:$($CR container ls --filter name=web-server --quiet) busybox cat /proc/1/environ

# Start BusyBox, sharing nginx's volumes
$CR container run --rm --volumes-from web-server busybox ls -al /usr/share/nginx

$CR container stop $($CR container ls --filter name=web-server --quiet)
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
Containers run from `images`.
An image is a `packaged filesystem` with all the files, libraries, and metadata needed to create a container.
It’s the blueprint from which containers are launched.

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
The recipe for building an image is most commonly called a `Dockerfile`.  
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

Using `latest` can always lead to breaking changes when rebuilding a container image


#### Application profiling
It is possible to limit the provided `CPU` and `RAM` when running container.  
This is especially important for applications that scale horizontaly.  
Only by doing so is it possible to properly calculate how big the host system or cluster is needed to be.

For that there are many application profiling tools like [scalene](https://pypi.org/project/scalene/) or [cProfile](https://docs.python.org/3/library/profile.html) for `python`.  
Additionaly with `$CR stats` it is possible to check the currently used `CPU` and `RAM` for each running container.

For `kubernetes` the `top` command can be used:
  - `kubectl top pod <POD_NAME> --containers`
  - `kubectl top node <NODE_NAME>`


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
