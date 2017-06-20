# Nearest point on mesh - demo
The goal of this project is to create a simple 3d model viewing application with one extra wrinkle.

Features include:
- Load and view model files in the OFF format.
- Load and view built in, primitive models.
- Rotate and zoom the model. <i>(keyboard and/or mouse)</i>

The wrinkle:
- Control a "focus" point in 3d space, represented as a red sphere. <i>(keyboard and/or mouse)</i>
- A target is drawn onto the model at the point that is closest to the user-controlled "focus" point.

![An image of the application](readme_screen01.png?raw=true "The application")

## Implementation details

### File loader
The file loader works with the majority of OFF files, with convex faces of arbitrary vertex counts.  It does <i>not</i> work with color data.  Color data was not part of the OFF specs referenced during development.

I first implemented model file loading with std::string and typical C++ file I/O.  I eventually switched to QT's file handling to allow loading resources.  I found QT's file handling and QString implementation to be wonderful to work with.  I would happily use them again.

<b>Enhancements: </b> to properly read color data, or at least properly ignore it.

Steps to load an OFF file:
1) Load the file content into a string
2) Remove comments
3) Tokenize the data
4) Parse the tokens into vertices and faces
5) Pass those vertices and faces into the model

### Nearest point calculation
This algorithm involves iterating over all triangles to find the point on each triangle that is closest to the "focus" point.  As each triangle is checked, the shortest overall point is kept and returned at the end.

The algorithm that I used to find the nearest point on a <i>single</i> triangle was one found on <a href='https://www.gamedev.net/forums/topic/552906-closest-point-on-triangle/' target='_blank'>this forum page</a>.  The algorithm integrated well with barely any tweaking.  Here is a brief description of how it works:

The "focus" point is transformed to be on a 2d plane space where the triangle's vertices are at (0,0), (0,1) and (1,0).  This reduces the problem to finding the closest point on the "unit" triangle to the 2d "focus" point.

<b>Enhancements: </b>I was planning to use an octree or bsp to reduce the number of triangle checks, but speed never became an issue, even on an old laptop.  Still, this would be a good first choice for optimization.

### The target decal
This was definitely the most involved feature.

After searching / reviewing source material on decals that conform to a mesh, I decided to start with "projected texture mapping" as it had many of the desired features.
After implementing projected texture mapping, however, I realized that it has a number of drawbacks as well.
1) The texture is projected through all layers, including the front and back of the mesh.
2) The projected texture can be too "blobby" when projected onto surfaces that are close to perpendicular with the projection vector.

![An image of projected texture drawbacks](readme_screen02.png?raw=true "Projected texture mapping drawbacks")

I tried a few other techniques for the decal target texture, but ultimately settled on projected textures after some adjustments to fix the issues.

I worked around issue #1 by preventing the texture from projecting onto back faces.  This doesn't stop the projected texture from rendering multiple times when the mesh has two front facing surfaces in its way.  The next point minimizes this issue, but it can still be a problem in some circumstances.

I handled issue #2 with the following vertex shader technique.  After generating the texture coordinates for the vertex, I calculated how far the vertex was from the center of the projection (from where the "nearest point" is).  I then took that distance and scaled the texture coordinates to be of that length.

My solution to issue #2 had a startlingly positive impact, and was the primary reason that I ended up staying with projected textures for the decal.  It isn't perfect, however.  I needed to turn the projected texture coordinates into normal 2d texture coordinates to be able to scale them.  This conversion introduces an occasional glitch in the texturing mapping, which I'm actively trying to resolve.

<b>Enhancements: </b> I never ended up trying the old school way of generating the decal with a special dynamic mesh.  It just seemed too involved and bug prone to make it worth my time.  Still, this could possibly produce better results.
Also, the current technique still has some minor bugs I'm working to fix.

### Other

<b>Basic shaders</b> - The shaders are simple and sufficiently functional.  Lighting is based on a dot product between the look vector and the normal.  When I ported this application to my old linux laptop, I needed to reduce the GLSL version from 3.3 to 1.2, which required a number of superficial adjustments, but nothing structurally significant.

<b>Controls</b> - Controls mostly involve dragging the mouse to rotate the scene or the "focus" point, zooming with the mouse wheel and hitting keys for various other transformations.  It took a bit of thinking to get the controls for the "focus" point to work relative to the scene rotation, but it was worth it as it made controlling the "focus" point <i>much</i> easier.


### C++ class list:
- MainWindow - The overarching UI controller and container.
- SceneUi - Represents the UI widget that draws the 3d scene and provides user input to manipulate the scene.
- Scene - Represents an entire 3d scene.  To be subclassed for a specifc scene.
- Scene_NearestPointDemo - A subclass of "Scene".  Represents the particular scene used in this application.
- Model - All objects in the scene are represented by instances of this class.
- ModelLoader_Primitive - A collection of methods for loading models with meshes of primitive, generated shapes (sphere, box, etc).
- ModelLoader_File_OFF - A collection of methods for loading models with meshes from 'OFF' model files.
- Model_WithCalculations - A subclass of 'Model' that adds extra functionality, useful for the application's 'mesh' model.

