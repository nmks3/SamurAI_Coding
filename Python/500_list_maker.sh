#!/bin/bash

find ../002_gazou_face/ -name '*.jpg' | sed -e 's|/hinnyuu_\(.*\)|/hinnyuu_\1 0|g' -e 's|/kyonyuu_\(.*\)|/kyonyuu_\1 1|g'

echo
echo 'you must make face_filelist_train.txt and face_filelist_test.txt from above list'
echo


