# Health probes
Kubernetes uses probes to know whether a container is started, healthy and ready:
  - `startupProbe`: Has a slow-starting container finished booting? It holds off the other probes until it passes.
  - `livenessProbe`: Is the container still working? On failure it is restarted (recovers from deadlocks).
  - `readinessProbe`: Is the container ready to serve? On failure the `pod` is removed from `Service` endpoints (no traffic), but not restarted.

Each probe can check via `httpGet`, `tcpSocket` or `exec`.  
The web app ([example-web-app-deployment.yaml](../../kubernetes-files/example-web-app-deployment.yaml)) uses an HTTP `livenessProbe` on `/healthz` (process is up) and
an HTTP `readinessProbe` on `/readyz` (which only succeeds when `valkey` is reachable), so traffic is only sent once the dependency is available.  
The `valkey` `StatefulSet` uses a `tcpSocket` `livenessProbe`.
