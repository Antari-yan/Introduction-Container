#!/bin/sh
# Discover the mpi-nodes replicas and run pi-estimation, one rank per node.
#
# Swarm exposes the task IPs under tasks.<service>,
# for Docker standalone the service name is used.
#
# Usage (inside a node): run-pi-estimation.sh [STEPS]
#   MPI_IFACE        network interface MPI uses for TCP  (default eth0)
#   OMP_NUM_THREADS  OpenMP threads per rank             (default 2)
set -e

service=mpi-nodes
iface=${MPI_IFACE:-eth0}

# Collect the peer IPs (dedupe; ahostsv4 prints several lines per address).
hosts=$(getent ahostsv4 "tasks.${service}" 2>/dev/null || getent ahostsv4 "${service}")
hosts=$(echo "${hosts}" | awk '{ print $1 }' | sort -u)
np=$(echo "${hosts}" | wc -l)
hostlist=$(echo "${hosts}" | sed 's/$/:1/' | paste -sd,)

# Detect CPUs available to this container, honouring a CFS quota if one is set.
# Docker's `cpus` and Kubernetes' `limits.cpu` are bandwidth quotas, not cpusets,
# so nproc and OpenMP's own default would report the host's cores and oversubscribe the cap.
# Read the cgroup quota instead and fall back to nproc when unlimited.
available_cpus() {
    _quota=""; _period=""
    if [ -r /sys/fs/cgroup/cpu.max ]; then                    # cgroup v2
        read -r _quota _period < /sys/fs/cgroup/cpu.max
    elif [ -r /sys/fs/cgroup/cpu/cpu.cfs_quota_us ]; then     # cgroup v1
        _quota=$(cat /sys/fs/cgroup/cpu/cpu.cfs_quota_us)
        _period=$(cat /sys/fs/cgroup/cpu/cpu.cfs_period_us)
    fi
    if [ -n "$_quota" ] && [ "$_quota" != "max" ] && [ "$_quota" -gt 0 ] 2>/dev/null; then
        # Round down to whole CPUs, but not less than one
        _n=$(( _quota / _period ))       
        [ "$_n" -lt 1 ] && _n=1
        echo "$_n"
    else
        nproc
    fi
}
 
# Autodeted the amount of available CPUs and set the same amount of threads.
threads=${OMP_NUM_THREADS:-$(available_cpus)}

# '-np' equals the node count and mapping is by node, so this places exactly one rank per node.
# :OVERSUBSCRIBE is needed because Open MPI 5 (Debian 13) otherwise
# may miscount the containers' slots for the --host form and refuses the run.
exec mpirun --host "${hostlist}" -np "${np}" --map-by node:OVERSUBSCRIBE --bind-to none \
    --mca btl tcp,self --mca btl_tcp_if_include "${iface}" \
    -x OMP_NUM_THREADS="${threads}" \
    /home/mpiuser/pi-estimation "$@"
