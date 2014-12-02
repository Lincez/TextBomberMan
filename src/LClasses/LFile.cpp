#include "../../include/LClasses/LClasses.h"

LTextInputStream* LFile::getTextInput()
{
    LFileText* f = new LFileText(getFileName());
    return f->getTextInputStream();
}
LTextOutputStream* LFile::getTextOutput()
{
    LFileText* f = new LFileText(getFileName(), true);
    return f->getTextOutputStream();
}
LOutputStream* LFile::getOutput()
{
    return (new LFileByte(getFileName()))->getOutputStream();
}
LInputStream* LFile::getInput()
{
    return (new LFileByte(getFileName()))->getInputStream();
}
