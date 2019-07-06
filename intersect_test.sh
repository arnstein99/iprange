#

# Test content of file1 intersected with content of file2

if [ $# -ne 2 ]
then
    echo "Usage: intersect_test.sh main_file intersect_file" >&2
    exit 1
fi

# The result to test.
./ipar_intersect "$2" < "$1" > test_result.txt

diff \
    <(comm -12 \
        <(./ipar_read -hex < "$1") \
        <(./ipar_read -hex < "$2") \
    ) \
    <(./ipar_expand -hex < test_result.txt)
