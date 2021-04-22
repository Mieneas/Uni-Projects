#/bin/sh
py_version=$(python --version) || $(python3 --version)
py_path=$(which python) || $(which python3)

if [[ $py_version == "Python 3."* ]] ;
then
    echo "Python version correct: $py_version"
else
    echo "Python version not correct."
    exit 1
fi

echo "Using python from $py_path"

export py_path=$py_path

# if [[]]
