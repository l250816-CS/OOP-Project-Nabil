# Tower Defense

An SFML-based tower defense game built as a modular CMake project.

The codebase is split into two libraries:

- `business` handles game rules, level loading, towers, enemies, save data, and wave logic
- `presentation` handles rendering, UI, audio, menus, and in-game screens

## Features

- Multiple tower and enemy types with polymorphic behavior
- Level loading from text files under `data/levels`
- Save and load support for game state
- SFML-based UI, audio, and rendering
- Relative asset loading so the project can run from a build folder on any machine
- Automatic copying of `assets` and `data` into the runtime output directory after build
- Exit Shortcut: Use `Q+A` to exit the game from any screen
- Cheat key Combos:
  - `Ctrl+C` to complete the current wave instantly
  - `Ctrl+W` to win the current level immediately
- Comprehensive logging of game events, score calculations, and page transitions to both console and log files
- Null pointer checks and safe page transitions to prevent crashes on Win/Lose pages
- Score calculation based on time taken and gold remaining, displayed on the Win Page
- A variety of game pages including Main Menu, Level Selection, Game Page with HUD, Pause Menu, Win Page, Lose Page, and About Page



## Build Logic

The top-level CMake file configures the project in a portable way:

- enforces C++20
- builds the `business` and `presentation` libraries first
- links them into the `main` executable
- copies `assets` and `data` beside the executable after build
- keeps all resource paths relative to the executable, which avoids hard-coded machine paths

SFML is resolved in this order:

1. use a local SFML 2.6 installation if available
2. otherwise fetch SFML with CMake `FetchContent`

## Requirements

- CMake 3.16 or newer
- A C++20-capable compiler
- Git, if CMake needs to fetch SFML automatically

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

If you are using a multi-configuration generator such as Visual Studio:

```bash
cmake --build build --config Release
```

If you want a clean rebuild, delete the build directory and configure again:

```bash
cmake -S . -B build-portable -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/Msys/Msys/ucrt64/bin/g++.exe"
cmake --build build-portable
```

## Run

Run the executable from the build output folder that CMake generated.

On MinGW, the executable is typically placed directly in the build directory.

On Visual Studio or other multi-config generators, it may be placed in a configuration subfolder such as `build/Release`.

Because the build copies resources beside the executable, the game can be launched from that output folder without manual file movement.

## Project Layout

- `src/` application entry point
- `business/` core gameplay systems and data handling
- `presentation/` rendering, UI, pages, and audio
- `assets/` textures, fonts, music, sounds, and UI art
- `data/` levels and save-related files
- `build/` local build output

## GitHub Notes

This repository is set up so a new contributor can clone it, configure CMake, and build without editing paths on their machine.

The README reflects the same portable flow used by the build system, so it is safe to follow on Windows or any other supported platform with a compatible compiler.