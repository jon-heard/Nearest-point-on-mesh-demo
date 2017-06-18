#ifndef MODELLOADER_FILE_OFF_H
#define MODELLOADER_FILE_OFF_H

// ModelLoader_File_Off - Provides functionality to load meshes from OFF files into model objects

#include <string>
#include <vector>
#include <utility> // std::pair
#include <QVector3D>

class Model;

class ModelLoader_File_OFF
{
  public:
    // Loads the mesh data from the given OFF file, and puts it into the given model
    bool loadFileIntoModel(Model* model, std::string filename);
    // Accessors for data on any error that occurs
    std::string getErrorFilename();
    std::string getErrorMessage();
  protected:
    bool loadFileIntoString(std::string filename, std::string& result);
    bool stripCommentsFromString(std::string source,
                                 std::string& destination,
                                 std::vector<std::pair<std::string,std::string>> comments);
    bool tokenizeString(std::string toTokenize, std::vector<std::string>& result);
    bool parseTokens(std::vector<std::string> tokens,
                     std::vector<QVector3D>& vertices,
                     std::vector<QVector3D>& tris);
    std::string errorFilename;
    std::string errorMessage;
};

#endif // MODELLOADER_FILE_OFF_H
