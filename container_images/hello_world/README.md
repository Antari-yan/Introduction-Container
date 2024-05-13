# Running binaries from scratch
```bash
# Python, unstatic binary
docker build --no-cache -t hello:py -f Dockerfile.py .
docker container run --rm hello:py
docker images hello:py
# ~97.1MB
```

```bash
# Python, static binary
docker build --no-cache -t hello:py2 -f Dockerfile.py2 .
docker container run --rm hello:py2
docker images hello:py2
# ~18.6MB
```

```bash
# Go
docker build --no-cache -t hello:go -f Dockerfile.go .
docker container run --rm hello:go
docker images hello:go
# ~1.8MB
```

```bash
# C
docker build --no-cache -t hello:c -f Dockerfile.c .
docker container run --rm hello:c
docker images hello:c
# ~118kB
```

```bash
# Cleanup
docker image prune --filter label=stage=builder
```
