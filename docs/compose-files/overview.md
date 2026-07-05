# Compose files
While using `docker run` is fine for testing and development,
when it comes to managing multiple containers it gets out of hand quickly and becomes unmanageable.  
There are many pieces that can go wrong,
a missing container or network,
a typo when entering the command,
incomplete copying from a documented command and so forth.

This is where `Compose files` come in.  
A `Compose file` is a `YAML` configuration file that describes how to run a set of containers as services.  
The default filename is `docker-compose.yml`, but can differ if needed.

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
  webserver:  # This is the name of the service, which can be set to anything
    image: docker.io/nginx:1-alpine # The image used for the container of the service "webserver"
    ports:  # A list of ports to be exposed
      - target: 8080  # Port on the host system
        published: 80 # Port in the container
        protocol: tcp # The used protocol
        mode: ingress
        # The mode is important for a Docker Swarm setup.
        # "ingress" mode (default) means the port is exposed on one host and Docker Swarm uses loadbalancing
        # "host" mode publishes the port on all nodes
    volumes:  # A list of volume mounts similar to "docker run"
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

Further options can be found in the [Docker Compose Reference](https://docs.docker.com/reference/compose-file).

> [!NOTE]
> Docker Compose files can also be packaged and distributed as `OCI artifacts`
> (stored in a container registry alongside images).
> - [Package and deploy Docker Compose applications as OCI artifacts](https://docs.docker.com/compose/how-tos/oci-artifact)
> - [Implementation for GitLab Container Registry](https://gitlab.com/groups/gitlab-org/-/epics/7056)


Checkout the following examples:
  - [docker-compose.registry.yml](../../compose-files/docker-compose.registry.yml)
  - [docker-compose.sidecar.yml](../../compose-files/docker-compose.sidecar.yml)
  - [docker-compose.webapp-and-storage.yml](../../compose-files/docker-compose.webapp-and-storage.yml)

Starting the services in the compose file can be done with `$CR compose up -d` or:  
```sh
$CR compose -f compose-files/docker-compose.registry.yml up -d
```

Stop them with:
```sh
$CR compose -f compose-files/docker-compose.registry.yml up -d
```

> [!NOTE]  
> Variables defined in the environment section can be used in the `command` and `entypoint` sections.  
> For that use two `$` instead of a single one, an example can be found in `compose-files/docker-compose.placeholder-templates.yml`.
