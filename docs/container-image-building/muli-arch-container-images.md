# Muli-arch Container Images
Modern images are often built for multiple architectures (e.g., amd64, arm64).  
When building for multiple architectures, it creates a manifest list that points to multiple images, one per architecture.  
Registries and runtimes then automatically pull the right one for the platform.
This is critical in environments that consist of mixed architectures (e.g., x86 servers, ARM-based laptops, Raspberry Pi clusters).  
Sadly the buildkits are limited to either directly push the build image into a container registry or output it into a directory.

To run different architectures locally the QEMU emulation support (binfmt_misc) needs to be enabled in the kernel.  
This can be done by running the `qemu-user-static` container:
```sh 
sudo $CR run --rm --privileged docker.io/multiarch/qemu-user-static --reset -p yes 
```
This works because the container shares the `kernel` with the host system and therefore if it changes things on the kernel,
like enabling the QEMU emulation support, the host system is also affected by it.  
Which is why especially for `podman` the container has to be run as root to have the required permissions.

Test it:
```sh
$CR run --rm --platform=linux/arm64 alpine:3 uname -m
# aarch64

$CR run --rm --platform=linux/amd64 alpine:3 uname -m
# x86_64
```

> [!NOTE]  
> It is also possible to build images for different architectures on separate hosts
> and combine them by creating a manifest.

> [!NOTE]  
> When building for multiple architectures the image needs to be pushed to a container registry before it can be used.


## Podman

Enable the QEMU emulation support (binfmt_misc):
```sh 
sudo $CR run --rm --privileged docker.io/multiarch/qemu-user-static --reset -p yes 
```

Create a manifest:
```sh
podman manifest create <registry>/<image-name>
```

```sh
$CR build \
  --platform linux/amd64,linux/arm64 \
  --manifest <registry>/<image-name> \
  .
```

Publish the manifest:
```sh
podman manifest push <registry>/<image-name>
```

Check for which architectures the image is available for:
```sh
$CR manifest inspect <image name>
```

## Docker
Dockers `buildx` is capable to build multi-arch images with a custom builder, that uses individual container to build each architecture. 
Therefore it doesn't need the QEMU emulation support (binfmt_misc) installed on the host.  

Create a builder:
```sh
docker buildx create --name builder
docker buildx inspect builder --bootstrap
docker buildx use builder
```

Build the images and push it to the registry:
```sh
docker buildx build \
  --push \
  --platform linux/amd64,linux/arm64 \
  --output=type=image,push=true,registry.insecure=true \
  -f Dockerfile \
  --tag <registry>/<image-name> .
```

Check for which architectures the image is available for:
```sh
docker manifest inspect <registry>/<image-name> -v
```

Build the image and save it to the same directory:
```sh
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  --output=type=local,dest=$PWD \
  -f Dockerfile \
  --tag <image-name> .
```

Stop the custom builder:
```bash
docker container stop buildx_buildkit_builder0
```
