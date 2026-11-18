**English** | [Español](README.es.md) | [Português](README.pt.md)

# ShuraFechditor

A terminal color profile editor for [fastfetch](https://github.com/fastfetch-cli/fastfetch).

It lets you choose a PNG image and one color for each of the 4 fastfetch module groups, preview the result in truecolor, and save the profile to `colores.conf`.

> **Status:** working. The interface, palette loading, preview, and profile saving now work. Tests are still pending.

## Features

- [x] Interactive terminal flow
- [x] Palette loaded from `color.options.colors`
- [x] Truecolor preview (ANSI 24-bit)
- [x] Input validation
- [x] Save profiles to `colores.conf`
- [x] Copy/rename the image to `logos/`
- [x] Consistency check between profiles and images
- [x] Real PNG validation (file signature)
- [ ] Tests
- [ ] Install script

## Requirements

- Linux (tested on CachyOS/Arch)
- C++17 compiler (`g++` or `clang++`)
- CMake >= 3.16
- Terminal with truecolor support
- `fastfetch` (to use the generated profiles)

## Build

```bash
git clone https://github.com/Sura578/ShuraFechditor.git
cd ShuraFechditor/code

cmake -S . -B build
cmake --build build
```

The executable is placed at `code/build/shurafechditor`.

CMake copies `color.options.colors`, `colores.conf`, and `config.jsonc` into the `build/` folder so you can run it from there.

## Usage

```bash
cd code
./build/shurafechditor
```

Flow:

1. Enter the path to a `.png` image.
2. Choose a color (0-8) for each group:
   - **Group 1:** OS, kernel, packages, shell
   - **Group 2:** WM, lm, icons, terminal
   - **Group 3:** host, CPU, GPU, disk, memory, uptime, display
   - **Group 4:** sound, player, media
3. Check the preview.
4. Confirm with `y` or cancel with `n`.

> ℹ️ Step 4 writes `colores.conf`, copies the PNG into `logos/<n>.png`, and keeps a backup while writing. If the process is interrupted, the backup is restored on the next run.

## File formats

### `color.options.colors`

Available palette. Each line uses the format `index NAME #RRGGBB`.

```
0 RED #F4A6A6
1 ORANGE #F8C49A
2 YELLOW #F9E7A5
3 GREEN #B9DDBB
4 BLUE #A9D5E8
5 INDIGO #B5B8E8
6 VIOLET #D8B5E5
7 WHITE #FFFFFF
8 BLACK #333333
```

### `colores.conf`

One profile per line: image number + 4 hexadecimal colors (one per group).

```
# image | group1 | group2 | group3 | group4
1  #8dbfca  #f9a2c5  #eec2d6  #96cdd9
```

`config.jsonc` uses `%%C1%%`, `%%C2%%`, `%%C3%%`, and `%%C4%%` to assign each module to its color group.

## Project structure

```
code/
├── CMakeLists.txt
├── main.cpp
├── ui.h / ui.cpp              # interface and flow
├── palette.h / palette.cpp    # palette loading from color.options.colors
├── profilemaker.h / .cpp      # profile creation, validation and recovery
├── color.options.colors       # palette
├── colores.conf               # profiles
└── config.jsonc               # fastfetch configuration
```

## Roadmap

- [x] Terminal interface
- [x] Palette loading and preview
- [x] `profilemaker()`: write `colores.conf` and copy the image
- [x] `profilechecker()`: detect missing profiles and images
- [x] `imagechecker()`: real PNG signature validation
- [x] Error handling, backup and recovery
- [ ] Tests
- [ ] Installation documentation

## Contributing

Issues and pull requests are welcome. If you find a bug, open an issue with the steps to reproduce it.

## License

This project does not have a defined license yet.

## Author

- **Sura578** ([@Sura578](https://github.com/Sura578))
