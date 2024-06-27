# RLUT

> [!WARNING]
> Work in progress!

**R**ogue-**L**ike **U**tilities **T**oolkit is a minimal + lightweight framework with most of the basic necessities to make a rogue-like game (probably).

The name and API are based off GLUT. The idea and design of RLUT is a mixture of GLUT and NCurses. RLUT doesn't handle any sort of game logic or map drawing; it just handling window + context creation, and rendering an NCurses-like screen buffer.

RLUT is designed to be built seemlessly as either a classic terminal process, or as a GUI application (SDL2) without having to change anything. ImGui is integrated into RLUT and works in both TUI and GUI builds (thanks to [ggerganov/imtui](https://github.com/ggerganov/imtui)).

## Features

- [X] Simple API to read, use and bind to another language
- [X] Window creation + event handling
    - [X] NCurses (TUI)
    - [ ] SDL2 (GUI) ```TODO```
- [X] NCurses-like cursor + print functions
- [X] Map generation functions
    - [X] Cellular automata (caves)
    - [X] Perlin + FBM (islands or overword)
    - [ ] More map generators ...
- [X] Random number generation functions
- [ ] AStar pathfinding (w/ or w/o diagonals) ```TODO```
- [ ] Poisson disc sampling ```TODO```
- [ ] Entity FOV calculation function ```TODO```

## Preview

```TODO```

## LICENSE
```
The MIT License (MIT)

Copyright (c) 2024 George Watson

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

```
