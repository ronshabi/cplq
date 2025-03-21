# IMPORTANT: use `make test` to invoke this. Do NOT invoke directly.
# Test the error checking capabilities of the compiler

clear
TESTS_DIR="tests/error"

TESTS=$(ls $TESTS_DIR)

i=0
for t in $TESTS; do
    echo -e "\033[35m$t\033[0m";
    
    ./cplq $TESTS_DIR/$t

    i=$((i+1))
    echo
done;