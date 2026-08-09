# Parallel computing with MPI + OpenMP example image.
#
# Every node runs this same image:
#   - nodes run sshd for communication via SSH
#   - mpirun is executed manually

FROM debian:13-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        gcc \
        libopenmpi-dev \
    && rm -rf /var/lib/apt/lists/*

COPY src/pi-estimation.c /src/pi-estimation.c
# mpicc adds the MPI headers/libraries nad '-fopenmp' enables the OpenMP pragmas.
# libgomp (the OpenMP runtime) is linked statically so the runtime stage doesn't need to install it.
RUN mpicc -O2 -fopenmp -Wall -Wextra -o /pi-estimation /src/pi-estimation.c \
        -Wl,-Bstatic -lgomp -Wl,-Bdynamic


FROM debian:13-slim
ARG SSH_PORT=2222

# openmpi-bin provides mpirun and orted (the per-node launch helper).
# The openssh packages let mpirun start orted on the other nodes.
# sshd runs as non-root on an unprivileged port, so no extra capability is needed.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        openmpi-bin \
        openssh-server \
        openssh-client \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /var/run/sshd

# Client-side SSH config for unprivileged SSH port
RUN sed -i "s/[ #]\(.*StrictHostKeyChecking \).*/ \1no/g" /etc/ssh/ssh_config \
    && echo "    UserKnownHostsFile /dev/null" >> /etc/ssh/ssh_config \
    && sed -i "s/[ #]\(.*Port \).*/ \1${SSH_PORT}/g" /etc/ssh/ssh_config

# Dedicated unprivileged user that owns and runs the MPI processes.
RUN useradd --create-home --shell /bin/bash mpiuser
WORKDIR /home/mpiuser

# id_rsa doubles as the sshd host key (see sshd_config) and the client key.
# On Kubernetes the MPI Operator overlays its own key
USER mpiuser
RUN mkdir -p /home/mpiuser/.ssh \
    && ssh-keygen -t rsa -b 3072 -N '' -f /home/mpiuser/.ssh/id_rsa \
    && cp /home/mpiuser/.ssh/id_rsa.pub /home/mpiuser/.ssh/authorized_keys \
    && chmod 700 /home/mpiuser/.ssh \
    && chmod 600 /home/mpiuser/.ssh/id_rsa /home/mpiuser/.ssh/authorized_keys \
    && printf 'PidFile /home/mpiuser/sshd.pid\nHostKey /home/mpiuser/.ssh/id_rsa\nStrictModes no\nPort %s\n' "$SSH_PORT" \
        > /home/mpiuser/.sshd_config

COPY --from=build --chown=mpiuser /pi-estimation /home/mpiuser/pi-estimation

# Launcher helper for Docker to discovers the mpi-nodes replicas and
# run the job.
# Not used on Kubernetes, where the MPI Operator drives mpirun.
COPY --chown=mpiuser --chmod=0755 run-pi-estimation.sh /home/mpiuser/run-pi-estimation.sh

EXPOSE ${SSH_PORT}/tcp

# Run sshd in the foreground.
CMD ["/usr/sbin/sshd", "-De", "-f", "/home/mpiuser/.sshd_config"]

# # Build the image and run three nodes from the project root:
# docker build -t example-parallel-computing ./data/example-parallel-computing
