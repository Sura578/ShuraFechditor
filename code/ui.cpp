#include "ui.h"
#include <iostream>

//ANSI escape code to reset the terminal color
static const char* const RESET = "\x1B[0m";

//check profile and select the profile number
void ui::saludations()
{
    std::cout << "Verifying profile color status\n";
    if (!profileMaker.profilechecker())
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
    if (!profileMaker.profilesToRemove().empty())
    {
        std::cout << "Some profiles are missing or invalid: ";
        for (int number : profileMaker.profilesToRemove())
            std::cout << '#' << number << ' ';
        std::cout << "\nThey will be removed and the rest renumbered.\n";
    }
    else
    {
        std::cout << "Problems were repaired automatically (a sample profile was created if needed).\n";
    }
    std::cout << "Press Enter to continue (Ctrl+C to fix manually)...\n";
    std::cin.get();

    // The user accepts the automatic repair.
    profileMaker.profileresolver();
}

//is going to ask for an image
std::string ui::lookingforimage()
{
    std::string path;
    while (true)
    {
        std::cout << "Please select the image you want to use (valid .png):\n";
        std::getline(std::cin>>std::ws, path);
        if (profileMaker.imagechecker(path))
            return path;
        errorimage();
    }
}

//if not its going to show an error message and ask for a new one
void ui::errorimage()
{
    std::cout << "Image is not a valid PNG (wrong path or signature).\n Please try again.\n";
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
            profileMaker.setImagePath(imagePath);
            profileMaker.setSelectedColors(selectedColors);
            if (profileMaker.profilemaker())
                std::cout << "\nNew profile documents created\n";
            else
                error();
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

void ui::error()
{
    std::cout << "Error: the profile could not be saved. The previous files were kept.\n";
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
