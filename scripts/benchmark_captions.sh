#!/bin/bash
# Usage: bash benchmark_captions.sh <video_directory>

VIDEO_DIR=$1
if [ -z "$VIDEO_DIR" ]; then
  echo "Usage: $0 <video_directory>"
  exit 1
fi

# GPU mode
START=$(date +%s.%N)
./captioner --videos "$VIDEO_DIR" --output out_gpu.json --use_gpu
END=$(date +%s.%N)
ELAPSED_GPU=$(echo "$END - $START" | bc)

# CPU mode
START=$(date +%s.%N)
./captioner --videos "$VIDEO_DIR" --output out_cpu.json --use_gpu=false
END=$(date +%s.%N)
ELAPSED_CPU=$(echo "$END - $START" | bc)

# Calculate total frames
TOTAL_FRAMES=$(ffprobe -v error -select_streams v:0 -count_frames -show_entries stream=nb_read_frames -print_format csv=p=0 "$VIDEO_DIR"/*.mp4 | paste -sd+ - | bc)

AVG_FPS_GPU=$(echo "$TOTAL_FRAMES / $ELAPSED_GPU" | bc)
AVG_FPS_CPU=$(echo "$TOTAL_FRAMES / $ELAPSED_CPU" | bc)

echo "| Mode | Total Time (s) | Avg FPS |"
echo "|------|---------------:|--------:|"
printf "| GPU  |       %.3f  |   %d |
" "$ELAPSED_GPU" "$AVG_FPS_GPU"
printf "| CPU  |       %.3f  |   %d |
" "$ELAPSED_CPU" "$AVG_FPS_CPU"