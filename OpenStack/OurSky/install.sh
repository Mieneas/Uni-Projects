#!/bin/bash
# Mit diesem Skript soll alles installiert werden können.

# Global configuration
py_venv_path=./venv

# First, check if python is setup correctly
py_version=$(python3 --version)
pyexec=$(which python3)

openstack_final_url="http://wally135.cit.tu-berlin.de/"

if [[ $py_version == "Python 3."* ]] ;
then
    echo "Python version: $py_version"
else
    echo "Python version not supported: $py_version"
    exit 1
fi

echo "Installing necessary packages"
apt-get install -y python3-venv python3-dev libffi-dev gcc libssl-dev

echo "Using python from $pyexec"

$pyexec -c 'import venv'
if [[ ! $? -eq 0 ]] ;
then
    echo "Python module venv is missing. Please install it using pip:\n  pip install venv"
    exit 1
fi

echo "Creating virtual environment ..."
$pyexec -m venv $py_venv_path

echo "Activating venv at $py_venv_path"
source $py_venv_path/bin/activate

pyexec=$(which python3)
echo "Now using Python at $pyexec"

echo "Updating pip to the latest version"
pip install -U pip
echo "Installing required packages..."
pip install -r requirements.txt

kolla_ansible_kibana_tasks_path=$py_venv_path/share/kolla-ansible/ansible/roles/kibana/tasks/
kolla_ansible_kibana_default_path=$py_venv_path/share/kolla-ansible/ansible/roles/kibana/default/

echo "Patching kolla-ansible kibana installation at $kolla_ansible_kibana_tasks_path"
cp -v -r ./Setup/kibana-patches/tasks/* $kolla_ansible_kibana_tasks_path

echo "Patching kolla-ansible kibana installation at $kolla_ansible_kibana_default_path"
cp -v -r ./Setup/kibana-patches/default/* $kolla_ansible_kibana_default_path

echo "=== Welcome to the OurSky Installer!"
read -p "Press ENTER to start the installation! " enterkey

# HOST BOOTSTRAP
echo "Bootstrapping your host machine. Ansible will first ask for your sudo (BECOME) password."
ansible-playbook -i Setup/inventory/multinode Setup/bootstrap_host.yml -K

# DEPLOY PRE-BOOTSTRAP
echo "Pre-Bootstrapping your OpenStack nodes ..."
ansible-playbook -i Setup/inventory/multinode Setup/bootstrap_nodes.yml

read -p "Press ENTER to start the kolla-ansible deployment! " enterkey
# DEPLOY BOOTSTRAP-SERVERS
echo "KOLLA DEPLOYMENT 1/4: bootstrap-servers"
kolla-ansible -i Setup/inventory/multinode bootstrap-servers

echo "KOLLA DEPLOYMENT 2/4: prechecks"
kolla-ansible -i Setup/inventory/multinode prechecks

echo "KOLLA DEPLOYMENT 3/4: deploy"
kolla-ansible -i Setup/inventory/multinode deploy

echo "KOLLA DEPLOYMENT 4/4: deploy"
kolla-ansible -i Setup/inventory/multinode post-deploy

source /etc/kolla/admin-openrc.sh

echo "Deployment successful!"
echo
echo "   OpenStack URL: $openstack_final_url"
echo "   OpenStack username: $OS_USERNAME"
echo "   OpenStack password: $OS_PASSWORD"

# TODO
# python Scripts/bootstrap.py
