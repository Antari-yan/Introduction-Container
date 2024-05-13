FROM golang:alpine as builder
LABEL stage=builder
WORKDIR /go/src/github.com/example/hello
COPY go.mod .
RUN go mod download
COPY hello.go .
RUN CGO_ENABLED=0 \
    GOOS=linux \
    GOARCH=amd64 \
    go build -a -installsuffix nocgo -o hello .

#FROM alpine as builder
#LABEL stage=builder
#WORKDIR /go/src/github.com/example/hello
#RUN apk add --no-cache git make musl-dev go
#COPY go.mod .
#RUN go mod download
#COPY hello.go .
#RUN CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -a -installsuffix nocgo -o hello .
# ~ 1.85MB

FROM scratch
COPY --from=builder /go/src/github.com/example/hello /
CMD ["/hello"]


# docker build -t hello:go -f Dockerfile.go .
# docker container run --rm hello:go
# docker images hello:go
# docker image prune --filter label=stage=builder
