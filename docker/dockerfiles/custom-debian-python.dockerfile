FROM debian:12-slim

WORKDIR /usr/src/app

RUN apt update && apt upgrade -y
RUN apt install curl python3 python3-pip -y
