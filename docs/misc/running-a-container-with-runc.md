# Running a container with runc
Since `runc` is used to actually run container, it is possible to use it directly without any higher-level container runtime:
  - switch into the `runc_container` directory
  - Make `hello.c` a static executable binary (requires the `gcc` and `musl-dev` packages): `gcc -o ./rootfs/helloworld -static-libgcc -static hello.c`
  - Create specification file for a rootless container (or use existing):
    - `runc spec --rootless`
    - Set `.process.terminal: false` and `.process.args["./helloworld"]`
  - Create a container: `runc create helloworld-container`
  - Run the container (afterwards press Enter): `runc start helloworld-container`
  - Remove the container: `runc delete helloworld-container`

While this container only contains a single binary, it is absolutely possible to add a full Linux distribution by adding its root filesystem into the `rootfs` directory.  
For example the [Alpine Linux mini root filesystem](https://alpinelinux.org/downloads/).
