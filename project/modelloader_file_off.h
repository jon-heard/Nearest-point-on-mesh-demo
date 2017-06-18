#ifndef MODELLOADER_FILE_OFF_H
#define MODELLOADER_FILE_OFF_H

// ModelLoader_File_Off - Provides functionality to load meshes from OFF files into model objects

#include <vector>
#include <utility> // std::pair
#include <QString>
#include <QVector3D>

class Model;

class ModelLoader_File_OFF
{
  public:
    // Loads the mesh data from the given OFF file, and puts it into the given model
    bool loadFileIntoModel(Model* model, QString filename);
    // Accessors for data on any error that occurs
    QString getErrorFilename();
    QString getErrorMessage();
  protected:
    bool loadFileIntoString(QString filename, QString& result);
    bool stripCommentsFromString(QString source,
                                 QString& destination,
                                 std::vector<std::pair<QString,QString>> comments);
    bool tokenizeString(QString toTokenize, std::vector<QString>& result);
    bool parseTokens(std::vector<QString> tokens,
                     std::vector<QVector3D>& vertices,
                     std::vector<QVector3D>& tris);
    QString errorFilename;
    QString errorMessage;
};

#endif // MODELLOADER_FILE_OFF_H
