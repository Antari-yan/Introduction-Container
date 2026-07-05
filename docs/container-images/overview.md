# Container Images
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

> [!IMPORTANT]  
> Images can take up a major part of disc space, because there is no automatic cleanup.  
> `$CR system df` can be used to check the current used disk space.  
> `$CR image prune` can be used to cleanup unused images.  
> Or forceully wiht `$CR container image prune -a -f`
