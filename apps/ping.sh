#bin/bash

#Phani's SERVER BOX 
#TARGET_MAC=18:03:73:17:07:d5

#Xen TEST BOX (@Robbie's Desk)
TARGET_MAC=18:03:73:1f:9c:3a

#Test box (Jiguo's PC)
#TARGET_MAC=18:03:73:1f:9d:d2

#TEST BOX @ Home
#TARGET_MAC=78:2B:CB:92:16:80

#Pablo's BOX (@ luke's desk)
#TARGET_MAC=78:2B:CB:99:3A:EA

HOST_IP=192.168.0.1
TARGET_IP=192.168.0.2
NETMASK=255.255.255.0
ARP_ROUTE1=111.111.111.0
ARP_ROUTE2=111.111.111.1
ARP_ROUTE3=222.222.222.0
ARP_ROUTE4=222.222.222.1
HOST_IF=eth0

echo "Host Interface:$HOST_IF, Target MAC:$TARGET_MAC"
sudo ifconfig $HOST_IF $HOST_IP up
sudo route add -net $ARP_ROUTE1 netmask $NETMASK $HOST_IF
sudo route add -net $ARP_ROUTE3 netmask $NETMASK $HOST_IF

sudo arp -s $ARP_ROUTE1 $TARGET_MAC
sudo arp -s $ARP_ROUTE2 $TARGET_MAC
sudo arp -s $ARP_ROUTE3 $TARGET_MAC
sudo arp -s $ARP_ROUTE4 $TARGET_MAC
sudo arp -s $TARGET_IP $TARGET_MAC
echo "(DOUBLE CHECK HOST HOST_IF AND TARGET MAC)"

