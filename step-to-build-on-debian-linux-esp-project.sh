#!/bin/bash

esp_project_path=`pwd`

#1.install basic package
sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python3 python3-pip python3-setuptools python3-serial python3-cryptography python3-future python3-pyparsing

#2.get the esp toolchain and extract in folder
mkdir $HOME/esp-toolchain
cd $HOME/esp-toolchain
wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf ./xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
export PATH="$HOME/esp-toolchain/xtensa-esp32-elf/bin:$PATH"

#3.get esp-idf
cd 
git clone -b v3.1 --recursive https://github.com/espressif/esp-idf.git

#4.export the path
export IDF_PATH=$HOME/esp-idf
. $IDF_PATH/add_path.sh

#5.setup is ready now you can build any esp project
cd $esp_project_path
make -j$(nproc)

