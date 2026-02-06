#!/bin/bash

TIME_LIMIT=1800
MEMORY_LIMIT=30000
SYMMETRY_BREAK="f"   # First node
WORKER_COUNT=1
ENC_TYPE="--seq"
ENC_DIR="../../../build"
DATASET_DIR="../../Benchmarks"
LOG_DIR="../../results/symmetry_first/seq"
ADD_CONFIGS="--from-lb"

mkdir -p "$LOG_DIR"

$ENC_DIR/abw_enc $DATASET_DIR/A-pores_1.mtx.rnd     $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/A-pores_1.log
$ENC_DIR/abw_enc $DATASET_DIR/B-ibm32.mtx.rnd       $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/B-ibm32.log
$ENC_DIR/abw_enc $DATASET_DIR/C-bcspwr01.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/C-bcspwr01.log
$ENC_DIR/abw_enc $DATASET_DIR/D-bcsstk01.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/D-bcsstk01.log
$ENC_DIR/abw_enc $DATASET_DIR/E-bcspwr02.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/E-bcspwr02.log
$ENC_DIR/abw_enc $DATASET_DIR/F-curtis54.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/F-curtis54.log
$ENC_DIR/abw_enc $DATASET_DIR/G-will57.mtx.rnd      $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/G-will57.log
$ENC_DIR/abw_enc $DATASET_DIR/H-impcol_b.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/H-impcol_b.log
$ENC_DIR/abw_enc $DATASET_DIR/I-ash85.mtx.rnd       $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/I-ash85.log
$ENC_DIR/abw_enc $DATASET_DIR/J-nos4.mtx.rnd        $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/J-nos4.log
$ENC_DIR/abw_enc $DATASET_DIR/K-dwt__234.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/K-dwt__234.log
$ENC_DIR/abw_enc $DATASET_DIR/L-bcspwr03.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/L-bcspwr03.log
$ENC_DIR/abw_enc $DATASET_DIR/M-bcsstk06.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/M-bcsstk06.log
$ENC_DIR/abw_enc $DATASET_DIR/N-bcsstk07.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/N-bcsstk07.log
$ENC_DIR/abw_enc $DATASET_DIR/O-impcol_d.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/O-impcol_d.log
$ENC_DIR/abw_enc $DATASET_DIR/P-can__445.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/P-can__445.log
$ENC_DIR/abw_enc $DATASET_DIR/Q-494_bus.mtx.rnd     $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/Q-494_bus.log
$ENC_DIR/abw_enc $DATASET_DIR/R-dwt__503.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/R-dwt__503.log
$ENC_DIR/abw_enc $DATASET_DIR/S-sherman4.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/S-sherman4.log
$ENC_DIR/abw_enc $DATASET_DIR/T-dwt__592.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/T-dwt__592.log
$ENC_DIR/abw_enc $DATASET_DIR/U-662_bus.mtx.rnd     $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/U-662_bus.log
$ENC_DIR/abw_enc $DATASET_DIR/V-nos6.mtx.rnd        $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/V-nos6.log
$ENC_DIR/abw_enc $DATASET_DIR/W-685_bus.mtx.rnd     $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/W-685_bus.log
$ENC_DIR/abw_enc $DATASET_DIR/X-can__715.mtx.rnd    $ENC_TYPE -limit-real-time $TIME_LIMIT -limit-memory $MEMORY_LIMIT -symmetry-break $SYMMETRY_BREAK -process-count $WORKER_COUNT $ADD_CONFIGS 2>&1 | tee $LOG_DIR/X-can__715.log
