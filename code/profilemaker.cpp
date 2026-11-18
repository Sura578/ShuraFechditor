#include "profilemaker.h"

#include "default_image.h"
#include "palette.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
    // Remove leading/trailing whitespace so malformed spacing does not break parsing.
    std::string trim(const std::string& text)
    {
        const std::string whitespace = " \t\r\n";
        const std::size_t first = text.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return "";
        const std::size_t last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
    }

    std::string toLower(std::string text)
    {
        for (char& character : text)
            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        return text;
    }

    bool endsWithPng(const std::string& name)
    {
        return name.size() > 4 && name.substr(name.size() - 4) == ".png";
    }
} // namespace

// 89 50 4E 47 0D 0A 1A 0A = PNG signature.
constexpr unsigned char kPngSignature[8] =
{
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
};

// ===== configuration =====

void maker::setConfigPath(const std::filesystem::path& path)
{
    configPath_ = path;
}

void maker::setProfilesDirectory(const std::filesystem::path& directory)
{
    profilesDirectory_ = directory;
}

void maker::setPalettePath(const std::filesystem::path& path)
{
    palettePath_ = path;
}

void maker::setImagePath(const std::filesystem::path& path)
{
    imagePath_ = path;
}

void maker::setSelectedColors(const int colors[kColorGroupCount])
{
    for (int group = 0; group < kColorGroupCount; ++group)
        selectedColors_[group] = colors[group];
}

const std::filesystem::path& maker::configPath() const
{
    return configPath_;
}

const std::filesystem::path& maker::profilesDirectory() const
{
    return profilesDirectory_;
}

int maker::profileCount() const
{
    return profilecounter;
}

int maker::nextProfileNumber() const
{
    return profilecounter + 1;
}

const std::vector<int>& maker::profilesToRemove() const
{
    return profiletoremove;
}

std::filesystem::path maker::imagePathFor(int number) const
{
    return profilesDirectory_ / (std::to_string(number) + ".png");
}

// ===== validation =====

bool maker::isValidHexColor(const std::string& text)
{
    if (text.size() != 7 || text[0] != '#')
        return false;
    for (std::size_t index = 1; index < text.size(); ++index)
    {
        if (!std::isxdigit(static_cast<unsigned char>(text[index])))
            return false;
    }
    return true;
}

bool maker::imagechecker(const std::filesystem::path& imagePath) const
{
    std::ifstream file(imagePath, std::ios::binary);
    if (!file.is_open())
        return false;

    char buffer[8] = {};
    file.read(buffer, static_cast<std::streamsize>(sizeof(buffer)));
    if (file.gcount() != static_cast<std::streamsize>(sizeof(buffer)))
        return false;

    for (std::size_t index = 0; index < sizeof(buffer); ++index)
    {
        if (static_cast<unsigned char>(buffer[index]) != kPngSignature[index])
            return false;
    }
    return true;
}

// Reads colores.conf, validating colors and their PNG files.
// `invalid` receives the profile numbers that must be repaired/removed;
// -1 means a line that could not be identified with a profile number.
bool maker::readProfiles(std::vector<Profile>& profiles, std::vector<int>& invalid)
{
    profiles.clear();
    invalid.clear();
    profilecounter = 0;

    std::ifstream file(configPath_);
    if (!file.is_open())
        return false;

    std::string line;
    std::vector<int> seen;
    while (std::getline(file, line))
    {
        const std::string text = trim(line);
        if (text.empty() || text[0] == '#')
            continue;

        std::istringstream stream(text);
        Profile profile;
        if (!(stream >> profile.number))
        {
            invalid.push_back(-1);
            continue;
        }
        profilecounter = std::max(profilecounter, profile.number);

        bool valid = profile.number > 0;
        for (int group = 0; group < kColorGroupCount && valid; ++group)
        {
            std::string color;
            if (!(stream >> color) || !isValidHexColor(color))
                valid = false;
            else
                profile.colors[group] = toLower(color);
        }
        if (!valid)
        {
            invalid.push_back(profile.number > 0 ? profile.number : -1);
            continue;
        }
        if (std::find(seen.begin(), seen.end(), profile.number) != seen.end())
        {
            invalid.push_back(profile.number); // duplicated profile line
            continue;
        }
        seen.push_back(profile.number);

        if (!imagechecker(imagePathFor(profile.number)))
        {
            invalid.push_back(profile.number); // missing or corrupted logo
            continue;
        }
        profiles.push_back(profile);
    }
    return true;
}

bool maker::profilechecker()
{
    profiletoremove.clear();

    std::error_code error;
    if (std::filesystem::exists(recoveryFlagPath(), error) && !error)
        profilesavior(); // an interrupted operation left a backup behind

    std::vector<Profile> profiles;
    std::vector<int> invalid;
    if (!readProfiles(profiles, invalid))
    {
        profilegenesis();
        return false;
    }

    profiletoremove = invalid;
    if (profiles.empty())
    {
        // Nothing usable: seed a sample profile instead of leaving an empty config.
        profilegenesis();
        profiletoremove.clear();
        return false;
    }
    return profiletoremove.empty();
}

// ===== writing =====

bool maker::writeProfiles(const std::vector<Profile>& profiles) const
{
    std::filesystem::perms permissions =
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write |
        std::filesystem::perms::group_read | std::filesystem::perms::others_read;
    std::error_code error;
    if (std::filesystem::exists(configPath_, error) && !error)
        permissions = std::filesystem::status(configPath_, error).permissions();
    if (error)
        return false;

    const std::filesystem::path temporary = configPath_.string() + ".tmp";
    std::ofstream file(temporary, std::ios::trunc);
    if (!file.is_open())
        return false;

    file << "# imagen | grupo1(OS,kernel,pkg,shell) | grupo2(WM,lm,icons,term) | "
            "grupo3(host,cpu,gpu,disk,mem,uptime) | grupo4(sound,player,media)\n";
    for (const Profile& profile : profiles)
    {
        file << profile.number;
        for (const std::string& color : profile.colors)
            file << "  " << color;
        file << '\n';
    }
    file.close();
    if (!file)
        return false;

    error.clear();
    std::filesystem::rename(temporary, configPath_, error);
    if (error)
    {
        std::filesystem::remove(temporary, error);
        return false;
    }
    std::filesystem::permissions(configPath_, permissions, error);
    return !error;
}

bool maker::renameImages(const std::vector<int>& oldNumbers,
                         const std::vector<int>& newNumbers) const
{
    if (oldNumbers.size() != newNumbers.size())
        return false;

    // newNumber <= oldNumber always, so ascending order never overwrites a
    // source file that is still needed.
    for (std::size_t index = 0; index < oldNumbers.size(); ++index)
    {
        if (oldNumbers[index] == newNumbers[index])
            continue;
        std::error_code error;
        std::filesystem::rename(imagePathFor(oldNumbers[index]),
                                imagePathFor(newNumbers[index]), error);
        if (error)
            return false;
    }
    return true;
}

// Writes the new config and renames/deletes the images as a single operation.
bool maker::commitProfiles(std::vector<Profile> profiles,
                           const std::vector<int>& removedNumbers)
{
    if (!createBackup())
        return false;

    bool success = true;
    for (int number : removedNumbers)
    {
        if (number <= 0)
            continue;
        // A duplicated line may report a number that is still valid.
        const bool stillKept = std::any_of(profiles.begin(), profiles.end(),
            [number](const Profile& profile) { return profile.number == number; });
        if (stillKept)
            continue;
        std::error_code error;
        const std::filesystem::path image = imagePathFor(number);
        if (std::filesystem::exists(image, error) && !error)
        {
            std::filesystem::remove(image, error);
            if (error)
            {
                success = false;
                break;
            }
        }
    }

    std::sort(profiles.begin(), profiles.end(),
        [](const Profile& left, const Profile& right) { return left.number < right.number; });

    std::vector<int> oldNumbers;
    std::vector<int> newNumbers;
    for (std::size_t index = 0; index < profiles.size(); ++index)
    {
        oldNumbers.push_back(profiles[index].number);
        profiles[index].number = static_cast<int>(index) + 1;
        newNumbers.push_back(profiles[index].number);
    }

    if (success)
        success = writeProfiles(profiles);
    if (success)
        success = renameImages(oldNumbers, newNumbers);

    if (!success)
    {
        restoreBackup();
        return false;
    }
    clearBackup();
    profilecounter = static_cast<int>(profiles.size());
    profiletoremove.clear();
    return true;
}

// ===== creation / removal =====

maker::Colors maker::defaultColors()
{
    return Colors {"#8dbfca", "#f9a2c5", "#eec2d6", "#96cdd9"};
}

maker::Colors maker::colorsFromPalette() const
{
    Colors result = defaultColors();
    Palette palette;
    const std::vector<std::filesystem::path> candidates =
    {
        palettePath_,
        std::filesystem::path("..") / palettePath_,
        std::filesystem::path("code") / palettePath_
    };

    for (const std::filesystem::path& candidate : candidates)
    {
        if (!palette.load(candidate.string()))
            continue;
        for (int group = 0; group < kColorGroupCount; ++group)
        {
            if (selectedColors_[group] >= 0 && selectedColors_[group] < palette.size())
                result[group] = toLower(palette.at(selectedColors_[group]).hex);
        }
        break;
    }
    return result;
}

bool maker::profilemaker()
{
    if (imagePath_.empty() || !imagechecker(imagePath_))
        return false;

    std::vector<Profile> profiles;
    std::vector<int> invalid;
    if (!readProfiles(profiles, invalid))
        return false;

    const int number = nextProfileNumber();
    const std::filesystem::path destination = imagePathFor(number);
    if (!createBackup())
        return false;

    bool success = true;
    std::error_code error;
    std::filesystem::create_directories(profilesDirectory_, error);
    if (error)
        success = false;

    if (success)
    {
        std::filesystem::copy_file(imagePath_, destination,
            std::filesystem::copy_options::overwrite_existing, error);
        if (error)
            success = false;
    }

    if (success)
    {
        Profile added;
        added.number = number;
        added.colors = colorsFromPalette();
        profiles.push_back(added);
        std::sort(profiles.begin(), profiles.end(),
            [](const Profile& left, const Profile& right) { return left.number < right.number; });
        success = writeProfiles(profiles);
    }

    if (!success)
    {
        std::filesystem::remove(destination, error);
        restoreBackup();
        return false;
    }

    clearBackup();
    profilecounter = number;
    profiletoremove.clear();
    return true;
}

void maker::profileresolver()
{
    if (profiletoremove.empty())
        return;

    std::vector<Profile> profiles;
    std::vector<int> invalid;
    if (!readProfiles(profiles, invalid))
        return;
    commitProfiles(profiles, profiletoremove);
}

bool maker::profileremover(int profileNumber)
{
    if (profileNumber <= 0)
        return false;

    std::vector<Profile> profiles;
    std::vector<int> invalid;
    if (!readProfiles(profiles, invalid))
        return false;

    const auto found = std::find_if(profiles.begin(), profiles.end(),
        [profileNumber](const Profile& profile) { return profile.number == profileNumber; });
    if (found == profiles.end())
        return false;

    profiles.erase(found);
    return commitProfiles(profiles, std::vector<int> {profileNumber});
}

// ===== sample profile / recovery =====

// Writes the real 1.png straight from the bytes embedded in the executable
// (default_image.h). No external source file is needed at runtime.
bool maker::restoreSampleImage(const std::filesystem::path& destination) const
{
    std::ofstream file(destination, std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return false;

    file.write(reinterpret_cast<const char*>(default_image::kData),
               static_cast<std::streamsize>(sizeof(default_image::kData)));
    file.close(); // flush before reading it back
    if (!file)
        return false;

    return imagechecker(destination); // sanity check on what we just wrote
}

bool maker::profilegenesis()
{
    std::error_code error;
    std::filesystem::create_directories(profilesDirectory_, error);
    if (error)
        return false;

    const std::filesystem::path sample = imagePathFor(1);
    if (!imagechecker(sample) && !restoreSampleImage(sample))
        return false;

    std::vector<Profile> profiles;
    std::vector<int> invalid;
    if (!readProfiles(profiles, invalid) || profiles.empty())
    {
        Profile first;
        first.number = 1;
        first.colors = defaultColors();
        if (!writeProfiles(std::vector<Profile> {first}))
            return false;
    }

    profilecounter = std::max(1, profilecounter);
    profiletoremove.clear();
    return true;
}

std::filesystem::path maker::backupConfigPath() const
{
    return configPath_.string() + ".bak";
}

std::filesystem::path maker::backupDirectory() const
{
    return profilesDirectory_.string() + "_bak";
}

std::filesystem::path maker::recoveryFlagPath() const
{
    return configPath_.string() + ".recovering";
}

bool maker::createBackup() const
{
    std::error_code error;
    std::filesystem::remove_all(backupDirectory(), error);
    error.clear();
    std::filesystem::create_directories(backupDirectory(), error);
    if (error)
        return false;

    std::filesystem::copy_file(configPath_, backupConfigPath(),
        std::filesystem::copy_options::overwrite_existing, error);
    if (error)
        return false;

    if (std::filesystem::is_directory(profilesDirectory_, error) && !error)
    {
        for (const auto& entry : std::filesystem::directory_iterator(profilesDirectory_, error))
        {
            if (error)
                break;
            if (!entry.is_regular_file())
                continue;
            const std::string name = entry.path().filename().string();
            if (!endsWithPng(name))
                continue;
            std::filesystem::copy_file(entry.path(), backupDirectory() / name,
                std::filesystem::copy_options::overwrite_existing, error);
            if (error)
                break;
        }
    }
    if (error)
        return false;

    std::ofstream flag(recoveryFlagPath(), std::ios::trunc);
    if (!flag.is_open())
        return false;
    flag << "recovery pending\n";
    return static_cast<bool>(flag);
}

bool maker::restoreBackup() const
{
    bool restored = false;
    std::error_code error;

    if (std::filesystem::exists(backupConfigPath(), error) && !error)
    {
        std::filesystem::copy_file(backupConfigPath(), configPath_,
            std::filesystem::copy_options::overwrite_existing, error);
        if (error)
            return false;
        std::filesystem::remove(backupConfigPath(), error);
        restored = true;
    }

    if (std::filesystem::is_directory(backupDirectory(), error) && !error)
    {
        std::filesystem::create_directories(profilesDirectory_, error);
        if (error)
            return false;

        // Replace the whole image set with the backed up one. Collect the
        // paths first so the directory is not modified while iterating it.
        std::vector<std::filesystem::path> currentImages;
        for (const auto& entry : std::filesystem::directory_iterator(profilesDirectory_, error))
        {
            if (error)
                break;
            if (entry.is_regular_file() && endsWithPng(entry.path().filename().string()))
                currentImages.push_back(entry.path());
        }
        if (error)
            return false;
        for (const std::filesystem::path& image : currentImages)
        {
            std::filesystem::remove(image, error);
            if (error)
                return false;
        }

        for (const auto& entry : std::filesystem::directory_iterator(backupDirectory(), error))
        {
            if (error)
                break;
            if (!entry.is_regular_file())
                continue;
            std::filesystem::copy_file(entry.path(),
                profilesDirectory_ / entry.path().filename(),
                std::filesystem::copy_options::overwrite_existing, error);
            if (error)
                break;
        }
        if (error)
            return false;

        std::filesystem::remove_all(backupDirectory(), error);
        restored = true;
    }

    error.clear();
    std::filesystem::remove(recoveryFlagPath(), error);
    return restored;
}

void maker::clearBackup() const
{
    std::error_code error;
    std::filesystem::remove(backupConfigPath(), error);
    std::filesystem::remove_all(backupDirectory(), error);
    std::filesystem::remove(recoveryFlagPath(), error);
}

bool maker::profilesavior()
{
    const bool restored = restoreBackup();
    if (restored)
        std::cout << "A backup was found after an unexpected close: restoring it as the main profile.\n";
    return restored;
}
