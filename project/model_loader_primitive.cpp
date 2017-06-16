#include "model_loader_primitive.h"
#include <vector>
#include <QVector3D>
#include "Model.h"

using namespace std;

bool Model_Loader_Primitive::loadSphereIntoModel(Model* model, float radius, int subdivisionCount)
{
  if (model == NULL)
  {
    return false;
  }
  // initial mesh (a cube)
  vector<QVector3D> vertices = vector<QVector3D>({
                                 {-1, -1, -1},
                                 {1, -1, -1},
                                 {-1, -1, 1},
                                 {1, -1, 1},
                                 {-1, 1, -1},
                                 {1, 1, -1},
                                 {-1, 1, 1},
                                 {1, 1, 1}
                               });
  vector<QVector3D> tris = vector<QVector3D>({
                              {0, 1, 2}, {3, 2, 1}, // y-neg
                              {4, 6, 5}, {7, 5, 6}, // y-pos
                              {4, 0, 6}, {2, 6, 0}, // x-neg
                              {5, 7, 1}, {3, 1, 7}, // x-pos
                              {4, 5, 0}, {1, 0, 5}, // z-neg
                              {6, 2, 7}, {3, 7, 2}  // z-pos
                           });
  // subdivide
  for (int i = 0; i < subdivisionCount; ++i)
  {
      int triCount = (int)tris.size();
      for (int i = 0; i < triCount; ++i) {
          QVector3D oldVertex1 = vertices[tris[i][0]];
          QVector3D oldVertex2 = vertices[tris[i][1]];
          QVector3D oldVertex3 = vertices[tris[i][2]];
          int newVertexIndex = (int)vertices.size();
          vertices.push_back((oldVertex1 + oldVertex2) / 2);
          vertices.push_back((oldVertex2 + oldVertex3) / 2);
          vertices.push_back((oldVertex1 + oldVertex3) / 2);
          tris.push_back(QVector3D(newVertexIndex + 2, tris[i][0], newVertexIndex + 0));
          tris.push_back(QVector3D(newVertexIndex + 1, newVertexIndex + 0, tris[i][1]));
          tris.push_back(QVector3D(tris[i][2], newVertexIndex + 2, newVertexIndex + 1));
          tris[i] = QVector3D(newVertexIndex + 0, newVertexIndex + 1, newVertexIndex + 2);
      }
  }
  // spherize vertices
  for (vector<QVector3D>::iterator i = vertices.begin(); i != vertices.end(); ++i)
  {
    *i = (*i).normalized() * radius;
  }
  // Initialize model
  model->initialize(vertices, tris);
  return true;
}

bool Model_Loader_Primitive::loadBoxIntoModel(Model* model, float sizeX, float sizeY, float sizeZ)
{
  if (model == NULL)
  {
    return false;
  }
  sizeX = sizeX/2;
  sizeY = sizeY/2;
  sizeZ = sizeZ/2;
  vector<QVector3D> vertices = vector<QVector3D>({
                                                   {-sizeX, -sizeY, -sizeZ},
                                                   {+sizeX, -sizeY, -sizeZ},
                                                   {-sizeX, -sizeY, +sizeZ},
                                                   {+sizeX, -sizeY, +sizeZ},
                                                   {-sizeX, +sizeY, -sizeZ},
                                                   {+sizeX, +sizeY, -sizeZ},
                                                   {-sizeX, +sizeY, +sizeZ},
                                                   {+sizeX, +sizeY, +sizeZ}
                                                 });
  vector<QVector3D> tris = vector<QVector3D>({
                                                {0, 1, 2}, {3, 2, 1}, // y-neg
                                                {4, 6, 5}, {7, 5, 6}, // y-pos
                                                {4, 0, 6}, {2, 6, 0}, // x-neg
                                                {5, 7, 1}, {3, 1, 7}, // x-pos
                                                {4, 5, 0}, {1, 0, 5}, // z-neg
                                                {6, 2, 7}, {3, 7, 2}  // z-pos
                                             });
  model->initialize(vertices, tris);
  return true;
}
