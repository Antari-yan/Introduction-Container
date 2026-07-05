# Home
The Container technology revolutionized in how to develop, run and manage software, from small applications to
huge interconnected clustered applications (e.g. Web-App, Database, ...).  
All of that is made possible through flexible configuration, Container Images and a short starting time.

This repository is intended as a short introduction into what containers are, how to run and manage them and
the concept of packaging software and its dependencies into a single portable unit.  
It also covers tools like [Podman](https://podman.io/) and [Docker](https://www.docker.com/),
as well as touching on [Kubernetes](https://kubernetes.io/)(K8s).

Anywhere the commands for Podman and Docker overlap the variable `CR` will be used,
that should contain `podman` or `docker`, depending on what is installed:
```sh
CR=$(command -v podman || command -v docker)
```
