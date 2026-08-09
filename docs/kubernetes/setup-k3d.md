# Setup k3d
[k3d](https://github.com/k3d-io/k3d/) is a small wrapper that runs `k3s` inside container.  
It uses the already available Docker or can be pointed at Podman, which makes it ideal for development, testing and CI.

A `k3d` cluster is just a set of container:
  - one `serverlb` container (a [Traefik](https://github.com/traefik/traefik) based loadbalancer that forwards mapped host ports into the cluster)
  - one container per `server` (control plane) node
  - one container per `agent` (worker) node

### Installing k3d
```sh
curl -s https://raw.githubusercontent.com/k3d-io/k3d/main/install.sh | bash
# Pin a version with: curl -s https://raw.githubusercontent.com/k3d-io/k3d/main/install.sh | TAG=v5.8.3 bash
k3d version
```

> [!NOTE]  
> `k3s` does not ship a plain Kubernetes, it bundles a set of components that are otherwise installed separately into a single binary:
>   - `CoreDNS` for in-cluster DNS
>   - `Traefik` as the `Ingress` controller
>   - `ServiceLB` (klipper) to implement `Service` type `LoadBalancer`
>   - `local-path` provisioner as the default `StorageClass`
>   - `metrics-server` for `kubectl top`

### Running k3d with Podman
`k3d` uses the Docker API, but Podman provides a compatible socket, so it can be used as the runtime instead.

Enable the Podman socket and point `k3d` at it:
```sh
systemctl --user enable --now podman.socket
ln -s /run/podman/podman.sock /var/run/docker.sock
export DOCKER_HOST=unix://$XDG_RUNTIME_DIR/podman/podman.sock
k3d cluster create introduction --port "8080:80@loadbalancer"
```

> [!IMPORTANT]  
> Podman support in `k3d` is officially experimental.  
> Podman has no default `bridge` network, so for a local registry use `--default-network podman` and avoid `--registry-create`.  
> Details: <https://k3d.io/stable/usage/advanced/podman/>

### Creating a cluster
Set the `CR` variable depending on what is installed, so the underlying container can be inspected:
  - Podman: `CR=podman`
  - Docker: `CR=docker`

Create a cluster with 1 server, 2 agents and the loadbalancer mapping host port `8080` to port `80` inside the cluster, from a config file:
```sh
k3d cluster create --config kubernetes/bootstrap/k3d-config.yaml

# k3d cluster create introduction \
#   --servers 1 \
#   --agents 2 \
#   --port "8080:80@loadbalancer" \
#   --port "8443:443@loadbalancer"
```

Look at the container that make up the cluster:
```sh
$CR container ls --filter name=k3d-introduction
```

Manage the cluster lifecycle:
```sh
k3d cluster list
k3d cluster stop introduction   # stops the container, keeps all data
k3d cluster start introduction  # resumes where it left off
# k3d cluster delete introduction  # removes the cluster and its data
```

> [!NOTE]  
> Creating a cluster also writes a context named `k3d-introduction` into `~/.kube/config` and switches to it.  
> List and switch contexts with:
>   - `kubectl config get-contexts`
>   - `kubectl config use-context k3d-introduction`

To interact with the set the `KUBECONFIG` env var pointing to the file created by `k3s`:
```sh
export KUBECONFIG=~/.kube/config
```
