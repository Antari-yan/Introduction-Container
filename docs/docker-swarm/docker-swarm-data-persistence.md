# Docker Swarm data persistence
When considering data persistence in Docker Swarm it is important to node that `mounts` and `named volumes`
are only done on one specific node.  
While `named volumes` are created automatically in case of a `failover`, the data is not transfered.  
THis can lead to inconsitent data and other issues and should be considered carefully.

To replicate small things like configuration or password files the `config` and `secret` optionms can be used.  
But both of them can not be changed as long as the stack is running,
which makes them not a good solution for e.g. certificates that rotate regularly.

As a workaround trick for general files, it is possible to add a `service` that is deployed in `global` mode (on all nodes),
and than creates files to a `named volume` where that volume is also attached to the actual service.  
This concept is an `init container`.

A better option is to use network storage like `NFS` or `S3` or cluster-aware storage like `GlusterFS`.

For clustered services (recommended for `databases`) it would be recommended to deploy them on all nodes and make them store their data on each node.

> [!NOTE]  
> The volume type `cluster` was added in the [Docker Engine API v1.42](https://docs.docker.com/reference/api/engine/version-history/#v142-api-changes)
> and is listed as option in the [Compose Reference](https://docs.docker.com/reference/compose-file/services/#long-syntax-6),
> but so far is barely explained and poorly documented.

> [!NOTE]  
> `NFS` only supports authentication from versions `4` onwards.  
> For older versions IP whitelisting and firewall rules are the only option.
