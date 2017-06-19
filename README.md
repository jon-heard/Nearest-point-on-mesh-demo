# Nearest point on mesh - demo
The goal of this project is to create a 3d model viewing application with a few extra features.

Features include:
- Can load object file format (OFF) model files to be viewed in the application.
- Can load a number of built in, primitive models for testing.
- Allows the user to rotate and zoom the model. <i>(keyboard and/or mouse)</i>
- Allows the user to control a "focus" point in 3d space, represented as a red sphere. <i>(keyboard and/or mouse)</i>
- Draws a target on the model at the point that is closest to the "focus" point.

## Implementation details

### File loader
The file loader works with most OFF files.  It handles faces with arbitrary vertex counts.  It does <i>not</i> work with OFF files with color data, as that was not in the specs listed on wikepedia or princeton's website.

<b>Enhancements: </b> to properly read color data, or at least properly ignore it.  Right now, it fails to load an OFF file if it contains color data.

Steps to load an OFF file:
1) Load the file contents into a string
2) Remove comments
3) Tokenize the data
4) Parse the tokens into vertices and faces
5) Pass those vertices and faces into the model

### Basic shaders
The shaders are simple and sufficiently functional.  Lighting is based on a dot product between the look vector and the normal.

### Controls
Controls mostly involve dragging the mouse to rotate the scene or the "focus" point, zooming with the mouse wheel and hitting keys for various other transformations.

### Nearest point calculation
This algorithm involves iterating over all triangles to find the point on each triangle that is closest to the "focus" point.  As each triangle is checked, the shortest overall point is kept and returned at the end.

<b>Enhancements: </b>I was planning to use an octree or bsp for optimization here, but speed never be came an issue.  Still, it'd be an obvious optimization choice if input data starts becoming to dense.

### The target decal
This was certainly the most involved feature.

After searching and reviewing source material about decals that deform to a mesh, I decided to start with "projected texture mapping" as it had many of the wanted features.
Straight projection mapping had a few drawbacks however.
1) It projects right through to the other side of the mesh.
2) Its deformations are often too "blobby" to be of use.

I fought issue #1 by preventing projection onto back faces.
As for issue #2, this was handled by scaling texture coordinates to match the distance from the texture's center.

<b>Enhancements: </b> I never ended up trying the old school way of generating the decal.  It just seemed to be too involved and too bug prone to make it worth my time.  Still, this may result in improvements.
Also, there are some minor bugs I'm still ironing out with the current technique.

### C++ class list:
- MainWindow - The overarching UI controller and container.
- SceneUi - Represents the UI widget that draws a 3d scene and provides user input to manipulate the scene.
- Scene - Represents an entire 3d scene.  To be subclassed for a specifc scene.
- Scene_NearestPointDemo - Represents the scene used in this application.
- Model - All objects in the scene are represented by instances of this class (or a subclass).
- ModelLoader_Primitive - Container for the 'loadSphereIntoModel' and 'loadBoxIntoModel' methods.  Used to load a model with the mesh data of an ico-sphere or a box.
- ModelLoader_File_OFF - Container for the 'loadFileIntoModel' method.  Used to load a model with mesh data from an OFF file.
- Model_WithCalculations - A subclass of 'Model' that adds a lot of functionality useful for the mesh.

