# Parallel computing (MPI / OpenMP)
Two models of parallelism are common in scientific and high-performance computing:
  - `MPI` (Message Passing Interface): parallelism across processes (called ranks),
    which may run on different hosts and communicate by passing messages.
  - `OpenMP`: process parallelization, across threads that share one host's memory.

The example program [pi-estimation.c](../../data/example-parallel-computing/src/pi-estimation.c)
estimates pi by numeric integration.

The container image in [data/example-parallel-computing](../../data/example-parallel-computing/Dockerfile) builds the program and sets up SSH for `mpirun`:
```sh
$CR build -t example-parallel-computing ./data/example-parallel-computing
```


## Docker Compose
Deploy [docker-compose.parallel-computing.yml](../../compose-files/docker-compose.parallel-computing.yml) from the project root:
```sh
$CR compose -f compose-files/docker-compose.parallel-computing.yml up -d
```

Launch [run-pi-estimation.sh](../../data/example-parallel-computing/run-pi-estimation.sh):
```sh
$CR compose -f compose-files/docker-compose.parallel-computing.yml exec mpi-nodes /home/mpiuser/run-pi-estimation.sh
```
First a serial run will be made, followed by a parallel run.  
Afterwards the output will show the result of both and the difference in time.

> [!NOTE]  
> Set `MPI_IFACE` (default `eth0`) with `exec -e MPI_IFACE=...` if the nodes use a different interface and
> `exec -e OMP_NUM_THREADS=...` to change the number of threads.

> [!NOTE]  
> In the compose file each replica has a resource limit defined to a total of `~1 CPU Cores` and `~384 MB RAM`.  
> This is to simulate multiple nodes, while preventing the replicas from fighting each other over resources on the host.  
> Reduce them if the host has less resources available.

Stop the deployment:
```sh
$CR compose -f compose-files/docker-compose.parallel-computing.yml down
```

For a Docker Swarm deployment replace `replicas:` with `mode: global` in the `deploy` section,
adjust the resource limits to fit the host nodes (leave enough for the host processes) and run:
```sh
docker stack deploy example-parallel-computing -c compose-files/docker-compose.parallel-computing.yml
docker exec $(docker container ls --filter name=example-parallel-computing_mpi-nodes --quiet) /home/mpiuser/run-pi-estimation.sh
docker stack rm example-parallel-computing
```


## Kubernetes
On Kubernetes the [Kubeflow MPI Operator](https://github.com/kubeflow/mpi-operator)
provides a simple and easy to use solution for running MPI jobs through `MPIJob` objects which:
- Creates the launcher and worker pods
- Generates the SSH key `Secret` and the hostfile
- Connects the Pods together over network

Install the operator once (it registers the `MPIJob` CRD and runs its controller):
```sh
kubectl apply --server-side -f https://raw.githubusercontent.com/kubeflow/mpi-operator/v0.8.2/deploy/v2beta1/mpi-operator.yaml
```

Import the image into the cluster:
```sh
k3d image import example-parallel-computing:latest -c introduction
```

[kubernetes/parallel-computing/example-parallel-computing.yaml](../../kubernetes/parallel-computing/example-parallel-computing.yaml)
defines the `namespace` and the `MPIJob`:
```sh
kubectl apply -f kubernetes/parallel-computing/example-parallel-computing.yaml
```

Watch the job and read the result from the launcher's logs:
```sh
kubectl -n example-parallel-computing get mpijob pi-estimation
kubectl -n example-parallel-computing logs -f job/pi-estimation-launcher
```

Clean up (the operator stays installed):
```sh
kubectl delete -f kubernetes/parallel-computing/example-parallel-computing.yaml
```

> [!NOTE]  
> `slotsPerWorker: 1` sets one MPI rank per worker.  
> The thread count and binding are set on the launcher's `mpirun` args.


## Using GPUs
The provided deployment examples use CPU acceleration.  
GPU acceleration can be used by:
- Enabling the corresponding lines in the deployment files
- Adding GPU support in the images (e.g. installing and enabling CUDA requirements)
- Using the corresponding pragma in the Code.  

### Cluster setup
Make the cluster GPU aware by deploying the corresponding GPU Operator:
```sh
# NVIDIA
# https://github.com/NVIDIA/gpu-operator
helm repo add nvidia https://helm.ngc.nvidia.com/nvidia
helm repo update
helm install gpu-operator nvidia/gpu-operator -n gpu-operator --create-namespace

# AMD
# https://github.com/ROCm/gpu-operator
helm repo add rocm https://rocm.github.io/gpu-operator
helm repo update
helm install amd-gpu-operator rocm/gpu-operator-charts -n kube-amd-gpu --create-namespace
```

Specifically for k3d + CUDA, which runs k3s in a container, changing the base image that runs the cluster may be required:
```dockerfile
# Set the tags to versions matching the k3s release and the host GPU driver.
# A -base- CUDA variant is enough here; the workload image needs -runtime-.
ARG K3S_TAG=v1.36.3-k3s1
ARG CUDA_TAG=13.3.1-base-ubuntu26.04

FROM rancher/k3s:${K3S_TAG} AS k3s
FROM nvcr.io/nvidia/cuda:${CUDA_TAG}

# NVIDIA container toolkit, then point containerd at the nvidia runtime.
RUN apt update && apt install -y --no-install-recommends curl gnupg ca-certificates \
    && curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey \
         | gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg \
    && curl -sL https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list \
         | sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' \
         > /etc/apt/sources.list.d/nvidia-container-toolkit.list \
    && apt update && apt install -y --no-install-recommends nvidia-container-toolkit \
    && nvidia-ctk runtime configure --runtime=containerd \
    && rm -rf /var/lib/apt/lists/*
 
COPY --from=k3s / / --exclude=/bin
COPY --from=k3s /bin /bin
 
VOLUME /var/lib/kubelet
VOLUME /var/lib/rancher/k3s
VOLUME /var/lib/cni
VOLUME /var/log
ENV PATH="$PATH:/bin/aux"
ENTRYPOINT ["/bin/k3s"]
CMD ["agent"]
```
```sh
sudo apt install nvidia-container-toolkit
docker build -t k3s-cuda .
sudo nvidia-ctk runtime configure --runtime=docker
sudo systemctl restart docker
k3d cluster create introduction --image k3s-cuda --gpus all

helm install gpu-operator nvidia/gpu-operator -n gpu-operator --create-namespace \
  --set driver.enabled=false --set toolkit.enabled=false
```

### Application deployment
Next adjust the Code `pragma` to use the desired CUDA (NVIDIA) or HIP (AMD) kernel and
use a compatible compiler like `clang`:
```sh
# NVIDIA (CUDA toolkit present in the build stage)
clang -O2 -fopenmp --offload-arch=sm_XX  -o pi-estimation src/pi-estimation.c
# AMD (ROCm present in the build stage)
clang -O2 -fopenmp --offload-arch=gfxYYY -o pi-estimation src/pi-estimation.c
```
A vendor image (`nvidia/cuda:*` or `rocm/dev-*`) can help with simplifying the dependency handling.

Lastly adjust the deployment file to add the GPUs to the container (check the comments in the corresponding files) and deploy it.
