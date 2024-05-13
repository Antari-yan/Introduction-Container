FROM python:alpine as builder
WORKDIR /hello
#RUN apk add --no-cache binutils patchelf gcc musl-dev
#RUN pip install --no-cache-dir pyinstaller staticx
RUN apk add --no-cache binutils
RUN pip install --no-cache-dir pyinstaller
COPY hello.py .
RUN pyinstaller --onefile hello.py -n hello
CMD ["/hello/dist/hello"]

# PyInstaller builds aren't fully static (libc and ld.so are still required), through staticx that can be fixed
# https://github.com/pyinstaller/pyinstaller/wiki/FAQ#gnulinux

# docker build -t hello:py -f Dockerfile.py .
# docker container run --rm hello:py
# docker images hello:py
# docker image prune --filter label=stage=builder
