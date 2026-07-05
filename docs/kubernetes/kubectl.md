# kubectl
[kubectl](https://github.com/kubernetes/kubectl) is the most commonly used CLI tool for interacting with kubernetes cluster (comparable the `podman` and `docker` CLIs):
```sh
# Ensure the installed kubectl version is compatible with the used kubernetes versionb
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl

sudo sh -c "kubectl completion bash > /etc/bash_completion.d/kubectl-completion"
source /etc/bash_completion.d/kubectl-completion

kubectl version --client

export KUBECONFIG=~/.kube/config
kubectl get nodes
```

| Description | kubectl | Podman/Docker equivalent |
| --- | --- | --- |
| Cluster/host info | `kubectl cluster-info` | `$CR system info` |
| List nodes | `kubectl get nodes` | (single host) |
| List workloads | `kubectl get pods` | `$CR container ls` |
| Detailed info | `kubectl describe <object> <name>` | `$CR container inspect <name>` |
| Logs | `kubectl logs <pod>` | `$CR container logs <name>` |
| Exec into it | `kubectl exec -it <pod> -- /bin/sh` | `$CR container exec -it <name> /bin/sh` |
| Apply a definition | `kubectl apply -f <file>` | `$CR compose up -d` |
| Remove a definition | `kubectl delete -f <file>` | `$CR compose down` |

Useful flags that work on most commands:
```sh
kubectl get pods -o wide          # more columns (node, IP, ...)
kubectl get pods -o yaml          # full object as YAML
kubectl get all                   # all common objects in the namespace
kubectl get pods --watch          # stream changes
kubectl get pods -A               # All pods in all namespaces
kubectl get pods -n <namespace>   # All pods in a specific namespaces
```
