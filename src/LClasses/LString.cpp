#include "../../include/LClasses/LClasses.h"

LString::LString()
{

}
LString::~LString()
{
    s.clear();
}
LString::LString(const LString& str)
{
    s = str.s;
}
LString::LString(const char* c)
{
    s = c;
}
LString::LString(int i)
{
    char* c = new char[11];
    itoa(i, c, 10);
    s = c;
    delete c;
}
LString& LString::operator= (const LString& str)
{
    s = str.s;
    return *this;
}
LString& LString::operator= (const char* c)
{
    s = c;
    return *this;
}
LString& LString::operator= (char c)
{
    s = c;
    return *this;
}
LString& LString::operator= (int i)
{
    char* c = new char[11];
    itoa(i, c, 10);
    s = c;
    delete c;
    return *this;
}
LString& LString::operator= (float f) // GAMBIARRA!!!
{
    FILE* file = fopen("floatToLString.txt", "w");
    fprintf(file, "%f", f);
    fclose(file);
    file = fopen("floatToLString.txt", "r");
    char *c = new char[20];
    fscanf(file, c);
    fclose(file);
    s = c;
    delete c;
    return *this;
}
size_t LString::size()
{
    return s.size();
}
size_t LString::length()
{
    return s.length();
}
void LString::clear()
{
    s.clear();
}
const char LString::at (size_t pos)
{
    return this->c_str()[pos];
}
LString& LString::operator+= (const LString& str)
{
    s+=str.s;
    return *this;
}
LString& LString::operator+= (const LString* str)
{
    s+=str->s;
    return *this;
}
LString& LString::operator+= (const char* c)
{
    s+=c;
    return *this;
}
LString& LString::operator+= (char c)
{
    s+=c;
    return *this;
}
LString& LString::operator+= (int i)
{
    char* c = new char[11];
    itoa(i, c, 10);
    s+=c;
    delete c;
    return *this;
}
LString LString::operator+(LString s)
{
    LString rt;
    rt = (*this);
    rt += s;
    return rt;
}
LString* LString::operator+ (LString*s)
{
    LString*rt = new LString(this->c_str());
    (*rt) += s->c_str();
    return rt;
}
const char* LString::c_str()
{
    return s.c_str();
}
bool LString::startsWith(const char* cs)
{
    LString temp = cs;
    if (temp.size() == 0) return true;
    LString *tempThis;
    tempThis = this->substr(0, temp.size());
    bool b = tempThis->equals(&temp);
    delete tempThis;
    return b;
}
bool LString::startsWith(LString *s)
{
    return this->startsWith(s->c_str());
}
bool LString::endsWith(LString*s_)
{
    return this->endsWith(s_->c_str());
}
bool LString::endsWith(const char* cs)
{
    LString temp = cs;
    if (temp.size() == 0) return false;
    LString* tempThis;
    tempThis = this->substr(this->size() - temp.size(), this->size());
    bool b = tempThis->equals(&temp);
    delete tempThis;
    return b;
}
void LString::print()
{
    printf("%s", s.c_str());
}

char* LString::toString()
{
    return (char*)this->c_str();
}
bool LString::equals(LString*o)
{
    if (strcmp(this->c_str(), o->c_str()) == 0) return true;
    return false;
}
bool LString::equals(const char*s)
{
    if (strcmp(this->c_str(), s) == 0) return true;
    return false;
}

LString operator +  (const char* a, LString b)
{
    LString rt = a;
    rt += b;
    return rt;
}
//char* operator +  (const char* a, LString b) { char*r = new char[strlen(a)+strlen(b.c_str())+1];  strcpy(r,a); strcat(r, b.c_str());  return r; }
void printf(LString*s) {printf(s->c_str());}
void printf(LException* e) {printf("%s\n%s\n", e->getMsg(), e->details.c_str());}

