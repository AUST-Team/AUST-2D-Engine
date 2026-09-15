![AUST Logo](<img width="1848" height="471" alt="AUSTLogo" src="https://github.com/user-attachments/assets/d44ca718-a447-4219-8ba7-8b889ee108a0" />)
# AUST 2D ENGINE

The AUST 2D Engine (AUST 2D or AUST for short), is a passion project of a single person to make a 2D game engine in C++. Original made in Java as a college project, it now features:
- Animations
- Sound
- ~~A lot of JSON~~ Data-driven architecture using JSON! (see now it sounds fancy)
- Custom UI
- ZERO smart pointer usage
- Basic modability
- C++20 features
- ... and probably a lot of bugs!

---

## How to play

1. Download the latest ZIP archive from the [Releases](../../releases) tab.
2. Extract the archive to a folder of your choice.
3. Run `AUST2D.exe`.

---

## Building from Source

This project is built with C++20 using MSVC (Visual Studio 2022) on Windows. It has not yet been tested on GCC/Clang or other platforms.

### Dependencies

If you place dependency folders alongside the project directory (e.g., `..\SDL3`), Visual Studio will resolve them automatically using relative paths.

Required libraries (Ensure you download the **VC** development packages, e.g., `SDL3-devel-3.x.x-VC.zip`):
- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
- [nlohmann JSON](https://github.com/nlohmann/json)
- [Visual Leak Detector](https://github.com/Azure/vld) *(Optional - only for Debug builds)*

### Manual Project Setup (from scratch)

1. Open your solution in Visual Studio 2022.
2. Go to **Project Properties** (`Alt` + `F7`):
   - **C/C++ > General > Additional Include Directories**: Add paths to `include/` for SDL3, SDL3_image, SDL3_ttf, SDL3_mixer, and `single_include/` for nlohmann JSON.
   - **C/C++ > Language > C++ Language Standard**: Set to **ISO C++20 Standard (`/std:c++20`)**.
   - **Linker > General > Additional Library Directories**: Add paths to `lib/x64/` for all SDL libraries.
   - **Linker > Input > Additional Dependencies**: Add `SDL3.lib; SDL3_image.lib; SDL3_ttf.lib; SDL3_mixer.lib;`.
3. Build and Run!

If it doesn't work after that, you are entirely on your own!

---

## Licensing

* **Source Code:** Licensed under the [MIT License](LICENSE.txt).
* **Game Assets:** See `resources/aust/LICENSE.txt` for specific asset terms.

---

## Contributing & Bug Reports

Pull requests and monetary donations are not accepted at this time.

If you want to support the project, please playtest the engine and submit bug reports via [GitHub Issues](../../issues)!

---

## FAQ

- I found a bug. What do I do?

Please use GitHub’s issues tab to report it; include steps to reproduce it, console / game logs (if available), and specify whether you were testing the original game files or a custom mod.
If you found a bug and know how to program (especially if you know C++), you can include additional information (memory report, possible causes, etc).

- I would like to help / contribute.

I’m glad you’re enthusiastic about helping a random game engine on the Internet. Unfortunately, not accepting pull requests until I make contributing and code writing guidelines (and learning how pull requests work), unless you are very very adamant about contributing to this project, then please get in touch and we can discuss.
If you wish to donate instead, I am also not accepting donations until further notice.
If you wish to help in other ways, playtest the engine.

- I want feature X / Why doesn’t this do Y / This doesn’t meet my expectations.

The AUST engine has been developed by a single person in his spare time in around ~6 months. While the first release will surely be very limiting, more features are planned. For the first release of the engine, I aimed to release it in a way that someone can make maps and UI for it; as such, some requested features may be missing, but these will be added as, hopefully, this engine keeps being developed.
- Why should I use this instead of other engines (RPGMaker, GameMaker, Godot, etc)?

Nobody is forcing you to use it. This is more a passion project than trying to compete with other well-established, mature engines.

- I would like to make a map.

Unfortunately, the first release of AUST does NOT ship with any tile, JSON, map or mod making tools. That means your best bet is to get to writing JSON by hand. Of course, you can just copy the files from aust/ and modify them.

- Why did it take so long to make?

The original plan was to make a game, just a game, not engine, so that means everything would be a little more hardcoded. Then I wanted to expand it to make easier to make said game with (a map.json instead of hardcoding, a configuration.json instead of hard coding everything), and slowly (thanks feature creep) it got to the point I was just “Ok, let’s make it into an engine”

- Why did you make it?

The story of AUST (the game) started as a college game project for a Java programming class (yes, the original original AUST is in Java). Then I wanted to rewrite it in C++ because I like the language and it would help my C++ skills. 6 months of development time, and here we are.

- So is this a game or a game engine?

Currently, just a game engine, but after more features will be added, I will start making the actual game.

- Has AI been used in this project? If yes, how?

With a heavy heart to all hardline AI haters that think even a single line of code written by one poisons the whole codebase, yes, AI has been used in this project.
Used in: writing templated functions and documentation (template as in they all followed the same code / writing style, see JSONParser.h comments, and the XToString() and XFromString functions for sentinel enums), generating drafts of code for ideas (such as the different classes in the Audio system; though not all), or reviewing code (such as giving it a function, snippet or file and to point out possible non-compilation / static analysis errors; missing breaks in switches, dangling pointers), checking the different ways (or atleast the ones it had access to) of achieving the same thing.
However, everything AI has given me, it was analyzed, understood and rewritten to meet my specifications (unless it was something super basic that I would’ve also written). AI is a tool, not a subsitutite.

- Could this project been possible without AI?

Absolutely! Though it would’ve taken a ton more time, and probably would’ve had less features. I would like to use less and less AI, but some things are perfect for it (templated stuff, which is just copy paste and change some words), but for everything else (code reviews, discussing methods of solving more complicated parts), I would really like to have someone with similar (or, more preferably, more) experience than me in these mattes.

- Why did you use JSON?

There are parsers available (like the one I use from Nlohmann), it’s quite intuitive to use until game creation tools get made. Once the engine is in a comfortable position, it might be switched to another format; either custom or another established format.

- Other than GitHub, where can ideas / bugs be discussed? Is there any community server to get mods from or talk to other people?

There is currently no Discord server, subreddit, etc, made, mostly because I am too busy to moderate and manage properly. If anyone wants to help in that regard, please contact me.

- What is next?

Well I would first like to keep adding mechanics to the engine (key rebinding; hopefully in-game as well, scripting possibly, make it compile on other systems, etc), then maybe actually make AUST according to the ORIGINAL game design document from that college project. The most high priority systems that must be refactored first are the same systems that existed in the original Java version of the game. Because I wanted to speed up development, I mostly copied (with very minimal changes) the already existing Java code and ported it to C++, and trust me, that Java code is horrible.
