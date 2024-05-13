#FROM debian:stable-slim as builder
#LABEL stage=builder
#RUN apt update && apt install -y gcc
#COPY hello.c /
#RUN gcc -o helloworld -static-libgcc -static hello.c
# ~ 762kB image

FROM alpine as builder
LABEL stage=builder
RUN apk add --no-cache gcc musl-dev
COPY hello.c /
RUN gcc -o helloworld -static-libgcc -static hello.c
 
FROM scratch
COPY --from=builder /helloworld /
CMD ["/helloworld"]


# docker build -t hello:c -f Dockerfile.c .
# docker container run --rm hello:c
# docker images hello:c
# docker image prune --filter label=stage=builder
