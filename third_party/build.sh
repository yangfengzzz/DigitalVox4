#!/bin/bash

cd physx/physx
./generate_projects.sh linux

cd compiler/linux-debug
make -j5
cd ..

cd linux-release
make -j5
cd ..
