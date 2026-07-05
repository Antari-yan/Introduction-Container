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
