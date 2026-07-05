# Cache DNS responses for outgoing connections
For outgoing connections when using `DNS names` repeated DNS querries are done,
because the responses usually are not stored properly.  
This can flood `DNS servers` which should be avoided.

here is an example for `Docker` to use the hosts `systemd-resolved`:
  - Get the `docker0` network ip: `ip a s` (default: `172.17.0.1`)
  - Create the following configuration in`/etc/systemd/resolved.conf` (owner `root`, file permission `644`):
    ```ini
    [Resolve]
      # Some examples of DNS servers which may be used for DNS= and FallbackDNS=:
      # Cloudflare: 1.1.1.1#cloudflare-dns.com 1.0.0.1#cloudflare-dns.com 2606:4700:4700::1111#cloudflare-dns.com 2606:4700:4700::1001#cloudflare-dns.com
      # Google:     8.8.8.8#dns.google 8.8.4.4#dns.google 2001:4860:4860::8888#dns.google 2001:4860:4860::8844#dns.google
      # Quad9:      9.9.9.9#dns.quad9.net 149.112.112.112#dns.quad9.net 2620:fe::fe#dns.quad9.net 2620:fe::9#dns.quad9.net
      #DNS=
      #FallbackDNS=
      #Domains=
      #DNSSEC=no
      #DNSOverTLS=no
      #MulticastDNS=no
      #LLMNR=no
      #Cache=no-negative
      Cache=yes
      #CacheFromLocalhost=no
      #DNSStubListener=yes
      #DNSStubListenerExtra=
      DNSStubListenerExtra=<docker0-IP>
      #ReadEtcHosts=yes
      #ResolveUnicastSingleLabel=no
      #StaleRetentionSec=0
    ```
  - Restart the `systemd-resolved` service:
    ```sh
    sudo systemctl restart systemd-resolved.service
    ```
