export PICO_DVI_HOME=${HOME}/git/PicoDVI/software
export GDVI_HOME=${HOME}/git/GDvi

clear
rm -rf build
mkdir -p build
cd build
cmake .. \
# cmake .. \
#     -DDVI_VERTICAL_REPEAT=1\
#     -DDVI_N_TMDS_BUFFERS=4\
#     -DDVI_SYMBOLS_PER_WORD=2\
    -DPICO_CORE1_STACK_SIZE=0x400\
    -DPICO_PLATFORM=rp2040\
    -DPICO_COPY_TO_RAM=1\
#     -DDVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg

make -j4
