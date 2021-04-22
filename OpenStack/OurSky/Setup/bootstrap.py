import os
import ansible_runner


def run_playbook(playbook_path, inventory_file = None):
    run_result = ansible_runner.run(playbook=os.path.abspath(playbook_path)) #, cmdline="--ask-become-pass")
    return not run_result.errored

def run_role(role, inventory_file = None):
    run_result = ansible_runner.run(roles=roles #, cmdline="--ask-become-pass")
    return not run_result.errored

if __name__ == "__main__":
    print("\n")
    print("=== Welcome to the OurSky Installer!")
    print("Press ENTER to start the installation!")
    input()
    
    print("Making sure that all required system packages are installed.")
    
    # pwd = input("Please enter your password: ")

    # result_pb_host = run_playbook("./Scripts/playbooks/")
    result_pb_host = run_role("./Scripts")
    
    if not result_pb_host:
        print(f"Error executing host bootstrap playbook")
        exit(1)
