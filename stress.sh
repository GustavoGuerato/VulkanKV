#!/bin/bash

HOST=127.0.0.1
PORT=8080
CLIENTS=50

stress_client() {
  i=$1

  (
    for j in $(seq 1 200); do
      printf "SET k%d_%d v%d\n" "$i" "$j" "$j"
      printf "GET k%d_%d\n" "$i" "$j"
    done
  ) | nc -q 1 $HOST $PORT > /dev/null 2>&1
}

for i in $(seq 1 $CLIENTS)
do
  stress_client "$i" &
done

wait

echo "REAL stress done"