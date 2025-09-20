FROM golang:1-alpine AS builder
LABEL stage=builder

WORKDIR /go/src/github.com/example/hello
COPY src/go.mod .
COPY src/hello.go .

RUN go mod download

RUN CGO_ENABLED=0 \
    GOOS=linux \
    go build -a -installsuffix nocgo -o hello .


FROM scratch
COPY --from=builder /go/src/github.com/example/hello /
CMD ["/hello"]
