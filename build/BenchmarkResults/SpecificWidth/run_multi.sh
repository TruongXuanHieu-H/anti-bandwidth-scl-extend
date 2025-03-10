#!/bin/bash

# Kiểm tra xem số lượng tham số có chia hết cho 3 không
if [ $(($# % 3)) -ne 0 ] || [ $# -eq 0 ]; then
    echo "Usage: $0 <problem_name1> <LB1> <UB1> [<problem_name2> <LB2> <UB2> ...]"
    exit 1
fi

BASE_DIR="/home/truongxuanhieu/anti-bandwidth-scl-extend/build"
RUNLIM="$BASE_DIR/runlim"
ENCODER="$BASE_DIR/abw_enc"
OUTPUT_DIR="$BASE_DIR/BenchmarkResults/SpecificWidth/Symmetry_highest"

# Duyệt qua danh sách các bộ ba đối số
while [ $# -gt 0 ]; do
    PROBLEM_NAME=$1
    LB=$2
    UB=$3
    BENCHMARK_SET="$BASE_DIR/BenchmarkSets/Benchmarks_Standard/${PROBLEM_NAME}.mtx.rnd"

    # Kiểm tra tính hợp lệ của LB và UB
    if [ "$LB" -gt "$UB" ]; then
        echo "Error: LB ($LB) must be less than or equal to UB ($UB) for problem $PROBLEM_NAME"
        shift 3
        continue
    fi

    # Chạy vòng lặp từ LB đến UB
    for ((w=LB; w<=UB; w++)); do
        OUTPUT_FILE="${OUTPUT_DIR}/${PROBLEM_NAME}--ladder_w=${w}.out"
        $RUNLIM -t 1800 -r 1800 -s 30000 -o "$OUTPUT_FILE" \
            $ENCODER "$BENCHMARK_SET" --ladder -symmetry-break h -set-lb $w -set-ub $w
    done
    
    # Chuyển sang bộ tham số tiếp theo
    shift 3
done
