# Introduction to Container
The Container technology revolutionized in how to develop, run and manage software, from small applications to huge interconnected clustered applications (e.g. Web-App, Database, ...).  
All of that is made possible through flexible configuration, Container Images and a short starting time.

This repository is intended to provide a basic introduction to containers, what they are, how to run and manage them and
the concept of packaging software and its dependencies into a single portable unit.

It also covers tools like [Podman](https://podman.io/) and [Docker](https://www.docker.com/),
as well as [Kubernetes](https://kubernetes.io/)(K8s).

To start, open the [github-pages](https://antari-yan.github.io/Introduction-Container), pull the repository and follow each step:
```sh
git clone https://github.com/Antari-yan/Introduction-Container.git
cd Introduction-Container
```

> [!NOTE]  
> Examples are provided for both `Podman` and `Docker`.  
> Where the commands overlap, the `CR` variable is used.  
> Set it to whichever is installed:  
> `CR=$(command -v podman || command -v docker)`


## Table of Contents
- Home
  - [Overview](docs/index.md)
  - [Motivation & Background](docs/motivation-and-background.md)
  - [What is a Container?](docs/what-is-a-container.md)
  - [container-runtime](docs/container-runtime.md)
- Fundamentals
  - [Setup](docs/fundamentals/setup.md)
  - [Basic commands](docs/fundamentals/basic-commands.md)
  - [Advanced Commands](docs/fundamentals/advanced-commands.md)
  - [Running and interacting with container](docs/fundamentals/running-and-interacting-with-container.md)
  - [Data persistence](docs/fundamentals/data-persistence.md)
  - [Networking](docs/fundamentals/networking.md)
  - [Sharing namespaces between containers](docs/fundamentals/sharing-namespaces-between-containers.md)
- Container Images
  - [Overview](docs/container-images/overview.md)
  - [Container Image Specification](docs/container-images/container-image-specification.md)
  - [Pulling a Container Image](docs/container-images/pulling-a-container-image.md)
  - [Inspecting a Container Image](docs/container-images/inspecting-a-container-image.md)
  - [Managing local Container Images](docs/container-images/managing-local-container-images.md)
- Container Registries
  - [Overview](docs/container-registries/overview.md)
  - [Example for a simple local Container Registry](docs/container-registries/example-for-a-simple-local-container-registry.md)
- Container Image building
  - [Basics](docs/container-image-building/basics.md)
  - [Muli-arch Container Images](docs/container-image-building/muli-arch-container-images.md)
  - [Recommendations](docs/container-image-building/recommendations.md)
- Compose files
  - [Overview](docs/compose-files/overview.md)
  - [Networking](docs/compose-files/networking.md)
  - [Recommendations](docs/compose-files/recommendations.md)
- Docker Swarm
  - [Overview](docs/docker-swarm/overview.md)
  - [Logging](docs/docker-swarm/logging.md)
  - [Docker Swarm data persistence](docs/docker-swarm/docker-swarm-data-persistence.md)
  - [Docker Swarm Networking](docs/docker-swarm/docker-swarm-networking.md)
- Kubernetes
  - [Overview](docs/kubernetes/overview.md)
  - [Architecture](docs/kubernetes/architecture.md)
  - [Kubernetes Objects](docs/kubernetes/kubernetes-objects.md)
  - [Setup k3d](docs/kubernetes/setup-k3d.md)
  - [kubectl](docs/kubernetes/kubectl.md)
  - [Deploying applications](docs/kubernetes/deploying-applications.md)
  - [Health probes](docs/kubernetes/health-probes.md)
  - [Resource requests and limits](docs/kubernetes/resource-requests-and-limits.md)
  - [Services and Networking](docs/kubernetes/services-and-networking.md)
  - [ConfigMaps and Secrets](docs/kubernetes/configmaps-and-secrets.md)
  - [Storage](docs/kubernetes/storage.md)
  - [Self-healing and Autoscaling](docs/kubernetes/self-healing-and-autoscaling.md)
  - [Rolling updates and rollbacks](docs/kubernetes/rolling-updates-and-rollbacks.md)
  - [Namespaces](docs/kubernetes/namespaces.md)
  - [RBAC and ServiceAccounts](docs/kubernetes/rbac-and-serviceaccounts.md)
  - [Other workload types](docs/kubernetes/other-workload-types.md)
  - [Helm](docs/kubernetes/helm.md)
  - [Operators and Custom Resources](docs/kubernetes/operators-and-custom-resources.md)
  - [Managing many clusters and GitOps](docs/kubernetes/managing-many-clusters-and-gitops.md)
  - [Cleanup](docs/kubernetes/cleanup.md)
  - [Additional Tools and Resources](docs/kubernetes/additional-tools-and-resources.md)
- Best practices
  - [Overview](docs/best-practices/overview.md)
  - [Validating the Docker daemon configuration](docs/best-practices/validating-the-docker-daemon-configuration.md)
  - [File descriptor limits](docs/best-practices/file-descriptor-limits.md)
  - [inotify resource limits](docs/best-practices/inotify-resource-limits.md)
  - [Cache DNS responses for outgoing connections](docs/best-practices/cache-dns-responses-for-outgoing-connections.md)
- Misc
  - [Overview](docs/misc/overview.md)
  - [Docker placeholder templates](docs/misc/docker-placeholder-templates.md)
  - [Get DockerHub pull rate limit](docs/misc/get-dockerhub-pull-rate-limit.md)
  - [Running a container with runc](docs/misc/running-a-container-with-runc.md)
  - [Change Docker/Podman container runtime](docs/misc/change-docker-and-podman-container-runtime.md)
  - [Parallel computing (MPI / OpenMP)](docs/misc/parallel-computing.md)
  - [Running kubernetes manifests directly with Podman](docs/misc/running-kubernetes-manifests-directly-with-podman.md)


## Rendering the docs locally
The pages under `docs/` are GitHub-flavoured Markdown, which is converted in the CI to be compatible with MkDocs.  
To build the site locally, convert the docs and serve them:
```sh
pip install "mkdocs-material~=9.7"
python3 .github/render_docs.py --docs docs --root . --repo-url https://github.com/Antari-yan/Introduction-Container
mkdocs serve
```

`render_docs.py` rewrites `docs/` in place, so restore it afterwards:
```sh
git restore docs
```
