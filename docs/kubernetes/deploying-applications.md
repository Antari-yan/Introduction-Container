# Deploying applications
A comparable application deployment to the [compose example](../../compose-files/docker-compose.webapp-and-storage.yml)
can be found in the [kubernetes-files](../../kubernetes-files) directory:
  - A python web frontend [simple_python_web_app](../../kubernetes-files/example-web-app-deployment.yaml)
  - A [valkey](../../kubernetes-files/valkey-statefulset.yaml) key-value store backed
  - All individual pieces required for configuration, networking, access control, autoscaling, etc.

Since the image is built locally and not in a container registry, it needs to be imported into the cluster (the nodes have their own image store):
```sh
$CR build -t simple_python_web_app ./data/example-web-app
k3d image import simple_python_web_app:latest -c introduction
```

Create the `Namespace` and switch the current context to it, so the following commands do not need `-n example-web-app`:
```sh
kubectl apply -f kubernetes-files/example-web-app-namespace.yaml
kubectl config set-context --current --namespace=example-web-app
```

The web app and valkey share a password from a `Secret`, which is created out-of-band so it never lands in Git:
```sh
echo "$(openssl rand -base64 24)" > data/valkey-password
kubectl create secret generic valkey-credentials --from-file=password=data/valkey-password
```

Deploy the whole application with a single command:
```sh
kubectl apply -f kubernetes-files/
```

This creates everything at once: the `example-web-app` `Deployment` (managed via a `ReplicaSet`),
the `valkey` `StatefulSet`, their `Services`, a `ConfigMap`, an `Ingress`, a `HorizontalPodAutoscaler`, RBAC, `NetworkPolicies` and two `Jobs`.  
Each of these will be explained in their own sections below.

Watch it come up and reach it through a port-forward:
```sh
kubectl get pods
kubectl rollout status deployment/example-web-app

kubectl port-forward svc/example-web-app 8081:8080
# In another shell:
curl http://localhost:8081
# <h1>Hello from Kubernetes</h1> ... viewed N times.
```

> [!NOTE]  
> Access through the `Ingress` over HTTPS is wired up later once [cert-manager](operators-and-custom-resources.md#self-signed-certificates-with-cert-manager) is installed.  
> A `port-forward` works immediately for testing and is independent of `Ingress` and `NetworkPolicies`.

Inspect and debug a `pod` like a standalone container:
```sh
POD=$(kubectl get pods -l app=example-web-app -o jsonpath='{.items[0].metadata.name}')
kubectl logs $POD
kubectl exec -it $POD -- /bin/sh
```
