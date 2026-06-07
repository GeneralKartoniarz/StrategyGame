#include "NameGenerator.hpp"

std::unordered_set<std::string> NameGenerator::generatedNames;

const std::vector<std::string> NameGenerator::malePrefixes = {
    "", "Stary ", "Czarny ", "Mroczny ", "Krwawy ", "Głuchy ", "Dziki ", "Wilczy ", "Zimny ", 
    "Mglisty ", "Martwy ", "Suchy ", "Ciemny ", "Ponury ", "Ślepy ", "Przeklęty "
};

const std::vector<std::string> NameGenerator::maleCores = {
    "Las", "Bór", "Szczyt", "Jar", "Brzeg", "Zrąb", "Kurhan", "Zagon", 
    "Wał", "Gród", "Wierch", "Kąt", "Dół", "Trakt", "Ołtarz"
};

const std::vector<std::string> NameGenerator::maleSuffixes = {
    "", " Północny", " Południowy", " Wschodni", " Zachodni", " Górny", " Dolny", " Wielki", " Mały", 
    " Niżny", " Wyżny", " Zapomniany", " Ukryty", " Skryty", " Milczący", " Święty"
};

const std::vector<std::string> NameGenerator::femalePrefixes = {
    "", "Stara ", "Czarna ", "Mroczna ", "Krwawa ", "Głucha ", "Dzika ", "Wilcza ", "Zimna ", 
    "Mglista ", "Martwa ", "Sucha ", "Ciemna ", "Ponura ", "Ślepa ", "Przeklęta "
};

const std::vector<std::string> NameGenerator::femaleCores = {
    "Puszcza", "Rzeka", "Góra", "Dolina", "Knieja", "Grań", "Polana", "Osada", 
    "Przełęcz", "Wyrwa", "Jaskinia", "Twierdza", "Wioska", "Zatoka", "Ziemia"
};

const std::vector<std::string> NameGenerator::femaleSuffixes = {
    "", " Północna", " Południowa", " Wschodnia", " Zachodnia", " Górna", " Dolna", " Wielka", " Mała", 
    " Niżna", " Wyżna", " Zapomniana", " Ukryta", " Skryta", " Milcząca", " Święta"
};

const std::vector<std::string> NameGenerator::neutralSuffixes = {
    " Mgły", " Cienia", " Mroku", " Świtu", " Zmierzchu", " Echa", " Lodu", " Krwi", 
    " Zmory", " Pustki", " Rozpaczy", " Nadziei", " Zguby", " Popiołu", " Szronu"
};

void NameGenerator::ResetMemory()
{
    generatedNames.clear();
}
std::string NameGenerator::GetRandomName()
{
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());

    std::uniform_int_distribution<> genderDist(0, 1);
    
    std::string finalName;
    int attempts = 0;

    do
    {
        bool isMale = genderDist(gen) == 0;
        std::string prefix, core, suffix;

        if (isMale)
        {
            std::uniform_int_distribution<> prefixDist(0, malePrefixes.size() - 1);
            std::uniform_int_distribution<> coreDist(0, maleCores.size() - 1);
            std::uniform_int_distribution<> suffixDist(0, maleSuffixes.size() + neutralSuffixes.size() - 1);

            prefix = malePrefixes[prefixDist(gen)];
            core = maleCores[coreDist(gen)];
            
            int sIndex = suffixDist(gen);
            if (sIndex < maleSuffixes.size()) 
                suffix = maleSuffixes[sIndex];
            else 
                suffix = neutralSuffixes[sIndex - maleSuffixes.size()];
        }
        else
        {
            std::uniform_int_distribution<> prefixDist(0, femalePrefixes.size() - 1);
            std::uniform_int_distribution<> coreDist(0, femaleCores.size() - 1);
            std::uniform_int_distribution<> suffixDist(0, femaleSuffixes.size() + neutralSuffixes.size() - 1);

            prefix = femalePrefixes[prefixDist(gen)];
            core = femaleCores[coreDist(gen)];
            
            int sIndex = suffixDist(gen);
            if (sIndex < femaleSuffixes.size()) 
                suffix = femaleSuffixes[sIndex];
            else 
                suffix = neutralSuffixes[sIndex - femaleSuffixes.size()];
        }

        finalName = prefix + core + suffix;
        attempts++;
        
        if (attempts > 500) break;

    } while (generatedNames.count(finalName));

    generatedNames.insert(finalName);
    return finalName;
}