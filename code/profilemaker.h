#ifndef PROFILEMAKER_H
#define PROFILEMAKER_H

#include <array>
#include <filesystem>
#include <string>
#include <vector>

// Manages fastfetch color profiles.
//
// A profile is one line of `colores.conf`:
//     <number> #rrggbb #rrggbb #rrggbb #rrggbb
// and its logo is `<number>.png` inside the profiles directory (`logos/`).
class maker
{
public:
    static constexpr int kColorGroupCount = 4;
    using Colors = std::array<std::string, kColorGroupCount>;

    // File layout and UI-provided input.
    void setConfigPath(const std::filesystem::path& path);
    void setProfilesDirectory(const std::filesystem::path& directory);
    void setPalettePath(const std::filesystem::path& path);
    void setImagePath(const std::filesystem::path& path);
    void setSelectedColors(const int colors[kColorGroupCount]);

    const std::filesystem::path& configPath() const;
    const std::filesystem::path& profilesDirectory() const;
    int profileCount() const;
    int nextProfileNumber() const;
    const std::vector<int>& profilesToRemove() const;

    // True only when the file starts with the PNG magic number.
    bool imagechecker(const std::filesystem::path& imagePath) const;

    // Checks colores.conf against the images. Returns false if something had
    // to be repaired; the offending profile numbers are kept in profilesToRemove().
    bool profilechecker();

    // Deletes the profiles reported by profilechecker() and renumbers the rest.
    void profileresolver();

    // Copies the selected image as <next number>.png and appends its colors.
    bool profilemaker();

    // Removes one profile and renumbers the remaining ones.
    bool profileremover(int profileNumber);

    // Creates a tiny valid sample profile when no usable profile exists.
    bool profilegenesis();

    // Restores the last backup after an interrupted operation.
    bool profilesavior();

private:
    struct Profile
    {
        int number = 0;
        Colors colors;
    };

    static bool isValidHexColor(const std::string& text);
    static Colors defaultColors();

    // Restores <number>.png from the image embedded in the executable.
    bool restoreSampleImage(const std::filesystem::path& destination) const;
    bool readProfiles(std::vector<Profile>& profiles, std::vector<int>& invalid);
    bool writeProfiles(const std::vector<Profile>& profiles) const;
    bool commitProfiles(std::vector<Profile> profiles, const std::vector<int>& removedNumbers);
    bool renameImages(const std::vector<int>& oldNumbers,
                      const std::vector<int>& newNumbers) const;
    Colors colorsFromPalette() const;
    std::filesystem::path imagePathFor(int number) const;

    std::filesystem::path backupConfigPath() const;
    std::filesystem::path backupDirectory() const;
    std::filesystem::path recoveryFlagPath() const;
    bool createBackup() const;
    bool restoreBackup() const;
    void clearBackup() const;

    int profilecounter = 0;
    std::vector<int> profiletoremove;

    std::filesystem::path configPath_ = "colores.conf";
    std::filesystem::path profilesDirectory_ = "logos";
    std::filesystem::path palettePath_ = "color.options.colors";
    std::filesystem::path imagePath_;
    std::array<int, kColorGroupCount> selectedColors_ {0, 0, 0, 0};
};

#endif // PROFILEMAKER_H
