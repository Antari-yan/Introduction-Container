FROM alpine:3 AS builder
LABEL stage=builder

COPY src/hello.c /

RUN apk add --no-cache gcc musl-dev
RUN gcc -o helloworld -static-libgcc -static hello.c
 
FROM scratch
COPY --from=builder /helloworld /
CMD ["/helloworld"]
