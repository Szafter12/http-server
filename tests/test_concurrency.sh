#!/usr/bin/env bash

HOST="127.0.0.1"
PORT="8080"
TOTAL_REQUESTS=50

echo "Start $TOTAL_REQUESTS parrarel requests..."
START_TIME=$(date +%s%N)

for i in $(seq 1 $TOTAL_REQUESTS); do
    (
        res=$(curl -s -o /dev/null -w "%{http_code}" "http://$HOST:$PORT/www/index.html?req=$i")
        if [ "$res" -ne 200 ]; then
            echo "Request #$i failed (code: $res)"
        fi
    ) &
done

wait

END_TIME=$(date +%s%N)
ELAPSED_MS=$(( (END_TIME - START_TIME) / 1000000 ))

echo "All request finish in ${ELAPSED_MS} ms."