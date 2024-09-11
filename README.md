# RLUT

> [!WARNING]
> Work in progress!

**R**ogue-**L**ike **U**tilities **T**oolkit is a minimal + lightweight framework with most of the basic necessities to make a rogue-like game (probably).

The name and API are based off GLUT. The idea and design of RLUT is a mixture of GLUT and NCurses. RLUT doesn't handle any sort of game logic or map drawing; it just handling window + context creation, and rendering an NCurses-like screen buffer.

RLUT is designed to be built seemlessly as either a classic terminal process, or as a GUI application (SDL2) without having to change anything. ImGui is integrated into RLUT and works in both TUI and GUI builds (thanks to [ggerganov/imtui](https://github.com/ggerganov/imtui)).

RLUT should be very easy to integrate into any other language. The source is C++ but the header is written to be compatible with C; so building RLUT as a dynamic library then linking to your C project (or any other lanuage via ffi) should be relatively pain free.

See the [sample](#sample) or [build](#build) sections for more info, or see [here](/aux/test.c) for an example project.

## Sample

```TODO```

## Features

- [X] Simple API to read, use and bind to another language
- [X] Window creation + event handling
    - [X] NCurses (TUI)
    - [ ] SDL2 (GUI) ```TODO```
- [X] NCurses-like cursor + print functions
- [X] ANSI escape parser (90% coverage)
- [X] Map generation functions
    - [X] Cellular automata (caves)
    - [X] Perlin + FBM (islands or overword)
    - [ ] More map generators ...
- [X] Random number generation functions
- [ ] A\* pathfinding (w/ or w/o diagonals) ```TODO```
- [ ] Poisson disc sampling ```TODO```
- [ ] Entity FOV calculation function ```TODO```

## Preview

```TODO```

## Build

```TODO```

## Dependencies

- [ocornut/imgui](https://github.com/ocornut/imgui)
- [ggerganov/imtui](https://github.com/ggerganov/imtui)
- [libsdl-org/SDL](https://github.com/libsdl-org/SDL)*
- [ncurses](https://invisible-island.net/ncurses/announce.html)*

**\*** Not provided in this repo, must be built + linked from system

## LICENSE

```
RLUT is a utilities toolkit for making rogue-likes

Copyright (C) 2024  George Watson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
