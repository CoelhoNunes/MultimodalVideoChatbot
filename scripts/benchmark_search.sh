#!/bin/bash
# Usage: bash benchmark_search.sh

QUERIES=("Show me the dog" "Muéstrame al perro" "给我看那只狗")
TOTAL_LANG=3
declare -A SUM_LATENCY

for query in "${QUERIES[@]}"; do
  START=$(date +%s.%N)
  RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" -d "{"type":"query","text":"$query"}" http://localhost:5000/query)
  END=$(date +%s.%N)
  ELAPSED=$(echo "$END - $START" | bc)
  SUM_LATENCY["total"]=$(echo "${SUM_LATENCY["total"]} + $ELAPSED" | bc)
done

AVG_LATENCY=$(echo "${SUM_LATENCY["total"]} / $TOTAL_LANG" | bc -l)
echo "Average end-to-end latency (s): $AVG_LATENCY"