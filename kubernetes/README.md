

ubnutu open file limit
ulimit -n
sudo /bin/bash
cat >/etc/security/limits.conf <<EOF
*               hard    nofile           100000
EOF
reboot