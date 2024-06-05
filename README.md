# OGLPlayground

Some text bla bla bla .


## Requirements for Ubuntu :

**sudo apt-get install build-essential libxmu-dev libxi-dev libgl-dev libwayland-dev libxkbcommon-dev libsdl2-dev gcc-14 g++-14**

Before build export CXX and GCC to gcc-14 version.

Run with x11 as wayland is bugged as hell. (at least with intel iGpu)

(might need more TODO check on clean system)


## TODO/DONE:
** **
**30.03.24**
- ✅ Update GLEW 
- ✅ Update GLFW
- ✅ Update ImGui
- ✅ Window Class
- ✅ App Class
- ✅ Shader Class
- ✅ Move external to git-submodules
- ✅ Transform class
- ✅ Camera class
** **
**31.03.24**
- ✅ Quaternion in camera
- ✅ Make transform work 
- ✅ Make camera work
- ✅ Add switch for wire render mode
- ✅ Added texture loading 
- ✅ Added Box(Cube)
**1.04.24**
- ✅ Add switch for Orthographic or perspective projection

**2.04.24**
- ✅ Imgui works

**3.04.24**
- ✅ Imgui control for camera proporties 

**4.04.24**
- ✅ Color picking to grab object (Boxes for now)
- ✅ AABB

**14.05.24**

TODO:
- [ ] Texture class
- [ ] Save configuration
- [ ] Model class
- [ ] Scene Hierarchy with classes (weak, shared ptrs ?)

- [✅] Collison on CPU: (Based on qu3e)
- [ ] Move collison to cpu
- [ ] Load fonts to display at screen
- [ ] FPS counter + Text display 
- [ ] More robust time (use realtime not const time per update)


Maybe: ?
- [ ] Add ignore
- [ ] Cloth after dealing with simple collison 
- [ ] Bouncing material (Like slime in minecraft)





Helper: 

✅
[x]