# Recommendations

## YAML anchor
Since `compose files` use the `YAML` syntax it is possible to use `YAML anchor` to reduce repetitive code.

Checkout the [docker-compose.yaml-anchor.yml](../../compose-files/docker-compose.yaml-anchor.yml) example.  
For further details check the [YAML reference](https://yaml.org/).

## Healthchecks
Use healthchecks whenever possible to ensure that deployments are properly functional.

Get the Healthcheck logs:
```sh
$CR inspect $($CR container ls --filter name=webserver --quiet) --format "{{json .State.Health }}" |
grep -Eo '"[^"]*" *(: *([0-9]*|"[^"]*")[^{}\["]*|,)?|[^"\]\[\}\{]*|\{|\},?|\[|\],?|[0-9 ]*,?' |
awk '{if ($0 ~ /^[}\]]/ ) offset-=4; printf "%*c%s\n", offset, " ", $0; if ($0 ~ /^[{\[]/) offset+=4}'
```
- The `inspect` section extracts the `.State.Health` field from a running container named `webserver`
- The `grep` section takes the returned `JSON` output from the `inspect` and breaks it into multiple lines
- The `awk` section basically makes a pretty-print of the content
For simplicity the package `jq` can also be used instead of `grep` and `awk`.

## Limit logging
In the logging section of a service it is possible to configure how logs from the container should be handeld.  
By default it doesn't limit the amount of logs which can completly fill the host disk space.  
Therefore limiting logs is always recommended:
```yaml
    logging:
      driver: "json-file"
      options:
        max-file: "5"
        max-size: "10m"
```

## Update config in parallel (mainly Docker Swarm)
When deploying multiple replicas of a service use the `update_config` option
to define how many services are allowed to be updated at the same time:
```yml
services:
  redis:
    image: valkey/valkey
    deploy:
      replicas: 6
      # placement:
      #   max_replicas_per_node: 1
      update_config:
        parallelism: 2
        delay: 10s
      restart_policy:
        condition: on-failure
```

## Limit capabilities
Capabilities are distinct units of privilege in the kernel.  
They are things like the ability to send raw IP packets, or bind to ports below 1024.
Most capabilities are required to manipulate the kernel/system, and these are used by the container runtime, but seldom used by the processes running inside the container.

Therefore to increase security it is recommended to enable as few capabilities as needed when running a container.

Get default Capabilities:
```sh
$CR run --rm fedora capsh --print
# $CR  run --network=host --rm alpine sh -c 'apk add -U libcap; capsh --print'
```

Drop all capabilities with `--cap-drop=ALL`:
```sh
$CR run --rm --cap-drop=ALL fedora capsh --print
```

Only add necessary capabilities:
```sh
$CR run --rm --cap-drop=ALL --cap-add=setuid --cap-add=setgid fedora capsh --print
```

For compose files use:
```yml
services:
  webserver:
    image: docker.io/nginx:1-alpine
    cap_add:
      - ...
    cap_drop:
      - ...
```

More information about capabilities can be found here:
  - [Linux capabilities man page](https://man7.org/linux/man-pages/man7/capabilities.7.html)
  - [Docker runtime privilege and linux capabilities](https://docs.docker.com/engine/containers/run/#runtime-privilege-and-linux-capabilities)
  - [Docker compsoe capabilities reference](https://docs.docker.com/reference/compose-file/services/#cap_add)

## Set resource limits
By default, containers can use as much CPU and memory as the host allows. This can lead to:
  - One misbehaving container consuming all RAM and causing the host to swap/crash.
  - One container monopolizing CPU and starving others.
  - Scaling of a service leading to resource exhaustion on the host

The limits can be defined like this:
```yaml
services:
  webserver:
    image: docker.io/nginx:1-alpine
    deploy:
      resources:
        reservations: # Soft guarantee of the resources if available
          cpus: '0.25'  # Percent of a Core
          memory: 20M
        limits: # Hard limit of resources that can be used, exceeding it can lead to the service being killed
          cpus: '0.50'
          memory: 50M
```
Often it is not easy to properly determine how many resources an applicaiton needs to properly run.  
For applications with a predefined load this can be done easily,
but for applications like web services the load can differ vastly making it more difficult.  
Since exceeding the resource limits lead to the service being killed, this option should be used carefully.
