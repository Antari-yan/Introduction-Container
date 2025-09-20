# Hello-World binary examples

This is a set of Hello-World examples with different programming languages packaged as container images.  
What is quite interesting to observe is the difference in the final image size.

```sh
CR=$(command -v podman || command -v docker)
```

## Python, dynamic binary
```sh
$CR build --no-cache -t hello:py-dynamic -f Dockerfile.py-dynamic .
$CR container run --rm hello:py-dynamic
$CR images hello:py-dynamic
```

## Python, static binary
```sh
$CR build --no-cache -t hello:py-static -f Dockerfile.py-static .
$CR container run --rm hello:py-static
$CR images hello:py-static
```

## Go
```sh
$CR build --no-cache -t hello:go -f Dockerfile.go .
$CR container run --rm hello:go
$CR images hello:go
```

## C
```sh
$CR build --no-cache -t hello:c -f Dockerfile.c .
$CR container run --rm hello:c
$CR images hello:c
```
