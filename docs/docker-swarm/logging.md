# Logging
There are two main options when checking logs of a container:
  - `docker service logs <service-name>`
  - `docker logs <task-name>`

The `service` logs combine the logs of all the underlying `tasks`.  
Important to note is that the service logs can be quite difficcult to read, becasue they are just lumped together how they are received and not ordered.

Reading the logs on one node can often be easier to do:
`docker logs $(docker container ls --filter status=running --filter name=<task-name> --quiet) -n 50 2>&1 | grep -i "error"`
