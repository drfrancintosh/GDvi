export PICO_DVI_HOME=${HOME}/git/PicoDVI

clear
rm -rf build
mkdir -p build
cd build
cmake ..
make