#

# Test reading in a file

if [ $# -ne 1 ]
then
    echo "Usage: read_test.sh input_file " >&2
    exit 1
fi

# The result to test.
./ipar_read < "$1" > test_result.txt

diff \
    <(./ipar_expand -hex < "$1" | sort | uniq) \
    <(./ipar_expand -hex < test_result.txt)
