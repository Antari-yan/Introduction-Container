# Docker Swarm
Running containers with `Podman` or `Docker` is fine for local development, but a production environment needs:
  - Scalability - automatically running more replicas when demand grows.
  - Fault tolerance - restarting containers on failure, redistributing workloads if a node goes down.
  - Service discovery & networking - assigning DNS names and load balancing between replicas.
  - Configuration & secrets management - distributing configuration and credentials
  - Rolling updates & rollbacks - upgrading without downtime.

To achive this Docker's built-in orchestrator `Docker Swarm` can be used:
  - Cluster setup: A Swarm is initialized with `docker swarm init`, then other nodes join as managers or workers.
  - Deployment: Applications are deployed with `docker stack deploy -c docker-compose.yml myapp`.
  - Scaling: Each service can be scaled dynamically (`docker service scale myservice=5`), but these changes don't persist if a stack iss stopped and restarted.
  - Networking: Uses overlay networks that span all nodes; services are reachable by DNS names.
  - State management: `Swarm managers` maintain desired state, automatically reconciling failed tasks.
  - Replicas of a `service` are called `tasks`

Limitations:
  - Development of Swarm has slowed significantly.
    - Many bugs have accumulated over the years.
    - Some `compose` features are not available or are broken in Swarm.
  - Lacks advanced features like namespaces, CRDs, or mature ecosystem integration.
  - Best for small/simple setups or training, but not the industry standard today.

With `docker stack ps <stack-name>` it is possible to check the state of a stack.  
This also shows `tasks` that failed previously, which can get lengthy for some deployments.  
To only see the running `tasks`:
```sh
docker stack ps <stack-name> --format "table {{.ID}}\t{{.Name}}\t{{.CurrentState}}\t{{.Node}}" --filter "desired-state=running"
```

> [!Important]  
> Docker Swarm doesn't automatically read `.env` files, a workaround is to load that file beforehand so that it gets passed into the deployment:  
> `export $(grep -v '^#' .env | xargs) > /dev/null 2>&1; docker stack deploy <stack-name> && unset $(grep -v '^#' .env | sed -E 's/(.*)=.*/\1/' | xargs)`
