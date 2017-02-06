#!/bin/bash

python ./502_hanbetu__parts.py "$@" | tee ../traindata/first/learnlog0/2/log_hanbetu_`date +%Y%m%d_%H%M%S`.txt

