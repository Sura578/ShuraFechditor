#include "palette.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace
{
std::string trim(const std::string& text)
{
    const std::string whitespace = " \t\r\n";
    const std::size_t first = text.find_first_not_of(whitespace);
    if (first == std::string::npos)
        return "";
    const std::size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

bool parseHex(const std::string& hex, int& r, int& g, int& b)
{
    if (hex.size() != 7 || hex[0] != '#')
        return false;

    for (std::size_t i = 1; i < hex.size(); ++i)
    {
        if (!std::isxdigit(static_cast<unsigned char>(hex[i])))
            return false;
    }

    r = std::stoi(hex.substr(1, 2), nullptr, 16);
    g = std::stoi(hex.substr(3, 2), nullptr, 16);
    b = std::stoi(hex.substr(5, 2), nullptr, 16);
    return true;
}
}

bool Palette::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    colors.clear();

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream stream(line);
        ColorOption color;
        if (!(stream >> color.index >> color.name >> color.hex))
            continue;

        int r = 0, g = 0, b = 0;
        if (!parseHex(color.hex, r, g, b))
            continue;

        colors.push_back(color);
    }

    return !colors.empty();
}

bool Palette::empty() const
{
    return colors.empty();
}

int Palette::size() const
{
    return static_cast<int>(colors.size());
}

const ColorOption& Palette::at(int index) const
{
    return colors.at(static_cast<std::size_t>(index));
}

std::string Palette::ansi(int index) const
{
    int r = 0, g = 0, b = 0;
    if (!parseHex(at(index).hex, r, g, b))
        return "";

    return "\x1B[38;2;" + std::to_string(r) + ";" +
           std::to_string(g) + ";" + std::to_string(b) + "m";
}
