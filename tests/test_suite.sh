#!/usr/bin/env bash

HOST="127.0.0.1"
PORT="8080"
URL="http://$HOST:$PORT"

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

FAILED=0

run_test() {
    local method="$1"
    local endpoint="$2"
    local expected_code="$3"
    local desc="$4"

    code=$(curl -s -o /dev/null -w "%{http_code}" -X "$method" "$URL$endpoint" --path-as-is)

    if [ "$code" -eq "$expected_code" ]; then
        echo -e "[${GREEN}PASS${NC}] ($code) $desc"
    else
        echo -e "[${RED}FAIL${NC}] Expected $expected_code, Got $code: $desc"
        FAILED=$((FAILED + 1))
    fi
}

echo "=== 1. Primary Tests (static files) ==="
run_test "GET" "/www/index.html" 200 "Request for static file"
run_test "GET" "/www/not.xyz" 404 "Request for not existing file (404)"

echo ""
echo "=== 2. Query String Tests ==="
run_test "GET" "/www/index.html?id=21&name=essa" 200 "Ignoring query string in static files"

echo ""
echo "=== 3. Security Tests (Directory Traversal) ==="
run_test "GET" "/www/../../etc/passwd" 404 "Attempt to access higher hierarchy directory (/..)"
run_test "GET" "/www/./../../etc/shadow" 404 "Complex path with dots"

echo ""
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests ends successful!${NC}"
    exit 0
else
    echo -e "${RED}Errors count: $FAILED${NC}"
    exit 1
fi