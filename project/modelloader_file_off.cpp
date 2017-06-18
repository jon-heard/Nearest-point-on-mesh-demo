#include "modelloader_file_off.h"
#include "Model.h"
#include <QVector3D>
#include <sstream>
#include <QFile>
#include <QDebug>

using namespace std;

const char* FILELOADER_OFF_WHITESPACE = " \t\n\r";
const vector<pair<string,string>> FILELOADER_OFF_COMMENTS = {{"#", "\n"}};


bool ModelLoader_File_OFF::loadFileIntoModel(Model* model, string filename)
{
  this->errorFilename = filename;
  // Input check
  if (model == NULL)
  {
    this->errorMessage = "Invalid input: model";
    return false;
  }
  // Load file
  string rawContents;
  if (!loadFileIntoString(filename, rawContents))
  {
    this->errorMessage = "Unable to open file";
    return false;
  }
  // Strip comments
  string decommentedContents;
  if (!stripCommentsFromString(rawContents, decommentedContents, FILELOADER_OFF_COMMENTS))
  {
    this->errorMessage = "Unable to decomment file";
    return false;
  }
  // Tokenize
  vector<string> tokens;
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

string ModelLoader_File_OFF::getErrorFilename() { return this->errorFilename; }
string ModelLoader_File_OFF::getErrorMessage() { return this->errorMessage; }

bool ModelLoader_File_OFF::loadFileIntoString(string filename, string& result)
{
  QFile mFile(QString(filename.c_str()));
  if(!mFile.open(QFile::ReadOnly | QFile::Text))
  {
    return false;
  }
  QTextStream in(&mFile);
  result = in.readAll().toStdString();
  return true;
}

bool ModelLoader_File_OFF::stripCommentsFromString(
    string source, string& destination, vector<pair<string,string>> comments)
{
  stringstream destinationStream;
  // Run through source, char by char
  for (unsigned int i = 0; i < source.size(); ++i)
  {
    bool foundComment = false;
    // Run through list of comment types, checking for each
    for (vector<pair<string,string>>::iterator j = comments.begin(); j != comments.end(); ++j)
    {
      if (source.substr(i, (*j).first.size())  == (*j).first)
      {
        do {
          ++i;
        } while (source.substr(i, (*j).second.size()) != (*j).second);
        for (unsigned int k = 0; k < (*j).second.size()-1; ++k)
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
      destinationStream << source[i];
    }
  }
  destination = destinationStream.str();
  return true;
}

bool ModelLoader_File_OFF::tokenizeString(string toTokenize, vector<string>& result)
{
  char* sourceBuffer = new char[toTokenize.size()+1];
  strcpy(sourceBuffer, toTokenize.c_str());
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
    vector<string> tokens, vector<QVector3D>& vertices, vector<QVector3D>& tris)
{
  // Confirm header tag
  if (tokens[0] != "OFF")
  {
    return false;
  }
  // Get item counts
  unsigned int vertexCount = atoi(tokens[1].c_str());
  unsigned int faceCount = atoi(tokens[2].c_str());
  if (vertexCount == 0 || faceCount == 0)
  {
    return false;
  }
  // Load vertex data
  unsigned int tokenIndex = 4;
  while (vertexCount > 0)
  {
    float x = atof(tokens[tokenIndex+0].c_str());
    float y = atof(tokens[tokenIndex+1].c_str());
    float z = atof(tokens[tokenIndex+2].c_str());
    vertices.push_back({x, y, z});
    tokenIndex += 3;
    --vertexCount;
  }
  // Load face data
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
  return true;
}
