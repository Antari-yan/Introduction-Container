# Running kubernetes manifests directly with Podman
Podman is capable of running plain Kubernetes manifests and Helm charts directly via `podman kube play`.  
>While limited (no `Deployments` scaling, `Services`, `Ingress`, ...) it can still be useful for testing a manifest quickly.

Create a manifest from the Helm template:
```sh
helm template webserver ./helm-files > ./data/webserver-k8s.yaml
```

Run the deployment:
```sh
podman kube play --replace data/webserver-k8s.yaml
```

Check state:
```sh
podman pod ps
podman ps
```

Stop the deployment:
```sh
podman kube play --down data/webserver-k8s.yaml
```

> [!NOTE]  
> Older Podman Version (like 3.4.4) use different commands:
>   - `podman play kube data/webserver-k8s.yaml`
>   - `podman pod stop webserver-pod-0`
>   - `podman pod rm webserver-pod-0`
