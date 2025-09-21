#!/usr/bin/env bash

# Declare an associative array to count container occurrences per Process/Thread
declare -A container_process_count
declare -A container_thread_count

# Iterate over all PIDs (processes) in /proc
for pid in $(ls /proc | grep '^[0-9]*$'); do
    # Iterate over all TIDs (threads) of the process
    for tid in $(ls /proc/$pid/task 2>/dev/null); do
        # Extract container ID from the mountinfo of the TID
        container_id=$(cat /proc/$tid/mountinfo 2>/dev/null | \
                       grep '/var/lib/docker/containers/' | \
                       grep hostname | \
                       awk -F'/containers/' '{print $2}' | \
                       awk -F'/' '{print $1}')
        if [[ -n $container_id ]]; then
            # Increment count for this container ID
            container_thread_count["$container_id"]=$((container_thread_count["$container_id"] + 1))
        fi
    done

    if [[ -n $container_id ]]; then
        # Increment count for this container ID
        container_process_count["$container_id"]=$((container_process_count["$container_id"] + 1))
    fi
done

# Filter container IDs that occur more than 5 times
output=()
for container_id in "${!container_thread_count[@]}"; do
    if [[ ${container_thread_count["$container_id"]} -gt 5 ]]; then
        container_name=$(docker ps --filter id=$container_id --format {{.Names}})
        output+=("$container_name: ${container_process_count["$container_id"]} Processes, ${container_thread_count["$container_id"]} Threads")
    fi
done

# Sort and print the output
printf "%s\n" "${output[@]}" | sort
