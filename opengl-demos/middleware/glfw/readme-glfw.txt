1. goto https://www.glfw.org/download.html
2. click "Source Package"
3. unzip file, cd glfw-x.y
4. mkdir build, cd build, cmake ..
5. open GLFW.sln
6. for glfw project properties, set same SDK version as my projects, update debug/Win32 runtime library to /MTd and release/Win32 runtime library to /MT
7. build glfw project (right click + build) in debug/Win32 and then in release/Win32
8. copy include folder into middleware location
9. copy debug/glfwx.lib and release/glfwx.lib into middleware location
10. setup include/lib paths in VS projects that use GLFW