kibana.yml muss in Deployment host (lokal) in /etc/kolla/ hibzugefügt werden. (Also neben globals.yml und passwords.yml)

globals.yml in Deployment host (lokal) in /etc/kolla/

multinode kann in einem gewünschten Ort gelegt werden, wichtig aber später auf die Datei zu linken. 

File main.yml muss in vm/share/kolla-ansible/ansible/roles/kibana/defaults/ sein.

File config.yml in in vm/share/kolla-ansible/ansible/roles/kibana/tasks/ sein.

File deploy.yml in in vm/share/kolla-ansible/ansible/roles/kibana/tasks/ sein.

Wenn OPenStack da ist, dann kann man den Dashboard nutzen.
Um das zu erreichen führe "kolla-ansible -i multinode post-deploy" aus. Dann bekommt man eine Datei in /etc/kola/, die "admin-openrc.sh" heißt. Diese muss man erst ausführen "bash /etc/kolla/admin-openrc.sh" und dann mit "cat" kann man der Nutzername und password sowie IP-Addresse ansehen und diese benutzen, um in OpenStack dashboard (Horizon) als Admin einzuloggen.