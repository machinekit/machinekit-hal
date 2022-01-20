PYTHON_INTERPRETER="$(which python3)"
echo "Using $PYTHON_INTERPRETER as the Python3 interpreter."
${PYTHON_INTERPRETER} ./test.py
exit "$?"
