FROM golang as builder
 LABEL stage=builder
WORKDIR /go/src/github.com/example/hello
COPY go.mod .
RUN go mod download
COPY hello.go .
RUN CGO_ENABLED=0 \
    go build -a -installsuffix nocgo -o hello .
 
FROM scratch as dev
COPY --from=builder /go/src/github.com/example/hello /
ENTRYPOINT ["/hello"]
 
# starting second stage
FROM alpine as prod
COPY --from=builder /go/src/github.com/example/hello /bin
CMD /bin/hello