# Container Registries
A `Container Registry` is a centralized service for storing and distributing container images, which can be compared to App Stores or Package Registries (used by `apt`, etc.).

Registries are Configured in the following files:
  - Podman:
    - `etc/containers/registries.conf`
    - `/etc/containers/registries.conf.d/...`
  - Docker:
    - `/etc/docker/daemon.json`

The default `Container Registry` for Docker is [docker.io](https://hub.docker.com/) Podman has a set of predefined aliases,
which can be found in `/etc/containers/registries.conf.d/shortnames.conf`.  
The most common Container Registries are:
  - [docker.io](https://hub.docker.com) ([usage and limits](https://docs.docker.com/docker-hub/usage))
  - [quay.io](https://quay.io) ([rate limited](https://access.redhat.com/solutions/6218921))
  - `ghcr.io`  which is the Container Registry for GitHub Repositories,
    details can be found [here](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry)
  - GitLab Repositories can have a Container Registry similar to GitHub,
    details can be found [here](https://docs.gitlab.com/user/packages/container_registry)
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
> To check the current limit for `DockerHub` checkout the [Get DockerHub pull rate limit](../misc/get-dockerhub-pull-rate-limit.md) section.

Shoutout to the [linuxserver](https://github.com/linuxserver) group for maintaining and providing many day-to-day applications as container images,
via their own [lscr.io](https://docs.linuxserver.io/images-by-category/) registry.
