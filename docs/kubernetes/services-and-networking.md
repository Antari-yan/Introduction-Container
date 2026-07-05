# Services and Networking
Each `pod` gets its own IP, but `pods` are ephemeral and their IPs change on every restart.  
A `Service` provides a stable virtual IP and a DNS name in front of a set of `pods` (selected by labels).  

Different `Service` types can be used:
  - `ClusterIP` (default): Only reachable inside the cluster.
  - `NodePort`: Exposes the `Service` on a port on every node.
  - `LoadBalancer`: Requests an external IP, in `k3s` implemented by `ServiceLB`.
  - [Ingress](../../kubernetes-files/example-web-app-ingress.yaml) is not a `Service` type, it is a separate object routing HTTP(S) by host/path to `ClusterIP` `Services`.

In-cluster DNS always follows the pattern `<service>.<namespace>.svc.cluster.local`.  
Inside the same namespace the short `<service>` name can also be used.

Resolve the `Service` names from a temporary `pod` (DNS works regardless of `NetworkPolicies`):
```sh
# kubernetes-files/example-web-app-service.yaml
kubectl run tmp --rm -it --image=busybox --restart=Never -- nslookup example-web-app.example-web-app.svc.cluster.local

# kubernetes-files/valkey-service.yaml
kubectl run tmp --rm -it --image=busybox --restart=Never -- nslookup valkey.example-web-app.svc.cluster.local
```

### Gateway API
The [Gateway API](https://github.com/kubernetes-sigs/gateway-api) is the successor to `Ingress` and increasingly the preferred way to expose HTTP(S).  
Its core resources reached `v1` (GA), but it does not deprecate `Ingress`, both will coexist for a while.

It splits the single `Ingress` object into role-oriented resources:
  - `GatewayClass`: Which controller implements it (provided by the infrastructure, e.g. Traefik).
  - `Gateway`: The entry points (ports, protocols, TLS), owned by the cluster operator.
  - `HTTPRoute`: The routing rules (host, path, headers, traffic splitting), owned by the app team.

Compared to `Ingress` it natively supports what previously needed controller-specific annotations,
like header-based routing, traffic splitting (e.g. canary releases) and routes across namespaces.  
It is also portable across implementations (Traefik, Cilium, Envoy/Istio, NGINX, ...).

[example-web-app-gateway.yaml](../../kubernetes-files/example-web-app-gateway.yaml) expresses the same routing as the `Ingress`, on its own host so the two run side by side.

The Gateway API CRDs ship with `k3s`, but the Traefik Gateway provider is off by default and needs to be enabled:
```sh
kubectl apply -f kubernetes-bootstrap/traefik-gateway-config.yaml
kubectl -n kube-system rollout status deployment/traefik   # wait for Traefik to re-roll with the new config
kubectl get gatewayclass                                    # "traefik" now exists
kubectl get gateway,httproute -n example-web-app            # the Gateway is now accepted
curl http://example-web-app-gateway.k3d.localhost:8080
```

### Network policies
By default every `pod` can talk to every `pod` in any namespace.  
This can be restricted with a `NetworkPolicy`, so that once a `pod` is selected by any policy, all traffic that is not explicitly allowed is denied.

[networkpolicy.yaml](../../kubernetes-files/networkpolicy.yaml) denies all ingress, then allows exactly what the app needs:  
  - `example-web-app` reachable on 8080 from the ingress controller and from `pods` labeled `access=example-web-app`
  - `valkey` reachable on 6379 only from the `example-web-app` `pods`

To verify it:
```sh
# allowed (carries the label) -> returns the page
kubectl run client-allowed --rm -it --labels=access=example-web-app --image=busybox --restart=Never -- wget -qO- --timeout=5 http://example-web-app:8080

# denied (no label) -> connection refused
kubectl run client-denied --rm -it --image=busybox --restart=Never -- wget -qO- --timeout=5 http://example-web-app:8080
```
