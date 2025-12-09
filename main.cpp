#include <iostream>
#include <fstream>
#include <string>
#include <regex>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: chrome_to_firefox <input.js> <output.js>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    std::ifstream in(inputFile);
    if (!in.is_open()) {
        std::cerr << "Failed to open input file: " << inputFile << "\n";
        return 1;
    }

    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << "\n";
        return 1;
    }

    std::string line;
    std::regex chromeRegex(R"(chrome\.)");
    std::regex storageGetRegex(R"(browser\.storage\.local\.get\(\s*([^\)]+),\s*([^\)]+)\))");
    std::regex runtimeSendMessageRegex(R"(browser\.runtime\.sendMessage\(\s*([^\)]+),\s*([^\)]+)\))");

    while (std::getline(in, line)) {
        // Step 1: replace chrome.* with browser.*
        line = std::regex_replace(line, chromeRegex, "browser.");

        // Step 2: wrap storage.local.get(callback) with promise
        // e.g., browser.storage.local.get(['x'], cb) => browser.storage.local.get(['x']).then(cb)
        line = std::regex_replace(line, storageGetRegex, "browser.storage.local.get($1).then($2)");

        // Step 3: wrap runtime.sendMessage(callback)
        line = std::regex_replace(line, runtimeSendMessageRegex, "browser.runtime.sendMessage($1).then($2)");

        out << line << "\n";
    }

    std::cout << "Conversion complete. Output: " << outputFile << "\n";
    return 0;
}
