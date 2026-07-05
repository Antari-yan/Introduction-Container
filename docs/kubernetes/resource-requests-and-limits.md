# Resource requests and limits
Each container can declare how much CPU and memory it needs, which drives scheduling and stability (and is required for the [HPA](self-healing-and-autoscaling.md)).
  - `requests`: The guaranteed amount, used by the scheduler to place the `pod`. The sum of requests cannot exceed a node's capacity.
  - `limits`: The hard ceiling. Exceeding the memory limit gets the container `OOMKilled`, exceeding the CPU limit throttles it.

The web app sets them:
```yaml
    resources:
      requests:
        cpu: 25m       # 25 millicores = 0.025 of a CPU core
        memory: 32Mi
      limits:
        cpu: 100m
        memory: 64Mi
```

The ratio of requests to limits determines the `pod`'s `Quality of Service` class, which decides what gets evicted first when a node runs out of memory:
  - `Guaranteed`: requests equal limits for every container. Evicted last.
  - `Burstable`: requests set but lower than limits (the example above). Evicted in between.
  - `BestEffort`: nothing set. Evicted first.

> [!IMPORTANT]  
> Always set memory `requests` and `limits`.  
> A `pod` without limits can consume a whole node and trigger `OOMKills` of other workloads, the same starvation problem behind [why workloads stay off the control plane](architecture.md).
