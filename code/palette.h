#ifndef PALETTE_H
#define PALETTE_H

#include <string>
#include <vector>

struct ColorOption
{
    int index = 0;
    std::string name;
    std::string hex;
};

class Palette
{
public:
    bool load(const std::string& path);

    bool empty() const;
    int size() const;

    const ColorOption& at(int index) const;
    std::string ansi(int index) const;

private:
    std::vector<ColorOption> colors;
};

#endif
