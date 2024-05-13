# Iterative images designs for an API App



https://github.com/GoogleContainerTools/distroless
docker image build --squash
https://github.com/slimtoolkit/slim



https://medium.com/analytics-vidhya/dockerizing-a-rest-api-in-python-less-than-9-mb-and-based-on-scratch-image-ef0ee3ad3f0a
https://pyinstaller.org/en/stable/usage.html

```bash
# based on Python Debian image
docker build --no-cache -t rndapi:0.1 -f Dockerfile.1 .
docker image ls rndapi:0.1
# ~1.08GB
```

```bash
# Based on Python Alpine image
docker build --no-cache -t rndapi:0.2 -f Dockerfile.2 .
docker image ls rndapi:0.2
# ~71MB
```

```bash
# Based on Python Alpine image but with script converted to binaries
docker build --no-cache -t rndapi:0.3 -f Dockerfile.3 .
docker image ls rndapi:0.3
# ~119MB
```

```bash
# Based on Python Alpine image but with script converted to a single binary file
docker build --no-cache -t rndapi:0.4 -f Dockerfile.4 .
docker image ls rndapi:0.4
# ~112MB
```

```bash
# Based on Empty image and script converted to a single binary file
docker build --no-cache -t rndapi:0.5 -f Dockerfile.5 .
docker image ls rndapi:0.5
# ~34.1MB
```

```bash
# Based on Empty image and script converted to a single binary file
# Additionally Docstrings and more removed from python, as wells as striped binary
docker build --no-cache -t rndapi:0.6 -f Dockerfile.6 .
docker image ls rndapi:0.6
# ~33.3MB
```

```bash
docker image ls rndapi

docker container run -p 8080:8080 -d --rm rndapi:0.6
curl  -w "\n" 127.0.0.1:8080/number
docker container stop $(docker container ls -lq)
```

```bash
# Cleanup
docker image prune --filter label=stage=builder
```