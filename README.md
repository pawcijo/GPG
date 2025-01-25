# GPG

Before trying to build run `download_dependencies.py` and `install_dependencies.py` to download, build and install newton-dynamics 
lib (https://github.com/MADEAPPS/newton-dynamics).

Run also `git submodule update --init --recursive` for needed submodules (glfw,glm,imgu,SPIRV).

In short :

```
python3 download_dependencies.py
python3 install_dependencies.py
git submodule update --init --recursive 

```

Also remember to build shaders in `build/shaders` directory.

For shaders you will need Vulkan SDK for your platform: https://vulkan.lunarg.com/#new_tab.



## Requirements for Ubuntu :
```
sudo apt-get install build-essential libxmu-dev libxi-dev libgl-dev libwayland-dev libxkbcommon-dev libsdl2-dev gcc-14 g++-14
sudo apt install vulkan-tools
sudo apt install libvulkan-dev
sudo apt install vulkan-validationlayers-dev spirv-tools

```
```
pip install python-git
```

## Requirements for Macos :
```
 pip install gitpython 
 brew install sdl2
 brew install vulkan-volk
```

Running binary on mac:
```
DYLD_LIBRARY_PATH=${VULKAN_SDK_PATH}/macOS/lib ./GPGPlayground
```

## Requirements for Windows :
Dont know yet xD

## TODO/DONE:

TODO:
- [ ] Finish Vulkan implementation. (make it simpler ?)  
- [ ] Create physics system with NewtonLibrary.  
- [ ] Try to build and run on windows. 

TODO ASAP:
- [ ] Fix memory leak for loading xml scene
- [ ] Fix rotation for reloading scene

Helper: 

✅
[x]
[]
