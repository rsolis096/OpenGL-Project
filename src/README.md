# A simple 3D rasterizer
## Controls

| Key  | Action |
| ------------- | ------------- |
| Mouse  | Look Around  |
| W  | Translate Forwards  |
| A | Translate Left  |
| S | Translate Backwards  |
| D  | Translate Right  |
| Space  | Ascend  |
| LCTRL  | Descend  |
| LSHIFT  | Increase Movement Speed |
| ` | Open Menu, once GUI is open use Mouse to interact  |
| Scroll Wheel  | Increase/Decrease Field of View (Zoom)  |
| ESC  | Close Program  |

## Includes
- Real time manipulation of 3D objects such as Position, Rotation, Textures and Colours through a Graphical User Interface
- Real Time Shadows produces using shadow maps
- Blinn Phong Lighting for Spot Lights, Point Lights and Directional Lights (Currently only SpotLights are available for shadow map purposes, other lights planned to be reintroduced)
- Simple gravity implementation

## How To Run

A standalone executable does not currently exist and for that reason I recommend running this application in Visual Studio. This will require you to compile a debug build of the assimp library from https://github.com/assimp/assimp. Once built, inside the assimp/build/bin/Debug folder you will find assimp-vc143-mtd.dll. Build this OpenGL project in debug and place this assimp-vc143-mtd.dll in OpenGL Project\x64\Debug.

![image](https://github.com/rsolis096/OpenGL-Project/assets/63280140/824838b2-cafa-46a1-b358-2600c045b7c2)


