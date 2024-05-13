# Docker Introduction
This repository is a brief introduction in how to setup and use `Docker`.

## What is Docker
Essentially `Docker` is a platform that manages multiple `container` and separates them from the `host` system it is running.
A `container` contains all dependencies and code required for running a single application e.g. Webserver, Database, a Python script, Docker... .  
By default each individual `container` is also separated from each other.
Additionally `Docker` can create connections between `container` and/or the `host` through connecting `networks` and exposing `Ports`.

One of the main advantages is the ability to run multiple (different or the same) applications, of different or the same version, on one `host`.

### Docker architecture
Docker uses a client-server architecture.  
The `Docker client` talks to the `Docker daemon`, which does the heavy lifting of building, running, and distributing `Docker containers`.  
Both can run on the same `host`, or you can connect a `Docker client` to a remote `Docker daemon`.
Communication between these two is done via a `REST API`, over `UNIX sockets` or a `network interface`.
E.g. another `Docker client` is `Docker Compose`, that lets you work with applications consisting of a set of containers.

<img src=./images/docker-architecture.png height=400px />

#### Docker daemon
The `Docker daemon` (dockerd) listens for `Docker API` requests and manages `Docker objects` such as `images`, `containers`, `networks`, and `volumes`.  
A `daemon` can also communicate with other `daemons` to manage `Docker services`.

#### Docker client
The `Docker client` (docker) is the entrypoint to interact with `Docker`.  
It sends commands invoked through e.g. `docker run` over the `Docker API` to `dockerd`, which carries them out.
A `Docker client` can communicate with more than one `daemon`.

#### Docker registries
A `Docker registry` stores `Docker images`.
The default `registry` of `Docker` is [Docker Hub](https://hub.docker.com/), but it is also possible to use a different or your own `registry`.  
It is always recommended to use `images` provided by a Official/Verified Publisher instead of private or community driven images like `Docker Official Image` on `Docker Hub`.
When ever the commands `docker pull` or `docker run` are used, `Docker` pulls the required `images` from the configured `registry`.  
With `docker push` you can upload your `images` to the configured `registry`.

#### Container images
A `container image` is a read-only template with instructions for creating a `container`.  
Often, an `image` is based on another `image`, with additional customization.  
E.g. an `image` is based on the `Ubuntu image` and additionally installs a `web server`.  

A `Dockerfile` is used for defining the steps needed to create the desired `image`.
Each `instruction` in a `Dockerfile` creates a `layer` in the `image`.  
When you change the `Dockerfile` and rebuild the `image`, only those `layers` which have `changed` are `rebuilt`.

#### Containers
A `container` is a runnable instance of an `image`.  
They can be `created`, `started`, `stopped`, `moved`, or `deleted` using the `Docker API`.

By default, a `container` is relatively well `isolated` from other `containers` and its `host`.  
It is possible to `control` how `isolated` a container's `network`, `storage`, etc. are from other `containers` or from the `host`.

A `container` is defined by its `image` as well as any `configuration` provided to it when `created` or `started`.  
When a `container` is `removed`, its `state` and `data` `disappears`. Except if a `persistent storage` is `provided` to the `container`.


## Setup
### Linux
#### Debian/Ubuntu
To install `Docker` on Debian based Linux Distribution download & extract this repository, open a terminal, switch to this directory and run:
``` sh
# Install Docker
bash setup.sh

# Add your User to Docker group to run docker commands without sudo
sudo usermod -aG docker <your_username>
```

### Windows
It is recommended to either use `WSL` (Windows Subsystem for Linux) or a VM with a Debian based Distribution.  
An introduction and how to setup `WSL` can be found [here](https://jugit.fz-juelich.de/iek-10/core-projects/dev-samples/wsl/introduction)  
Otherwise [Docker Desktop](https://www.docker.com/products/docker-desktop/) can be used.


## Getting Started tutorial
The best way to get a good introduction into `Docker` is through the official `getting-started` tutorial.

```
docker run -dp 80:80 docker/getting-started
```
Open a browser and access http://localhost when you are using Windows, WSL or a Linux with an UI.
Or when you are using a remote VM use the VM specific IP Address.


## Command samples
### Basic commands
``` sh
# Show all available command options
docker -h
```
``` sh
# List current running container
docker ps
```
``` sh
# Show logs of a specific container
docker logs <container_name>
```
``` sh
# Execute a command inside of a container
docker container exec -it <container_name> <command>
```
``` sh
# Start an nginx proxy container
docker run -d nginx
```

### Interacting with container
``` sh
# Start a python container and open its CLI (Command Line Interface)
docker run -it --rm --name python-script python:3
# `exit()` will close the container
```
``` sh
# Run a python script placed in the current directory through a python container
docker run -it --rm --name python-script -v "$PWD":/usr/src/myapp -w /usr/src/myapp python:3 python your-daemon-or-script.py
```

### Creating a new container
``` sh
# Build a container based on the Debian image and install python into it
docker build -t custom-debian-python -f dockerfiles/custom-debian-python.dockerfile .
```
``` sh
# Start the built container, mount the current directory and open its shell
# First startup may take a few minutes
docker run -it --rm --name debian-python -h debian -v ${PWD}:/usr/src/app custom-debian-python
# `exit` will close the container
```
