#include "model_fileloader_off.h"
#include "Model.h"
#include <QVector3D>
#include <sstream>
#include <QFile>
#include <QDebug>

using namespace std;

const char* DELIMETERS = " \t\n\r";

bool Model_FileLoader_OFF::loadFileIntoModel(Model* model, string filename)
{
  this->errorFilename = filename;
  if (model == NULL)
  {
    errorMessage = "Invalid model input";
    return false;
  }
  vector<string> tokens;
  if (!loadFileIntoTokens(filename, tokens))
  {
    return false;
  }
  vector<QVector3D> vertices;
  vector<QVector3D> tris;
  if (!parseTokens(tokens, vertices, tris))
  {
    return false;
  }
  model->initialize(vertices, tris);
  return true;
}

string Model_FileLoader_OFF::getErrorFilename() { return this->errorFilename; }
string Model_FileLoader_OFF::getErrorMessage() { return this->errorMessage; }

bool Model_FileLoader_OFF::loadFileIntoTokens(std::string filename,
                                              std::vector<std::string>& result)
{
  QFile mFile(QString(filename.c_str()));
  if(!mFile.open(QFile::ReadOnly | QFile::Text)){
    this->errorMessage = "Unable to open file";
    return false;
  }
  QTextStream in(&mFile);
  QString rawContent = in.readAll();
  mFile.close();
  string content;
  if (!stripComments(rawContent.toStdString(), content, {{"#", "\n"}}))
  {
    return false;
  }
  if (!tokenize(content, result))
  {
    return false;
  }
  return true;
}

bool Model_FileLoader_OFF::stripComments(string source,
                                         string& destination,
                                         vector<pair<string,string>> comments)
{
  stringstream destinationStream;
  for (int i = 0; i < source.size(); ++i)
  {
    bool foundComment = false;
    for (vector<pair<string,string>>::iterator j = comments.begin(); j != comments.end(); ++j)
    {
      if (source.substr(i, (*j).first.size())  == (*j).first)
      {
        do {
          ++i;
        } while (source.substr(i, (*j).second.size()) != (*j).second);
        for (int k = 0; k < (*j).second.size()-1; ++k)
        {
          ++i;
        }
        foundComment = true;
        break;
      }
    }
    if (!foundComment)
    {
      destinationStream << source[i];
    }
  }
  destination = destinationStream.str();
  return true;
}

bool Model_FileLoader_OFF::tokenize(string toTokenize, vector<string>& result)
{
  char* sourceBuffer = new char[toTokenize.size()+1];
  strcpy(sourceBuffer, toTokenize.c_str());
  char* token = strtok(sourceBuffer, " \t\n\r");
  while (token)
  {
    result.push_back(token);
    token = strtok(NULL, " \t\n\r");
  };
  delete sourceBuffer;
  return true;
}

bool Model_FileLoader_OFF::parseTokens(vector<string> tokens,
                                       vector<QVector3D>& vertices,
                                       vector<QVector3D>& tris)
{
  // Run tokens
  if (tokens[0] != "OFF")
  {
    this->errorMessage = "Incorrect file format";
    return false;
  }
  int vertexCount = atoi(tokens[1].c_str());
  int faceCount = atoi(tokens[2].c_str());
  if (vertexCount == 0 || faceCount == 0)
  {
    this->errorMessage = "Incorrect file format";
    return false;
  }
  int tokenIndex = 4;
  while (vertexCount > 0)
  {
    float x = atof(tokens[tokenIndex+0].c_str());
    float y = atof(tokens[tokenIndex+1].c_str());
    float z = atof(tokens[tokenIndex+2].c_str());
    vertices.push_back({x, y, z});
    tokenIndex += 3;
    --vertexCount;
  }
  while (faceCount > 0)
  {
    float vertexCount = atof(tokens[tokenIndex].c_str());
    if (vertexCount == 3)
    {
      float v1 = atof(tokens[tokenIndex+1].c_str());
      float v2 = atof(tokens[tokenIndex+2].c_str());
      float v3 = atof(tokens[tokenIndex+3].c_str());
      tris.push_back({v1, v2, v3});
      tokenIndex += 4;
    }
    else if (vertexCount == 4)
    {
      float v1 = atof(tokens[tokenIndex+1].c_str());
      float v2 = atof(tokens[tokenIndex+2].c_str());
      float v3 = atof(tokens[tokenIndex+3].c_str());
      tris.push_back({v1, v2, v3});
      v1 = atof(tokens[tokenIndex+1].c_str());
      v2 = atof(tokens[tokenIndex+3].c_str());
      v3 = atof(tokens[tokenIndex+4].c_str());
      tris.push_back({v1, v2, v3});
      tokenIndex += 5;
    }
    --faceCount;
  }

//  vertices = vector<QVector3D>({
//                                 {-1, -1, -1},
//                                 {1, -1, -1},
//                                 {-1, -1, 1},
//                                 {1, -1, 1},
//                                 {-1, 1, -1},
//                                 {1, 1, -1},
//                                 {-1, 1, 1},
//                                 {1, 1, 1}
//                               });
//  tris = vector<QVector3D>({
//                              {0, 1, 2}, {3, 2, 1}, // y-neg
//                              {4, 6, 5}, {7, 5, 6}, // y-pos
//                              {4, 0, 6}, {2, 6, 0}, // x-neg
//                              {5, 7, 1}, {3, 1, 7}, // x-pos
//                              {4, 5, 0}, {1, 0, 5}, // z-neg
//                              {6, 2, 7}, {3, 7, 2}  // z-pos
//                           });
  return true;
}
