# Evaluation Procedures

## 1. Caption Benchmark

```bash
cd scripts
bash benchmark_captions.sh ../data/test_videos
```
- Outputs total time and avg FPS for GPU vs. CPU.

## 2. Caption Quality

```bash
python3 evaluate_captions.py   --gen results/generated_captions.json   --ref data/ground_truth_captions.json
```
- Prints BLEU-1, BLEU-4, and CIDEr scores.

## 3. Search/Chat Latency

```bash
cd scripts
bash benchmark_search.sh
```
- Outputs average latencies breakdown.

## 4. Multilingual Correctness

```bash
python3 validate_multilingual.py --config data/multilingual_tests.json
```
- Reports pass/fail for each query in English, Spanish, Mandarin.