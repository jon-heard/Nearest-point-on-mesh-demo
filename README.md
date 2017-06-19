# Nearest point on mesh - demo
The goal of this project is to create a simple 3d model viewing application with one extra wrinkle.

Features include:
- Load and view model files in the OFF format.
- Load and view built in, primitive models.
- Rotate and zoom the model. <i>(keyboard and/or mouse)</i>

The wrinkle:
- Control a "focus" point in 3d space, represented as a red sphere. <i>(keyboard and/or mouse)</i>
- Target is drawn onto the model at the point that is closest to the user-controlled "focus" point.

![An image of the application](readme_screen01.png?raw=true "The application")

## Implementation details

### File loader
The file loader works with the majority of OFF files, with convex faces of arbitrary vertex counts.  It does <i>not</i> work with color data.  Color data was not part of the OFF specs referenced during development.

<b>Enhancements: </b> to properly read color data, or at least properly ignore it.

Steps to load an OFF file:
1) Load the file content into a string
2) Remove comments
3) Tokenize the data
4) Parse the tokens into vertices and faces
5) Pass those vertices and faces into the model

### Nearest point calculation
This algorithm involves iterating over all triangles to find the point on each triangle that is closest to the "focus" point.  As each triangle is checked, the shortest overall point is kept and returned at the end.

The algorithm that I used to find the nearest point on a triangle was from <a href='https://www.gamedev.net/forums/topic/552906-closest-point-on-triangle/'>this forum page</a>.  Here is a brief description:

The "focus" point is transformed to be on a 2d plane space where the triangle's vertices are at (0,0), (0,1) and (1,0).  This reduces the problem to finding the closest point on the "unit" triangle to the 2d "focus" point.

<b>Enhancements: </b>I was planning to use an octree or bsp to cull some triangle checks, but speed never became an issue.  Still, it'd be a good first choice for optimization.

### The target decal
This was certainly the most involved feature.

After searching / reviewing source material on decals that conform to a mesh, I decided to start with "projected texture mapping" as it had many of the desired features.
Projected texture mapping has a few drawbacks however.
1) It projects through all layers, rendering the texture on the back side as well
2) Its deformations are often too "blobby".

![An image of projected texture drawbacks](readme_screen02.png?raw=true "Projected texture mapping drawbacks")

I fought issue #1 by preventing projection onto back faces.

I handled issue #2 by scaling texture coordinates (after they were generated) to match the distance of the vertex from the texture's center coordinates (the "nearest point" coordinate).

<b>Enhancements: </b> I never ended up trying the old school way of generating the decal with special geometry.  It just seemed to be too involved and bug prone to make it worth my time.  Still, this could produce better results.
Also, the current technique still has some minor bugs I'm working to fix.

### Other

<b>Basic shaders</b> - The shaders are simple and sufficiently functional.  Lighting is based on a dot product between the look vector and the normal.

<b>Controls</b> - Controls mostly involve dragging the mouse to rotate the scene or the "focus" point, zooming with the mouse wheel and hitting keys for various other transformations.


### C++ class list:
- MainWindow - The overarching UI controller and container.
- SceneUi - Represents the UI widget that draws a 3d scene and provides user input to manipulate the scene.
- Scene - Represents an entire 3d scene.  To be subclassed for a specifc scene.
- Scene_NearestPointDemo - Represents the scene used in this application.
- Model - All objects in the scene are represented by instances of this class (or a subclass).
- ModelLoader_Primitive - Container for the 'loadSphereIntoModel' and 'loadBoxIntoModel' methods.  Used to load a model with the mesh data of an ico-sphere or a box.
- ModelLoader_File_OFF - Container for the 'loadFileIntoModel' method.  Used to load a model with mesh data from an OFF file.
- Model_WithCalculations - A subclass of 'Model' that adds a lot of functionality useful for the mesh.

