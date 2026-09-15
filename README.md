How to compile (VS2022)

Download:
  - [SDL3](https://github.com/libsdl-org/SDL),
  - [SDL3_image](https://github.com/libsdl-org/SDL_image)
  - [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer)
  - [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
  - [nlohmann JSON](https://github.com/nlohmann/json)
  - optionally, [Visual Leak Detector](https://github.com/Azure/vld)

Make sure you have the **SDL3** versions of each SDL library, and that you download the **VC** (example: SDL3-devel-3.4.14-VC.zip; latest at the writing of this README).

Download and unzip, then go in VS2022 -> (Open your project here) -> Project (bar at the top) -> (ProjectName) Properties and do the following:

In C/C++ -> General -> Additional Include Directories -> Add path/to/each/directory/SDL3/include; path/SDL3_image/include; path/SDL3_ttf/include; path/nlohmann/single_include; ...

In C/C++ -> Language -> C++ Language Standard -> C++20

In Linker -> General -> Additional Library Directories -> Add path/SDL3/lib/x64 (or x32); path/SDL3_image/lib/x64; ...; Note that nlohmann JSON doesn't need anything here.

In Linker -> Input -> Additional Dependencies -> Add SDL3.lib; SDL3_image.lib; SDL3_ttf.lib; SDL3_mixer.lib; vld_x64.lib

Give Intellisense a second to adjust itself, then you should be ready to go! If there are errors, you are entirely on your own!
