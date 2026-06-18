# Image Loader

This is an image loader that I created for my future game engine. I wanted to try to make it for png only, but jpeg is coming soon.

## Compilation and Usage

For compilation I use clang, because it's easy and quite comfortable to use. And for the file to work, I am using zlib for the deflate algorithm. 

### Zlib installation

##### MacOS

```bash
brew install zlib
```
##### Linux

These are the main distros

- Ubuntu/Debian
```bash
sudo apt install zlib
```
- Fedora
```bash
sudo dnf install zlib-devel
```
- Arch
```bash
sudo pacman -S zlib
```
- Tarball 

Official Tarball that I have found on the [zlib's website](https://www.zlib.net/) is by the following link [https://www.zlib.net/current/zlib.tar.gz]

##### Windows

- I have no means to test (for now), so I would not recommend following my advice, however there are chocolatey package that follows that, or you can simply use WSL:

```bash
choco install zlib
```

### Compilation
After installing zlib, simply use clang to compile: 
```bash
clang -lz loader.c -o loader
```
### Usage
##### Library
Simply use it to process needed png or jpeg (coming soon) file like so:
```cpp
int width, height, nrChannels;
uint8_t *tex_pixels = process_png("./assets/test.png", &width, &height, &nrChannels);
```
Which will output the raw pixels to use for graphical output.

##### Direct testing
Although this is meant like a library, you can test it by calling it like so:
```bash
./loader test.png
open out.ppm
```
PPM is the easiest file output possible, without any compression, so it shows the raw bytes perfectly.

## Contact

I am available on [LinkedIn](https://www.linkedin.com/in/nazar-kaniuka-326437231/) or by [Email](mailto:nazarkaniuka6@gmail.com)
