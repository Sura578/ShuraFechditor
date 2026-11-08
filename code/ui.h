#ifndef UI_H
#define UI_H
#include <string>
class ui
{
public:
    void menu();
private:
    void saludations();
    void profilestatusok();
    void profilestatuserror();
    void errorimage();
    void coloroptions();
    std::string lookingforimage();
    void profileasker();
    void finisher();
    void preview();
    std::string imagePath;
    //imageselected are the different color options saved as vector
    int selectedColors[4] = {0, 0, 0, 0};
};
#endif
