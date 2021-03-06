# Nearest point on mesh - demo
The goal of this project is to create a simple 3d model viewing application with one extra wrinkle.

Features include:
- Load and view model files in the OFF format.
- Load and view built in, primitive models.
- Rotate and zoom the model. <i>(keyboard and/or mouse)</i>

The wrinkle:
- Control a "focus" point in 3d space, represented as a red sphere. <i>(keyboard and/or mouse)</i>
- A target is drawn onto the model at the point that is closest to the user-controlled "focus" point.

You can download the windows version [here](https://github.com/jon-heard/Nearest-point-on-mesh-demo/raw/master/build/Nearest-point-on-mesh-demo_win64.zip).

![An image of the application](readme_screen01.png?raw=true "The application")

## Implementation details

### C++ class list:
- <b>MainWindow</b> - The overarching UI controller and container.
- <b>SceneUi</b> - Represents the UI widget that draws the 3d scene and provides user input to manipulate the scene.
- <b>Scene</b> - Represents an entire 3d scene.  To be subclassed for a specifc scene.
- <b>Scene_NearestPointDemo</b> - A subclass of "Scene".  Represents the particular scene used in this application.
- <b>Model</b> - All objects in the scene are represented by instances of this class.
- <b>ModelLoader_Primitive</b> - A collection of methods for loading models with meshes of primitive shapes (sphere, box).
- <b>ModelLoader_File_OFF</b> - A collection of methods for loading models with meshes from 'OFF' model files.
- <b>Model_WithCalculations</b> - A subclass of 'Model' that adds extra functionality, useful for the application's 'mesh' model.

### File loader
The file loader works with the majority of OFF files, with convex faces of arbitrary vertex counts.  It does <i>not</i> work with color data.  Color data was not part of the OFF specs referenced during development, though it <i>is</i> included in certain OFF specs that I found later on.

I first implemented OFF file loading with std::string and typical C++ file I/O.  I eventually switched to QT's file handling to allow loading internal application resources.  I found QT's file handling and QString implementation to be easy to work with.  I would happily use them again.

Steps to load an OFF file:
1) Load the file content into a string
2) Remove comments from the string
3) Tokenize the string
4) Parse the tokens into vertices and faces
5) Pass those vertices and faces into the model

<b>Future enhancements: </b> to properly read color data, or at least properly ignore it.

### Nearest point calculation
This algorithm involves iterating over all triangles, finding the point on each triangle that is closest to the "focus" point.  As each triangle is checked, the shortest overall point is kept and returned at the end.

The algorithm that I used to find the nearest point on a <i>single</i> triangle was one found on <a href='https://www.gamedev.net/forums/topic/552906-closest-point-on-triangle/' target='_blank'>this forum page</a>.  The algorithm integrated well with my code, with minimal work.  Here is a brief description of how it works:

The "focus" point is transformed to be on a 2d plane space where the triangle's vertices are at (0,0), (0,1) and (1,0).  This reduces the problem to finding the closest point on the "unit" triangle to the 2d "focus" point.

<b>Future enhancements: </b>I was planning to use an octree or bsp to reduce the number of triangle checks, but speed never became an issue, even on an old laptop.  Still, this would be a good first choice for optimization.

### The target decal

First, the decal types: These represent my evolution of decal processing.  Specifically, they represent points where bugs have been introduced.  The current decal type can be specified in the UI.
1) <b>No decal</b> - Do not calculate or render the decal at all.
2) <b>Projected</b> - Use pure projected texture mapping to render the decal.
3) <b>Projected (manual texcoord)</b> - The same as projected, except that the projected texture coordinates are manually calculated.  I calculated the texture coordinates based on a number of references, including the kronos website, but this process still introduces a bug wherein texture artifacts can occasionally be seen on the mesh.
4) <b>Projected (truncate)</b> - Use projecte texture mapping to render the decal, but also scale texture coordinates to be bound in 3d space.
5) <b>Projected (truncate, no back)</b> - Same as "Projected (truncate)" except that the projection is also blocked from projecting onto back surfaces.

This was definitely the most involved feature.

I began by searching / reviewing source material on various techniques for rendering decals that conform to a mesh.  I eventually chose to start with "projected texture mapping" as it had many of the desired features.
After implementing projected texture mapping, however, I realized that it also has a number of drawbacks.
1) The texture is projected through all layers, including both the front and back of the mesh.
2) The projected texture can be too "blobby" when projected onto surfaces that are close to perpendicular with the projection vector.

![An image of projected texture drawbacks](readme_screen02.png?raw=true "Projected texture mapping drawbacks")

I tried implementing a few other techniques for rendering conforming decals, but ultimately settled on projected textures after some adjustments to address the stated issues.

I worked around issue #1 by preventing the texture from projecting onto back faces.  This doesn't stop the projected texture from rendering multiple times when the mesh has two front facing surfaces in the projection path.  The next adjustment greatly minimizes this issue, but it can still be a problem in some circumstances.

I handled issue #2 with the following vertex shader technique.  After generating the texture coordinates for the vertex with projected texture mapping, I calculate how far the vertex is from the center of the decal (from where the "nearest point" is).  I then take that distance and scale the texture coordinates so that it's length matches the distance.

My solution to issue #2 had a startlingly positive impact, and was the primary reason that I ended up staying with projected textures for the decal.  It isn't perfect, however.  I needed to turn the projected texture coordinates (vec4) into normal 2d texture coordinates  (vec2) to be able to scale them.  This conversion introduces an occasional glitch in the texturing mapping, which I'm actively trying to resolve.

<b>Future enhancements: </b> I never ended up trying the old school way of generating the decal with a special dynamic mesh.  It just seemed too involved and bug prone to make it worth my time.  Still, this could possibly produce better results.
Also, the current technique still has some minor bugs I'm working to fix.

### Other

<b>Basic shaders</b> - The shaders are simple and sufficiently functional.  Lighting is based on a dot product between the camera's look vector and the vertex normal.  When I ported this application to my old linux laptop, I needed to reduce the GLSL version from 3.3 to 1.2, which required a number of superficial adjustments, but nothing structurally significant.

<b>Controls</b> - Controls mostly involve dragging the mouse to rotate either the scene or the "focus" point.  There is also zooming with the mouse wheel and hitting keys for other controls.  It took a bit of thinking to get controls for the "focus" point to function relative to the camera/scene transform, rather than relative to the world axis.  This work was worth it, though, as it made controlling the "focus" point <i>much</i> more intuitive.

### Known bugs

#### Artifacts with texture conversion
My technique required manually converting projected texture coordinates to standard 2d texture coordinates.  I've used what seems to be the correct technique, but manual conversion introduces artifacts.

Steps to reproduce:
1) Load a low-poly sphere
2) Set decal type to "Projected"
3) Move the "focus" point within the mesh
4) Change decal type to "Projected (manual texCoord)"
5) Notice the artifacts that are added with the second decal type.  The only difference is the projected to standard texture coordinate conversion.

<i>Note: The vertex shaders used for these decal types are "mesh_projectedTexture.vert" and "mesh_projectedTexture_manualTexCoord.vert" respectively</i>

#### Rare unprojected polygons

Occasionally a spacial configuration will occur that results in one of the polygons not showing the projected texture.  This is rare, and quickly passes when any object in the scene moves.

#### Double projection

If a mesh has two surfaces facing the same direction and they are close enough, the target may be rendered onto both surfaces.
