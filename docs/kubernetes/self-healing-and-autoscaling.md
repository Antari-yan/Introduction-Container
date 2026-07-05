# Self-healing and Autoscaling
The self-healing mechanisim in kuberentes ensures that the desired state matches with the real state (if possible):
```sh
# Delete a pods and see that it automatically is recreated
kubectl delete pod $(kubectl get pods -l app=example-web-app -o jsonpath='{.items[0].metadata.name}')
kubectl get pods -l app=example-web-app --watch
```

Kubernetes can scale automatically on three levels:
  - `HorizontalPodAutoscaler` (HPA): Adds or removes `pod` replicas based on metrics (CPU, memory or custom). Built in, relies on `metrics-server` (bundled in `k3s`).
  - `VerticalPodAutoscaler` (VPA): Adjusts a `pod`'s CPU/memory requests and limits to match actual usage. Installed separately.
  - `Cluster Autoscaler` / [Karpenter](https://github.com/kubernetes-sigs/karpenter): Adds or removes whole nodes when `pods` cannot be scheduled or nodes sit idle. Depends on the infrastructure.

The web app already has an HPA ([example-web-app-hpa.yaml](../../kubernetes-files/example-web-app-hpa.yaml), 2-5 replicas at 50% CPU):
```sh
kubectl get hpa example-web-app
```

Verify its function by generating load and watching it scale up:
```sh
# Start a load generator that hammers the Service with many parallel requests
kubectl run load --image=busybox --restart=Never --labels=access=example-web-app -- /bin/sh -c \
  'i=0; while [ $i -lt 30 ]; do (while true; do wget -q -O /dev/null http://example-web-app:8080/healthz; done) & i=$((i+1)); done; wait'

# Watch the replica count climb towards maxReplicas as the CPU target is exceeded
kubectl get hpa example-web-app --watch

kubectl get deployments.apps example-web-app
```

Stop the load again and the HPA scales back down after its stabilization window (5 minutes by default):
```sh
kubectl delete pod load
```

> [!IMPORTANT]  
> Always set a sensible `maxReplicas`.  
> Without an upper bound a traffic spike (or a runaway client) can scale a workload until it exhausts the cluster's resources and starves everything else.

> [!NOTE]  
> Because the HPA manages the replica count, the `example-web-app` `Deployment` deliberately omits the `replicas` field.  
> Setting both would let a re-apply fight the HPA, the HPA's `minReplicas` takes over that role.

> [!NOTE]  
> HPA and VPA should not manage the same metric (e.g. CPU) on the same workload, as they would work against each other.
