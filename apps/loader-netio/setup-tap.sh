sudo ip tuntap add tap100 mode tap
sudo ip addr add 10.0.5.1/24 broadcast 10.0.5.255 dev tap100
sudo ip link set dev tap100 up
sudo bash -c 'echo 1 > /proc/sys/net/ipv4/conf/tap100/proxy_arp'
