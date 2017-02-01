#!/bin/bash

time python ./501_model_make__parts.py | perl -pe '$|=1;@x=localtime();$x=sprintf("[%02d/%02d/%02d %02d:%02d:%02d] ",$x[5]+1900,$x[4]+1,$x[3],$x[2],$x[1],$x[0]); s/^/$x/;' | tee log_model_make_`date +%Y%m%d_%H%M%S`.txt




