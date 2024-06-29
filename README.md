# GPG

Before trying to build run `download_dependencies.py` and `install_dependencies.py` to download, build and install newton-dynamics 
lib (https://github.com/MADEAPPS/newton-dynamics).



## Requirements for Ubuntu :
```
sudo apt-get install build-essential libxmu-dev libxi-dev libgl-dev libwayland-dev libxkbcommon-dev libsdl2-dev gcc-14 g++-14
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
DYLD_LIBRARY_PATH=${VulkanSdkPath}/macOS/lib ./OPGLPlayground
```

## Requirements for Windows :
Dont know yet xD

## TODO/DONE:

TODO:
- [ ] Finish Vulkan implementation. (make it simpler ?)  
- [ ] Create physics system with NewtonLibrary.  
- [ ] Try to build and run on windows.  

Helper: 

✅
[x]
[]
