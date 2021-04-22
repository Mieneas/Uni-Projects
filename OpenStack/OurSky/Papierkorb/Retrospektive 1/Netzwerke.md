# Netzwerke

Aus dem Post von Alexander (https://isis.tu-berlin.de/mod/forum/discuss.php?d=341531):<br>
> Wichtig!!: Ihr müsst das zweite Netzwerkinterface der Nodes konfigurieren und aktivieren. Nutzt bitte folgendes IP-Muster: 10.0.42.<Nummer des jeweiligen wally-Nodes>
z.B.: Für wally142 --> 10.0.42.142

OpenStack-Beschreibung der Netzwerk-Interfaces:<br>
https://docs.openstack.org/mitaka/install-guide-ubuntu/environment-networking.html

Wir brauchen also ein Provider und ein Management-Interface

Auf den VMs sind folgende Netzwerke vorhanden:
- **enp2s0**: Standard-Interface. Vorkonfiguriert, mit Internetverbindung. Dieses werden wir als **Provider-Interface** benutzen
- **eno1**: zweites Interface. Eigener NIC, keine Internetverbindung. Dieses werden wir als **Management-Interface** benutzen.

Erst habe ich versucht, `eno1` über `/etc/network/interfaces` zu konfigurieren ([offizielle OpenStack-Doku](https://docs.openstack.org/install-guide/environment-networking-controller.html)), aber das hatte keinen Effekt.<br>
Ubuntu verwendet `netplan` ([ein paar Beispiele](https://netplan.io/examples/)), um die Interfaces beim Boot zu konfigurieren, wodurch `/etc/network/interfaces` ignoriert wird ([offizielle Ubuntu-Doku](https://ubuntu.com/server/docs/network-configuration)).<br>
Beispielhafte Anleitung: https://www.itzgeek.com/how-tos/linux/ubuntu-how-tos/netplan-how-to-configure-static-ip-address-in-ubuntu-18-04-using-netplan.html

Auf **`wallyXXX`** habe ich folgende Konfiguration verwendet:
```
# This file describes the network interfaces available on your system
# For more information, see netplan(5).
network:
  version: 2
  renderer: networkd
  ethernets:
    enp2s0:
      dhcp4: yes
    eno1:
      addresses: [10.0.42.XXX/24]
      dhcp4: no
```

Das funktioniert soweit. Allerdings besteht noch keine Internetverbindung von `eno1` aus. Ich denke, das ist ok, da wir das Interface nur zum Management der anderen Nodes brauchen. Bin mir aber nicht sicher.<br>

![](https://docs.openstack.org/mitaka/install-guide-ubuntu/_images/networklayout.png)
