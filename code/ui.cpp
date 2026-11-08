#include "ui.h"
#include <iostream>
#define BLACK   "\x1B[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1B[37m"
#define ORANGE  "\x1B[38;2;255;128;0m"
#define ROSE    "\x1B[38;2;255;151;203m"
#define LBLUE   "\x1B[38;2;53;149;240m"
#define LGREEN  "\x1B[38;2;17;245;120m"
#define GRAY    "\x1B[38;2;176;174;174m"
#define PURPLE  "\033[35m"
#define BG_WHITE   "\x1B[47m"
#define RESET   "\x1B[0m"

// ===== STUBS TEMPORALES: borrar cuando exista ProfileMaker =====
bool profilechecker()
{
    return true;

}
bool imagechecker(const std::string& path)
{
    return path.size() > 4 &&
    path.substr(path.size() - 4) == ".png";
}
void profilesaver() {}
// ==============================================================

//check profile and select the profile number
void ui::saludations()
{
    std::cout << "Verifying profile color status\n";
    if (!profilechecker())
    {
        profilestatuserror();
    }
    else
    {
        profilestatusok();
    }
}
//if ok is going to make a new one
void ui::profilestatusok()
{
    std::cout << "Color profiles are OK \n let's make a new one\n";
}

//else is going the select the profile number a make a new one for it
void ui::profilestatuserror()
{
    // TODO: PENDING TO COMPLETE PROFILE CHECKER
    std::cout << "Some profiles are missing or invalid.\n";
    std::cout << "Press Enter any key to continue (Ctrl+C to fix manually)...\n";
    std::cin.get();
    /*
     *    //gets amount and numbers from the profiles and images not matching from the logic;
     *    for(counter=0;counter<amount;counter++)
     *    {
     *    //cout<<"Error in &profile# ";
     *        //if(NO image) cout<<"No image n\"<<;
     *        //cout<<"No color profile n\ Remember to read the manual on fakelink.com to see how to do it n\ Type any key to continue or type ctrl+c if you wanna solve this manually
     *        //cin>>any key to continue;
}
*/
    //if user continue lowest profile number will be replaced
}

//is going to ask for an image
std::string ui::lookingforimage()
{
    std::string path;
    while (true)
    {
        std::cout << "Please select the image you want to use (must be .png):\n";
        std::getline(std::cin>>std::ws, path);
        if (imagechecker(path))   // TODO: imagechecker ProfileMaker
            return path;
        errorimage();
    }
}

//if not its going to show an error message and ask for a new one
void ui::errorimage()
{
    std::cout << "Image is not .png or the address is not correct \n Please try again \n";
}

//its going print the coloroptions and the option number
void ui::coloroptions()
{
    for (int i = 0; i < palette.size(); ++i)
    {
        const ColorOption& color = palette.at(i);
        std::cout << palette.ansi(i)
                  << "Press " << color.index << " for "
                  << color.name << " (" << color.hex << ")\n"
                  << RESET;
    }
}

void ui::profileasker()
{
    //Categories saves the category info and Group is a counter
    static const char* categories[4] =
    {
        "Os info", "Graphic info", "Hardware info", "Multimedia"
    };
    coloroptions();
    for (int group = 0; group < 4; group++)
    {
        int option;
        while (true)
        {
            std::cout << "Profile " << (group + 1)
            << " (" << categories[group]
            << "): choose a color 0-" << (palette.size() - 1) << " \n";
            std::cin >> option;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Invalid input\n";
                continue;
            }
            if (option >= 0 && option < palette.size())
            {
                selectedColors[group] = option;
                break;
            }
            std::cout << "Invalid option, try again \n";
        }
    }
}
void ui::preview()
{
    //Categories saves the category info and Group is a counter
    static const char* categories[4] =
    {
        "Os info", "Graphic info", "Hardware info", "Multimedia"
    };

    std::cout << "\n===== PREVIEW =====\n";
    for (int group = 0; group < 4; group++)
    {
        int chosen = selectedColors[group];
        const ColorOption& color = palette.at(chosen);
        std::cout << palette.ansi(chosen)
                  << categories[group]
                  << " [" << color.name << " " << color.hex << "] sample text\n"
                  << RESET;
    }
    std::cout << "===================\n";
}

void ui::finisher()
{
    bool nani = false;
    char option;
    do
    {
        std::cout<<"Ready to make a new profile? y/Y or N/n \n";
        std::cin>>option;
        if (option == 'y' || option == 'Y')
        {
            profilesaver();
            std::cout<<"\nNew profile documents created\n";
            nani = true;
        }
        else if (option == 'n' || option == 'N')
        {
            std::cout<<"Goodbye, have a nice day\n";
            break;
        }
        else std::cout<<"Invalid input, try again\n";
    }
    while (!nani);
    //user can close in any moment with ctrl c via terminal
}

bool ui::loadpalette()
{
    const char* paths[] = {
        "color.options.colors",
        "../color.options.colors",
        "code/color.options.colors"
    };

    for (const char* path : paths)
    {
        if (palette.load(path))
            return true;
    }
    return false;
}

void ui::menu()
{
    if (!loadpalette())
    {
        std::cout << "Error: could not load color.options.colors\n";
        return;
    }

    saludations();
    imagePath = lookingforimage();
    profileasker();
    preview();
    finisher();
}
