# Introduction to Container

:warning: WiP
:-


## Information about Docker
```bash
docker version
docker info
docker help
```

## Basic Commands
```bash
# Run IP command in a Container
docker container run alpine ip a s

# List all container
docker container ls -a

# Get ID of last created Container
docker container ls -lq

# Remove that Container
docker container rm $(docker container ls -lq)

# Run a Container but remove it upon exit
docker container run --rm alpine cat /etc/os-release

# -d daemon
# --rm delete after process terminated
# --tty -t create tty
# -i interactive
docker container run -d --rm -ti alpine sleep 3600

# Remove al unused containers
docker container prune

# Start a shell interactive inside a container distro
docker container run -it alpine /bin/sh
# ip a s
# exit

# Restart a container
docker container restart $(docker container ls -lq)

# Attach to terminal
docker container attach $(docker container ls -lq)
# curl google.de
# detach terminal  
# exit

# List all process of the last container from outside
docker container top $(docker container ls -lq)
# List all process of the last container from inside
docker container exec $(docker container ls -lq) ps -ef
```

## Advanced Commands
```bash
# Show Only ID and names of running Containers
docker container ls --format "{{.ID}}: {{.Names}}"
docker container ls --format "table {{.ID}}\t{{.Names}}"
```
```bash
# Get ID of last created Container
docker container ls -lq
```
```bash
# Stop last created Container
docker container stop $(docker container ls -lq)
```
```bash
# Get Filesystem diff of a Container
docker diff $(docker container ls -lq)
```
```bash
# Get IP of last created container
docker container inspect --format \
  '{{.NetworkSettings.Networks.bridge.IPAddress}}' \
  $(docker container ls -lq)
```
```bash
# Get Env Vars of last created Container
docker container inspect --format \
  '{{range .Config.Env}}{{with split . "="}}{{printf "%s: %s\n" ( index . 0 ) ( index . 1 )}}{{end}}{{end}}' \
  $(docker container ls -lq)
# range: Iterate over an array, setting the . value to each array element.
# with split: The with command runs the nested statement with a new value for .. And the split command parses each line, separating it into an array using the = as a delimiter.
# printf: This formats some output, useful when you want to truncate a string, or in this case include linefeeds.
# index: This selects an element from an array. We’re in luck that Go starts their arrays at 0 like a proper language.
# end: These close the range and with loops.
# $( ): This is bash shell notation to run a command and include the resulting output in current command.

# https://docs.docker.com/config/formatting/
```


## Attach to another container
```bash
# Start nginx container
docker container run --rm -d -p 127.0.0.1:8080:80 --name nginx nginx
curl http://127.0.0.1:8080

# Attach to an nginx
docker container run --rm --network container:nginx --pid container:nginx -ti alpine /bin/sh
```
```bash
# Show some stuff
ip a s
ps -ef
 
# Edit files from nginx container
cat  /proc/1/root/etc/nginx/nginx.conf
echo "" | tee /proc/1/root/etc/nginx/nginx.conf > /dev/null
cat  /proc/1/root/etc/nginx/nginx.conf
curl http://127.0.0.1:8080
exit
```


## Building Container

### Adding Labels
[Label specification](https://github.com/opencontainers/image-spec/blob/main/annotations.md)

```Dockerfile
FROM alpine
 
LABEL org.opencontainers.image.ref.name="apache-container"  \
      org.opencontainers.image.vendor="FZJ" \
      org.opencontainers.image.description="Apache example" \
      org.opencontainers.image.licenses="Apache-2.0"
 
RUN apk add --update --no-cache apache2  
CMD [ "httpd", "-D", "FOREGROUND", "-k", "start"]
EOF
```
```bash
# Show labels of container
docker image ls --filter 'label=org.opencontainers.image.licenses'
```



## Further recommends
- Use Ports > 1024 for your applications to increase security
  - Ports <= 1024 require root access even inside a container, due to the nature of the unix systems
- Create a non root user inside of the container
  - e.g.:
    ```
    ARG USERNAME=user-name-goes-here
    ARG USER_UID=1000
    ARG USER_GID=$USER_UID

    # Create the user
    RUN groupadd --gid $USER_GID $USERNAME \
        && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
        #
        # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
        && apt-get update \
        && apt-get install -y sudo \
        && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
        && chmod 0440 /etc/sudoers.d/$USERNAME

    # ********************************************************
    # * Anything else you want to do like clean up goes here *
    # ********************************************************

    # [Optional] Set the default user. Omit if you want to keep the default as root.
    USER $USERNAME
    ```
    - Alpine requires the `shadow` package for this:
      ```
      RUN apk add --no-cache shadow
      ```
- Use a `.dockerignore` file to prevent copying unneeded files