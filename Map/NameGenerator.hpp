#pragma once
#include <string>
#include <vector>
#include <random>
#include <unordered_set>

class NameGenerator
{
public:
    static std::string GetRandomName();
    static void ResetMemory();

private:
    static const std::vector<std::string> malePrefixes;
    static const std::vector<std::string> maleCores;
    static const std::vector<std::string> maleSuffixes;

    static const std::vector<std::string> femalePrefixes;
    static const std::vector<std::string> femaleCores;
    static const std::vector<std::string> femaleSuffixes;

    static const std::vector<std::string> neutralSuffixes;

    static std::unordered_set<std::string> generatedNames;
};