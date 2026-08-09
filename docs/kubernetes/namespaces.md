# Namespaces
`Namespaces` group and isolate objects, e.g. per project or environment (the whole application above lives in `example-web-app`).  
`Services` in different namespaces are reachable via their full DNS name `<service>.<namespace>.svc.cluster.local`.

```sh
kubectl create namespace demo
kubectl create deployment demo --image=docker.io/nginx:1-alpine -n demo
kubectl get pods -n demo

# Switch the default namespace of the current context
kubectl config set-context --current --namespace=demo

# Clean up and switch back to the application namespace
kubectl delete namespace demo
kubectl config set-context --current --namespace=example-web-app
```

> [!NOTE]  
> Deleting a `Namespace` deletes everything inside it, which is the quickest way to tear down the whole application.

### LimitRange and ResourceQuota
Two namespaced objects keep a single `Namespace` from consuming everything (useful when several teams or environments share a cluster):
  - [LimitRange](../../kubernetes/manifests/example-web-app-limitrange.yaml): Per-container defaults and bounds. Containers without `requests`/`limits` get the defaults, and values outside the allowed min/max are rejected.
  - [ResourceQuota](../../kubernetes/manifests/example-web-app-resourcequota.yaml): A cap on the namespace as a whole, e.g. total CPU/memory requested and limited, or the number of objects of a kind.
```sh
kubectl describe resourcequota -n example-web-app
kubectl describe limitrange -n example-web-app
 
# valkey and the Jobs declare no resources of their own, so they receive the LimitRange defaults
kubectl get pod valkey-0 -n example-web-app -o jsonpath='{.spec.containers[0].resources}'; echo
```
 
> [!IMPORTANT]  
> Once a `ResourceQuota` constrains CPU/memory, every `pod` in the namespace must declare the matching `requests`/`limits` or it is rejected.  
> A `LimitRange` with defaults satisfies this automatically for workloads that do not set their own, which is why the two are usually deployed together.
