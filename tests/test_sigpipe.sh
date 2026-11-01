#!/usr/bin/env bash

SERVER_PID=$(pgrep -f "./bin/http_server")

if [ -z "$SERVER_PID" ]; then
    echo "Working server proccess not found!"
    exit 1
fi

echo "Send cut request..."

printf "GET /www/index.html HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc -N 127.0.0.1 8080 > /dev/null 2>&1 &

sleep 0.5

if kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "[PASS] Serwer survive (resistance to SIGPIPE)."
else
    echo "[FAIL] Server got killed by SIGPIPE or error in write()!"
fi