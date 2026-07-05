# Basics
While there are many prebuilt images, most real-world projects need custom images.  
The recipe for building an image is most commonly called a `Dockerfile`, which is also the default filename.  
`Containerfile` is part of the [container-libs](https://github.com/containers/container-libs/tree/main) and uses the same syntax,
it is what `podman` and it's buildkit `buildah` defaults to.  
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

# The executable and paramters run at the start of the container, can be overwritten with "--entrypoint ''".
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
$CR build -t myimage:1 -f Dockerfile  -o "type=local,dest=myimage-1" .
```

Checkout the [hello-world](../../dockerfiles/hello-world) for some different coding language examples in the [dockerfiles](../../dockerfiles) directory.

> [!IMPORTANT]  
> For running some kind of API as application use `0.0.0.0:<port>`.  
> This makes the API properly available in the container and accessible when connected to the host or to other container.

> [!IMPORTANT]  
> When building images containing code, be careful what base image is used.  
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

Checkout the [Dockerfile.multi-stage](../../dockerfiles/Dockerfile.multi-stage) as an example.
