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

```bash
./loader test.png
open out.ppm
```

