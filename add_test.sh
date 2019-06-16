#

# Test content of file1 minus content of file2

if [ $# -ne 1 ]
then
    echo "Usage: add_test.sh input_file " >&2
    exit 1
fi

# The result to test.
./ipar_read < "$1" > test_result.txt

diff \
    <(./ipar_expand < "$1" | sort | uniq) \
    <(./ipar_expand < test_result.txt)
