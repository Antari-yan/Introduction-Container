# Recommendations

## Logging
Applications should log as much as possible but only as much as needed.  
Well structured, detailed and specific logs are usually more helpful than too many logs.  
Additionally logs should be properly separated by log level:
  - `DEBUG`: System flow information
    - Entering/exiting functions.
    - Important decision branches ("User has admin role, enabling feature X").
    - Non-critical events that help reproduce issues.
  - `INFO`: Application events that are part of the "normal" lifecycle
    - Application start/stop.
    - Successful connection to a DB.
    - Handling a user request.
    - Periodic health/status messages.
  - `WARN` (or `WARNING`): Something unexpected happened, but the application can continue running
    - Deprecated API or function used.
    - Retry needed due to transient failure.
    - Config value missing, using a default.
  - `ERROR`: An error occurred that prevented part of the application from working properly.
    - Database write failure.
    - Payment API call failed.
    - Unhandled exception in a request handler.
  - `FATAL` (or `CRITICAL`): Severe error that makes the application unusable and requires immediate attention
    - Application can't start (e.g., config missing, DB unreachable).
    - Data corruption detected.
    - Security breach detected.

In a production environment the `DEBUG` log level should not be used/needed.  
The other log levels should only container necessary information and not create an information overload,
so that they are easy check, filter and work with.  

Using centralized logging and monitoring solutions like
[Grafana](https://github.com/grafana/grafana) + [Prometheus](https://github.com/prometheus/prometheus) + [Loki](https://github.com/grafana/loki) is always helpful,
especially since container logs are not retained after a restart.


## Healthchecks
While the `HEALTHCHECK` option in the Dockerfile/Containerfile is `docker` specific it is good to
add some form of capability that allows running a healthcheck in the deployment configuration.  
For APIs this is quite simple when tools like `curl` or `wget` are available in the container.  
For other types of applications it has to be implemented differently by adding options to e.g. trigger a function that checks if the application is still responding
or something to check if the process is working.

## Reduce image size
Reducing the image size helps save space and cost on the host system.  
It can be done easily by doing small adjustments when building the container image.  
Additionally there are also tools like [slim](https://github.com/slimtoolkit/slim), which can be useful for reducing image size.


### Use small base images
While it might seem intuitive for some to use images like `debian`, there are often smaller base images available, like:
  - Instead of `debian:13` use `debian:13-slim`
  - Instead of `python:3.13` (based on debian) use `python:3.13-slim` or switch to an `alpine` base image: `python:3.13-alpine`

`alpine` is one of the smallest Linux distributions and great to use in a container environment.  
But be careful that things like the `c-compiler` can differ compared to other distributions.

There are also the [distroless](https://github.com/GoogleContainerTools/distroless) images which can be used for a wide variety.

But nothing beats using an empty `scratch` image that only contains a static binary, though that is not always easily achievable.  
For applications with outgoing `HTTPS` connections add the `ca-certificates`, like in the [Dockerfile.scratch-with-certificates](../../dockerfiles/Dockerfile.scratch-with-certificates) example.

Checkout the examples in the [Use multi-stage builds](#use-multi-stage-builds) section.


### Container image layer optimization
A simple way of reducing the container image size is by optimizing the individual layers.  

When installing a package via a package registry, the package list should always be updated, but this also leads to the list still being contained in the image.

Don't do it like this:
```Dockerfile
RUN apt update
RUN apt upgrade -y
RUN apt install htop
RUN apt install python3
RUN apt upgrade -y
RUN apt autoclean -y
RUN apt autoremove -y
RUN apt clean -y
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
  - [Dockerfile.clean-apk](../../dockerfiles/Dockerfile.clean-apk)
  - [Dockerfile.clean-apt](../../dockerfiles/Dockerfile.clean-apt)
  - [Dockerfile.clean-python](../../dockerfiles/Dockerfile.clean-python)


### Use multi-stage builds
With multi-stage builds it is easily possible to reduce the image size, because only the final stage is included in the image.  
This is quite useful if, e.g.: an application that is build in one stage doesn't need all the dependencies to run compared to when it is build.

Checkout these example:
  - [Dockerfile.multi-stage.yml](../../dockerfiles/Dockerfile.multi-stage)
  - [Dockerfile.non-root](../../dockerfiles/Dockerfile.non-root)
  - [Dockerfile.non-root-scratch](../../dockerfiles/Dockerfile.non-root-scratch)

> [!NOTE]  
> For applications with outgoing connections like `HTTPS`,
> It is usually required to have the `public certificates` of the target URL:
>   - For normal valid certificates it usually suffices to install the `ca-certificates` package
>   - For `self-signed` certificates they need to be added directly
> Checkout the [Dockerfile.scratch-with-certificates](../../dockerfiles/Dockerfile.scratch-with-certificates) example.

> [!NOTE]  
> Each stage results in a stored untagged image.  
> They can be cleaned up with: `$CR image prune --filter label=stage=builder`


## Use non-root
Using a non-root user in a container improves security because:
  - In some cases, having root as user can be escalated to the root on the host (e.g., through kernel or runtime escapes)
  - A non-root process inside a container limits what can be read, written, or executed
  - It can prevent accidental (or malicious) interference with other workloads (e.g. when using mounts)
  - It follows the `Least Privilege Principle`:
    - Most applications don't need root access to run
    - Running as a less-privileged user means even if compromised, the attacker has fewer permissions

Checkout these example:
  - [Dockerfile.non-root](../../dockerfiles/Dockerfile.non-root)
  - [Dockerfile.non-root-scratch](../../dockerfiles/Dockerfile.non-root-scratch)

## Version pinning
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


## Application profiling
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
For that checkout the [docker_process_count.bash](../../scripts/docker_process_count.bash) script which can output the total amount of threads and processes for each running container.

## Security
There are tools like [trivy](https://github.com/aquasecurity/trivy) and [osv-scanner] (https://github.com/google/osv-scanner) that should be used to check container images for vulnerabilities.

> [!IMPORTANT]  
> The vulnerability checks are always a snapshot at a specific point in time.  
> Just because there was no vulnerability found during the initial check,
> That doesn't mean that there will not be any vulnerability found in the future.  
> Vulnerability checks should be done on a regular and continuous basis.

## Use annotations during build
The [container image spec](https://github.com/opencontainers/image-spec) also specifies some `annotations` that are recommended to be added during the build process.  
These include information like who build the image, where can the source code be found, etc.,
an overview can be found [here](https://github.com/opencontainers/image-spec/blob/main/annotations.md).

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

## Sign the container images
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

## Automate the build process
Following all best practices can make the developement process tedious, which is why automating the process is highly recommended:
  - For `GitHub` there are a varity of [GitHub Actions](https://docs.github.com/en/actions) available.
  - For `GitLab` there are also a varity oc [CI Templates](https://docs.gitlab.com/ci/examples/) available.

> [!NOTE]  
> Checkout this GitLab CI Component:  
> [Container CI Component](https://jugit.fz-juelich.de/iek-10/public/developer-tools/gitlab-ci-components/container)
