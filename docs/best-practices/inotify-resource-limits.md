# inotify resource limits
The `"too many open files"` error can also be caused by exhausting `inotify` resources.
This is especially common when running logging and metrics collection container
(e.g. Prometheus node-exporter, Filebeat, Loki promtail, etc.).

Check the current limits:
```sh
cat /proc/sys/fs/inotify/max_user_watches
cat /proc/sys/fs/inotify/max_user_instances
```

Temporarily increase the limits (resets on reboot):
```sh
sudo sysctl fs.inotify.max_user_watches=524288
sudo sysctl fs.inotify.max_user_instances=1024
```

To make the change persistent by creating a file `/etc/sysctl.d/99-inotify-limits.conf`:
```ini
fs.inotify.max_user_watches = 524288
fs.inotify.max_user_instances = 1024
```

To find which processes are consuming inotify watches:
```sh
# Count active watchers system-wide (kernel perspective)
sudo find /proc/*/fdinfo -type f -exec grep -H inotify {} + 2>/dev/null | wc -l

# Break down by PID and process name
sudo find /proc/*/fdinfo -type f -exec grep -H inotify {} + 2>/dev/null \
  | awk -F'/' '{print $3}' | sort | uniq -c | sort -nr \
  | while read count pid; do
      printf "%5s %s %s\n" "$count" "$pid" "$(ps -p $pid -o comm= 2>/dev/null)"
    done
```

> [!WARNING]
> These numbers only reflect the `current` steady state.
> During a rolling deployment, new containers start before old ones are removed,
> which can temporarily double the watcher count.  
> Size limits accordingly for large clusters.
