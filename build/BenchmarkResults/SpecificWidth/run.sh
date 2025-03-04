#!/bin/bash

# Kiểm tra xem có đủ 3 tham số không
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <problem_name> <LB> <UB>"
    exit 1
fi

PROBLEM_NAME=$1
LB=$2
UB=$3

BASE_DIR="/home/truongxuanhieu/anti-bandwidth-scl-extend/build"
RUNLIM="$BASE_DIR/runlim"
ENCODER="$BASE_DIR/abw_enc"
BENCHMARK_SET="$BASE_DIR/BenchmarkSets/Benchmarks_Standard/${PROBLEM_NAME}.mtx.rnd"
OUTPUT_DIR="$BASE_DIR/BenchmarkResults/SpecificWidth/Symmetry_highest"

# Kiểm tra xem LB và UB có hợp lệ không
if [ "$LB" -gt "$UB" ]; then
    echo "Error: LB ($LB) must be less than or equal to UB ($UB)"
    exit 1
fi

# Chạy từ LB đến UB
for ((w=LB; w<=UB; w++)); do
    OUTPUT_FILE="${OUTPUT_DIR}/${PROBLEM_NAME}--ladder_w=${w}.out"
    
    $RUNLIM -t 1800 -r 1800 -s 30000 -o "$OUTPUT_FILE" \
        $ENCODER "$BENCHMARK_SET" --ladder -symmetry-break h -set-lb $w -set-ub $w
done
