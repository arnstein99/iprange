#

# Test content of file1 minus content of file2

if [ $# -ne 2 ]
then
    echo "Usage: subtract_test.sh main_file subtraction_file" >&2
    exit 1
fi

# The result to test.
./ipar_subtract "$2" < "$1" > test_result.txt

diff \
    <(comm -23 \
        <(./ipar_read -test < "$1") \
        <(./ipar_read -test < "$2") \
    ) \
    <(./ipar_read -test < test_result.txt)
