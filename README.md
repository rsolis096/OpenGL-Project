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
| LSHIFT (Hold)  | Increase Movement Speed |
| ` | Open Menu, once GUI is open use Mouse to interact  |
| Scroll Wheel  | Increase/Decrease Field of View (Zoom)  |
| ESC  | Close Program  |

## Includes
- Real time manipulation of 3D objects such as Position, Rotation, Textures and Colors through a Graphical User Interface
- Real Time Shadows Maps (Directional, Spot, Point)
- Blinn Phong Lighting for Spot Lights, Point Lights and Directional Lights 
- Model Loading

## How To Run

#### <ins>Using the executable</ins>
- Views the "Releases" tab and download the latest executable of the project
- This project expects assets which can be downloaded [here](https://drive.google.com/drive/folders/1xrtGR_Ftvw49WWUczeX7o8o1jT-aFrRH?usp=sharing). Store the assets folder in the same directory as the executable
- Run the executable

#### <ins>Using Visual Studio 2022</ins> 
(If any of the following folders do not exist, then create them with the folder storing the .sln acting as the root)
- Download GLFW 64-bit Windows Binaries [here](https://www.glfw.org/download).
- Copy glfw3.lib into "third party\lib\glfw\Debug" and "third party\lib\glfw\Release".
- Build Assimp 64-bit binaries by downloading [assimp](https://github.com/assimp/assimp) and running BUILDBINARIES_EXAMPLE.bat for your version of Visual Studio.
- Copy assimp-vc143-mt.lib into "third party\lib\assimp\Release" and assimp-vc143-mtd.lib into "third party\lib\assimp\Debug"
- Copy assimp-vc143-mt.dll into "x64\Release" and assimp-vc143-mtd.dll into "x64\Debug"
- This project expects assets which can be downloaded [here](https://drive.google.com/drive/folders/1xrtGR_Ftvw49WWUczeX7o8o1jT-aFrRH?usp=sharing). Store the assets folder in the same directory as the .sln
  
## Screenshots
### Point Light Shadows
![sponza-demo](https://github.com/user-attachments/assets/ceccfcf2-3ba8-48a2-954a-fca7fdb37943)

#### Spot Light Shadows
![image](https://github.com/rsolis096/OpenGL-Project/assets/63280140/824838b2-cafa-46a1-b358-2600c045b7c2)




