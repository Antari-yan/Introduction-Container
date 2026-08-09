# Change Docker/Podman container runtime
It is possible to change the container runtime used in `Docker` and `Podman`.

Here is an example to use [crun](https://github.com/containers/crun/):
  - A simple way of installing `crun` is by using just the binary from the [release page](https://github.com/containers/crun/releases)
  - Add it to `/usr/bin/crun` or `/usr/local/bin/crun`
  - Use it in `Docker`:
    - Adjust `/etc/docker/daemon.json`:
      ```json
      {
        "default-runtime": "runc",
        "runtimes": {
          "crun": {
            "path": "/usr/bin/crun"
          }
        }
      }
      ```
    - Check:
      ```sh
      docker info | grep -A3 Runtimes
      ```
    - Use it:
      ```sh
      docker run --runtime=crun --rm alpine echo it works
      ```
  - Use it in `Podman`:
    ```sh
    podman run --runtime=crun --rm --memory 512k alpine echo it works
    podman run --runtime=runc --rm --memory 3M alpine echo it works
    ```
    ```sh
    podman run --runtime /usr/bin/crun --rm --memory 512k alpine echo it works
    podman run --runtime /usr/bin/runc --rm --memory 3M alpine echo it works
    ```

details can be found here:
  - [Docker Engine alternative container runtimes](https://docs.docker.com/engine/daemon/alternative-runtimes/)
  - [Docker Compose runtime specification](https://docs.docker.com/reference/compose-file/services/#runtime)
