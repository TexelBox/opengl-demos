1. goto https://glad.dav1d.de/
2. use C/C++, OpenGL, Core, gl version (whatever), gles1,gles2,glsc2=None, extensions (none or any), generate a loader
3. Generate, download zip file, unzip it
4. move include folder into middleware location (+ link with project)
5. Add Existing File (glad.c) - this allows the actual file location to stay in middleware, the project just links to it.