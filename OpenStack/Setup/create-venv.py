import os
import sys
import shutil

from venv import EnvBuilder

VENV_PATH = os.path.abspath("./venv")
VENV_PYTHON = "3.9.1"

if __name__ == "__main__":    
    if len(sys.argv[1]) > 1:
        VENV_PATH = sys.argv[1]
    
    VENV_PATH = os.path.abspath(VENV_PATH)
    
    if os.path.isdir(VENV_PATH) and False:
        print(f"Removing virtual environment at {VENV_PATH}")
        shutil.rmtree(VENV_PATH)
        
    print(f"Installing virtual environment to {VENV_PATH}")
    eb = EnvBuilder(with_pip=True)
    eb.create(VENV_PATH)
    print(f"Virtual environment successfully created at {VENV_PATH}")
