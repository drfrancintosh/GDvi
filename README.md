# GDvi - A Simplified Wrapper around Luke Wren's PicoDVI

- Author: Gregory Smith

- See: (PicoDVI)[https://github.com/Wren6991/PicoDVI]
- For: Raspberry Pi Pico RP2040

# Introduction

## Luke Wren's PicoDVI
Luke Wren's PicoDVI is a powerful utility that "bit bangs" the PIO output from the Raspberry Pi Pico to generate DVI output compatible with most HDMI displays.

The demos are well-known, but few projects have picked up where Wren left off. 

The details of Wren's work are highly technical and exposes what is nothing short of genius level work on Wren's part.

However, it hasn't been easy to extend his work beyond the initial offering because a clear understanding of TDMS signals is necessary to leverage PicoDVI to its fullest. The best demos require "ping-ponging" between CORE-0 and CORE-1. Wren's code is a master course in multi-processing. In many cases, CORE-1 bangs the PIO while CORE-0 prepares each scanline output. This takes up nearly 100% of CORE-1 and a sizeable portion of CORE-0. Demos where full-color images are displays expertly read from the Flash and use Direct Memory Access (or DMA). CORE-1 displays the current scan line while CORE-0 is preparing the next one. It's remarkable code.

## The Inspiration for GDvi - a wrapper around PicoDVI

- A YouTube video got me to thinking

After watching a YouTube on displaying graphics on a TFT display without a frame buffer on the RPI Pico (https://www.youtube.com/watch?v=WUGTSiKEeMA). It occurred to me that Jay Miner created Display List Graphics for the Atari ANTIC chip. It uses sprites to display objects on the screen without (necessarily) using a frame buffer.

I decided to attempt a Display List solution for PicoDVI that could display rectangles from a list and render each row of the display in real time - without backing memory.

However, as I dived into Wren's code, it became immediately apparent that the code was very technical. It might be possible to create a display system using one of the demos (the SPRITE demo looked promising). But these demos used both cores to achieve their goals. And I wanted a single-core solution.

- Reducing Expectations

Having determined that my Display List solution was too complicated, I decided to create a simplified interface with the goal of achieving the highest resolution graphics possible on the Pico. My goal was 1920 x 1080p and 24-bit color.

The more I learned about PicoDVI's design, the more it became clear that a simple, single-core solution would not be viable. So, I decided to lower my expectations and shoot for a bit-mapped approach (using a frame buffer) that at least ran on a single core.

This led me to the `dht_logging` demo in Wren's github repo. It utilized 3 bitplanes and a custom TDMS pipeline. Reverse-engineering this (and the `hello_dvi` and `colour_terminal` apps) gave me the education I needed.

- 3-bit colors

One limitation of the frame-buffered approach appears to be 3-bits of display. While other demos achieved higher color resolutions, they all used both cores to alternately prepare a scanline and display it.

I felt that 8 colors might be a reasonable alternative if a palette of 16- or 24-bit color was possible. While the `vista_palette` example offered a large palette, the TDMS palette code was too advanced for my skillset. While I was able to "grok" the code that use the TDMS encoders, getting deeper into TDMS was not going to be a possibility.

- The Result

After a week of part-time work, I settled on 640x480 pixels and 1, 2, or 3-bit color. The one requirement I insisted upon was a palette of color. Wren's implementation of 1-bit-per-pixel TDMS made it difficult to choose any two colors for 1-bit solution, and the only colors available are 3-bits (8 primary colors). And doing this for 2-bits was even harder.

Another challenge is the TDMS encoding happens in real time. So you have only nanoseconds to generate 3 channels of 640 pixels. This restriction required some very tight coding to create a palette for 1- and 2-bit solutions.

The ensuing logic required me to revisit my college study of Boolean logic gates and Karnaugh maps (K-maps). I ended up creating some very tight boolean logic to handle the 2- and 4-color pallette solution. The same solution could not be used for the 8-color palette as the number of combinations soared from 16 combinations to 256 for the 8-colors. 

## Conclusions and Future Work

I have delivered a very simple wrapper around Wren's PicDVI that gives a lot of functionality without sacrificing a lot of memory, CPU cycles, or DVI quality. I think most people who desire HDMI/DVI capabilities from the Pico will find GDvi very easy to install and use.

Future projects may expand GDvi to take advantage of Wren's cooperative core processing to get more colors on screen. I also want to integrate GDvi into my GOOP and GAPP framework for the Pico. This will make adding HDMI/DVI graphics to any project a breeze.

## Design Goals

- Create a simplified API for PicoDVI
    - single call to set up the Pico for HDMI/DVI
- CORE-1 background processing
    - run DVI on CORE-1 without interfering with CORE-0
    - no special multi-processing code for CORE-0
    - no need for "swapping" between cores
- Multiple Graphics Modes
    - 640 x 480 pixels and multiple bit-width pixels
    - Options for pixel doubling
        - for 320 horizontal pixels
        - and 240 vertical pixels
    - Resulting Options:
        - 640 or 320 pixels horizontally
        - Variable vertical pixel with automatic pixel doubling and header/footer centering
            - (eg: 200, 240, 360, 480, and all values in between)
        - 1, 2, or 3 bit planes (2, 4, or 8 colors from a palette of 8 primary colors)
- Color Palette
    - Desired:
        - Palette of 16-bit or 24-bit colors from a palette of 2, 4, 8, 16, or 256 colors
    - Achieved:
    -   Palette of 2, 4, or 8 colors of 3-bits each (8 colors)
- Text Modes
    - Desired:
        - 40 or 80 column modes
        - 25 or more rows
    - Achieved:
        - None

## API Interface

- `#include "gdvi.h"`
- `GDvi *gdvi_init(uint16_t width, uint16_t height, uint8_t bits, void *context)`
    - initialize the RP2040 for PicoDVI
    - returns: `GDvi *` pointer to control structure
    - `width`: 320 or 640 pixels
    - `height`: variable from 1-480 (GDvi will automatically center vertically and double pixels)
    - `bits`: 1, 2, or 3 bits (for 2, 4, or 8 colors)
    - `context`: pointer to user data

- `gdvi_setPalette(GDvi* gdvi, uint8_t *palette)`
    - Sets the palette
    - `gdvi`: GDvi pointer returned by gdvi_init()
    - `palette`: a pointer to an array of 2, 4 or 8 RGB 3-bit colors
    - NOTE: The Border Colors are reset to the Background Color (Palette[0]) upon a call to `gdvi_setPalette()`

- `gdvi_setBorderColors(uint8_t top, uint8_t bottom, uint8_t left, uint8_t right)`
    - Sets the border colors of the screen when the user selects a `height` other than 240 or 480
    - `top`: RGB 3-bit color for the top of the screen when `height` is not 240 or 480 pixels
    - `bottom`: RGB 3-bit color for the bottom of the screen when `height` is not 240 or 480 pixels
    - `left`: RGB 3-bit color for the left of the screen when `width` is not 320 or 640 pixels (`unimplemented`)
    - `right`: RGB 3-bit color for the right of the screen when `width` is not 320 or 640 pixels (`unimplemented`)
    - NOTE: The Border Colors are reset to the Background Color (Palette[0]) upon a call to `gdvi_setPalette()`

- `gdvi_start()`
    - Kicks off PicoDVI on CORE-1 and returns immediately

- `gdvi_stop(GDvi *gdvi)`
    - Stops PicoDVI on CORE-1
    - NOTE: It is not possible to restart PicoDVI once stopped

- `gdvi_destroy(GDvi *gdvi)`
    - Frees the bitmaps

## Building the Apps

- The root `cmake` file is designed to build the libraries only (`./libs/`). The example apps have their own `CMakeLists.txt` that will build the apps with `PicoDVI` and `GDvi` as external libraries.
- Install `PicoDVI` from Wren's GitHub: `git clone https://github.com/devcybiko/PicoDVI.git`
- Install `GDvi` from my GitHub: `git clone
## Future Work

- Add a way to synchronize CORE-0 with CORE-1 so that the display is not updated during a raster scan (causing fractured displays)