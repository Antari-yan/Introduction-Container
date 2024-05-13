### Multiarch builds
```bash
# Run multiarch/qemu-user-static which contains a register script to add multiarch binaries
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes 

# Create a builder
docker buildx create --name builder
docker buildx inspect builder --bootstrap
docker buildx use builder


# Start registry
docker run --rm -d -p 127.0.0.1:5000:5000 --restart=always --name registry registry:2
# Get IP of the registry
IP_REGISTRY=$(docker container inspect --format '{{.NetworkSettings.Networks.bridge.IPAddress}}' $(docker container ls -lq))

# Build the images and push it to the registry
docker buildx build \
  --push \
  --platform linux/amd64,linux/arm64 \
  --output=type=image,push=true,registry.insecure=true \
  --target=prod \
  -f Dockerfile \
  --tag $IP_REGISTRY:5000/hello:0.1 .


# Run the image locally on different architectures
docker run --rm --platform=linux/arm64 127.0.0.1:5000/hello:0.1
docker run --rm --platform=linux/amd64 127.0.0.1:5000/hello:0.1
```

```bash
# Build the image and save it to the same directory
docker buildx build --platform linux/amd64,linux/arm64 \
  --output=type=local,dest=$PWD \
  --target=prod \
  -f Dockerfile \
  --tag multiatch-hello:0.1 .
```

```bash
docker container stop buildx_buildkit_builder0
docekr container stop registry
```