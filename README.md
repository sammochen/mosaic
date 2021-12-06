# mosaic

Create mosaics from images :)

## Usage

Run:
```
./mosaic input.jpg output.jpg numSpaces numColors
```

`numSpaces` is the number of mosaic pieces. Higher = fine, lower = coarse.  
`numColors` is the number of colors. Higher = precise, lower = abstract.

## Build

### Requirements

The repository uses the STB library for image reading/writing: [https://github.com/nothings/stb](https://github.com/nothings/stb)  

Clone the repository locally and update `CMakeLists.txt` so that `STB_ROOT` points to the `stb` folder.

### Compilation

Follow the steps below:

```
mkdir build
cd build
cmake ..
make
```

The executable `mosaic` will be in the `build` folder.

