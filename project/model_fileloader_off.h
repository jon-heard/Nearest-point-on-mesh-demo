#ifndef MODEL_FILELOADER_OFF_H
#define MODEL_FILELOADER_OFF_H

#include <string>
#include <vector>
#include <utility>
#include <QVector3D>

class Model;

class Model_FileLoader_OFF
{
  public:
    bool loadFileIntoModel(Model* model, std::string filename);
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

#endif // MODEL_FILELOADER_OFF_H
