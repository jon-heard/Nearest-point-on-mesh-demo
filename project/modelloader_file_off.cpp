#include "modelloader_file_off.h"
#include "Model.h"
#include <QVector3D>
#include <sstream>
#include <QFile>
#include <QDebug>

using namespace std;

const char* FILELOADER_OFF_WHITESPACE = " \t\n\r";
const vector<pair<QString,QString>> FILELOADER_OFF_COMMENTS = {{"#", "\n"}};


bool ModelLoader_File_OFF::loadFileIntoModel(Model* model, QString filename)
{
  this->errorFilename = filename;
  // Input check
  if (model == NULL)
  {
    this->errorMessage = "Invalid input: model";
    return false;
  }
  // Load file
  QString rawContents;
  if (!loadFileIntoString(filename, rawContents))
  {
    this->errorMessage = "Unable to open file";
    return false;
  }
  // Strip comments
  QString decommentedContents;
  if (!stripCommentsFromString(rawContents, decommentedContents, FILELOADER_OFF_COMMENTS))
  {
    this->errorMessage = "Unable to decomment file";
    return false;
  }
  // Tokenize
  vector<QString> tokens;
  if (!tokenizeString(decommentedContents, tokens))
  {
    this->errorMessage = "Unable to tokenize file: incorrect format is likely";
    return false;
  }
  // Parse
  vector<QVector3D> vertices;
  vector<QVector3D> tris;
  if (!parseTokens(tokens, vertices, tris))
  {
    this->errorMessage = "Unable to parse file";
    return false;
  }
  // Initialize model
  model->initialize(vertices, tris);
  return true;
}

QString ModelLoader_File_OFF::getErrorFilename() { return this->errorFilename; }
QString ModelLoader_File_OFF::getErrorMessage() { return this->errorMessage; }

bool ModelLoader_File_OFF::loadFileIntoString(QString filename, QString& result)
{
  QFile mFile(filename);
  if(!mFile.open(QFile::ReadOnly | QFile::Text))
  {
    return false;
  }
  QTextStream in(&mFile);
  result = in.readAll();
  return true;
}

bool ModelLoader_File_OFF::stripCommentsFromString(QString source, QString& destination, std::vector<std::pair<QString, QString> > comments)
{
  stringstream destinationStream;
  // Run through source, char by char
  for (int i = 0; i < source.size(); ++i)
  {
    bool foundComment = false;
    // Run through list of comment types, checking for each
    for (vector<pair<QString,QString>>::iterator j = comments.begin(); j != comments.end(); ++j)
    {
      if (source.mid(i, (*j).first.size())  == (*j).first)
      {
        do {
          ++i;
        } while (source.mid(i, (*j).second.size()) != (*j).second);
        for (int k = 0; k < (*j).second.size()-1; ++k)
        {
          ++i;
        }
        foundComment = true;
        break;
      }
    }
    // Copy char from source to destination (unless end of comment)
    if (!foundComment)
    {
      destinationStream << source.at(i).toLatin1();
    }
  }
  destination = destinationStream.str().c_str();
  return true;
}

bool ModelLoader_File_OFF::tokenizeString(QString toTokenize, vector<QString>& result)
{
  char* sourceBuffer = new char[toTokenize.size()+1];
  strcpy(sourceBuffer, toTokenize.toStdString().c_str());
  char* token = strtok(sourceBuffer, FILELOADER_OFF_WHITESPACE);
  while (token)
  {
    result.push_back(token);
    token = strtok(NULL, FILELOADER_OFF_WHITESPACE);
  };
  delete sourceBuffer;
  return true;
}

bool ModelLoader_File_OFF::parseTokens(
    vector<QString> tokens, vector<QVector3D>& vertices, vector<QVector3D>& tris)
{
  // Confirm header tag
  if (tokens[0] != "OFF")
  {
    return false;
  }
  // Get item counts
  unsigned int vertexCount = atoi(tokens[1].toStdString().c_str());
  unsigned int faceCount = atoi(tokens[2].toStdString().c_str());
  if (vertexCount == 0 || faceCount == 0)
  {
    return false;
  }
  // Load vertex data
  unsigned int tokenIndex = 4;
  while (vertexCount > 0)
  {
    float x = atof(tokens[tokenIndex+0].toStdString().c_str());
    float y = atof(tokens[tokenIndex+1].toStdString().c_str());
    float z = atof(tokens[tokenIndex+2].toStdString().c_str());
    vertices.push_back({x, y, z});
    tokenIndex += 3;
    --vertexCount;
  }
  // Load face data
  while (faceCount > 0)
  {
    int vertexCount = atoi(tokens[tokenIndex].toStdString().c_str());
    for (int i = 1; i < vertexCount-1; ++i)
    {
      float v1 = atof(tokens[tokenIndex+1].toStdString().c_str());
      float v2 = atof(tokens[tokenIndex+i+1].toStdString().c_str());
      float v3 = atof(tokens[tokenIndex+i+2].toStdString().c_str());
      tris.push_back({v1, v2, v3});
    }
    tokenIndex += vertexCount + 1;
    --faceCount;
  }
  return true;
}
