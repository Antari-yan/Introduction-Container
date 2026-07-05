# Parallel computing (MPI / OpenMP)
Containers can be used for MPI workloads (e.g. scientific simulations):
- Network performance: use `--network host` or pass through an RDMA device.
- Match the MPI library version between the container and the host MPI launcher.
- For multi-node MPI, ensure the same container image is available on all nodes.
