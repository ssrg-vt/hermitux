#sudo ip link add br0 type bridge
#sudo ip link set enp0s31f6 master br0
#sudo dhclient br0
#sudo ip tuntap add tap100 mode tap
#sudo ip addr add 192.168.1.4 broadcast 192.168.1.255 dev tap100
#sudo ip link set dev tap100 up
#sudo bash -c 'echo 1 > /proc/sys/net/ipv4/conf/tap100/proxy_arp'
#sudo ip link set tap100 master br0
