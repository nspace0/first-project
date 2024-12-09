PATTERN="pattern"
TEST_FILE1="./files/file1.txt"
TEST_FILE2="./files/file2.txt"

for flag in -e -i -v -c -l -n -s -o -h -f; do
    echo "Testing flag $flag with Valgrind"
    if [ "$flag" == "-f" ]; then
        valgrind --leak-check=full ./s21_grep $flag $TEST_FILE1 $TEST_FILE2 -s
    else
        valgrind --leak-check=full ./s21_grep $flag "$PATTERN" $TEST_FILE1 -s
    fi
done
