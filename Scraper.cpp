#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

// Struct is used to store the important data that can be extracted from password files.
struct Combo {
	std::string Username;
	std::string Password;
	std::string Source;
};

std::string FormatCombo(const Combo& combo) {
	return std::format("{}:{} | {}", combo.Username, combo.Password, combo.Source);
}

// Sets the title of the terminal.
void ConsoleTitle(const std::string& title) {
	SetConsoleTitleA(title.c_str());
}

// Obtains all valid directories that include a password file.
std::vector<std::string> GetDirectoryPaths(std::string basePath) {
	std::vector<std::string> validPaths, ratPaths;
	const std::string pwdFile = "\\Passwords.txt";

	for (const auto& entry : fs::directory_iterator(basePath)) {
		if (fs::is_directory(entry)) validPaths.push_back(entry.path().string());
	}

	for (const std::string& foundDir : validPaths) {
		if (fs::exists(foundDir + pwdFile)) ratPaths.push_back(foundDir + pwdFile);
	}

	return ratPaths;
}

// Returns all the formatted data into a vector of Combo's structure.
std::vector<Combo> extractedData(std::vector<std::string> ratPaths) {
	int i = 0;
	size_t r = r = ratPaths.size();
	std::vector<Combo> obtainedCombos;

	for (const std::string& path : ratPaths) {
		ConsoleTitle(std::format("RParseX 1.0.0 ~ by Serialized | Scanning Directories: {}/{}", i, r));

		// Obtain the data from the password file.
		std::vector<std::string> fileContents;
		std::string line;
		std::ifstream inputPath(path);

		while (std::getline(inputPath, line)) {
			fileContents.push_back(line);
		}

		inputPath.close();

		// Removes basically common useless lines.
		fileContents.erase(std::remove_if(fileContents.begin(), fileContents.end(), [](const std::string& line) {
			return (line.find("===============") != std::string::npos) || (line.find("Application") != std::string::npos);
			}), fileContents.end());

		std::vector<Combo> extractedContents;

		for (size_t i = 0; i < fileContents.size(); i++) {
			if (fileContents[i].find("URL:") == 0) {
				std::string url = fileContents[i].substr(4);
				url.erase(0, url.find_first_not_of(" \t\r\n"));

				std::string username = (i + 1 < fileContents.size()) ? fileContents[i + 1].substr(9) : "";
				username.erase(0, username.find_first_not_of(" \t\r\n"));

				std::string password = (i + 2 < fileContents.size()) ? fileContents[i + 2].substr(9) : "";
				password.erase(0, password.find_first_not_of(" \t\r\n"));

				if (!username.empty() && !password.empty()) {
					extractedContents.push_back({ username, password, url });
				}
			}
			else if (fileContents[i].find("SOFT:") == 0) {
				std::string soft = fileContents[i].substr(5);
				soft.erase(0, soft.find_first_not_of(" \t\r\n"));

				std::string url = (i + 1 < fileContents.size()) ? fileContents[i + 1].substr(4) : "";
				url.erase(0, url.find_first_not_of(" \t\r\n"));

				std::string username = (i + 2 < fileContents.size()) ? fileContents[i + 2].substr(6) : "";
				username.erase(0, username.find_first_not_of(" \t\r\n"));

				std::string password = (i + 3 < fileContents.size()) ? fileContents[i + 3].substr(6) : "";
				password.erase(0, password.find_first_not_of(" \t\r\n"));

				if (!url.empty() && !username.empty() && !password.empty()) {
					extractedContents.push_back({ username, password, url });
				}
			}
		}

		for (const Combo& combo : extractedContents) obtainedCombos.push_back(combo);
	}

	return obtainedCombos;
}

void CreateFile(const std::string& filename) {
	if (!fs::exists(filename)) {
		std::ofstream file(filename);
		if (file) file.close();
	}
}

void ScanTarget(std::vector<Combo> data) { 
	fs::path currentPath = fs::current_path();
	std::string websiteFile = (currentPath / "Websites.txt").string();
	std::string combosFile = (currentPath / "Combos.txt").string();

	CreateFile(websiteFile);
	CreateFile(combosFile);


	size_t comboCount = data.size();
	int hits = 0, i = 0;

	// Obtain targets
	std::vector<std::string> targets;
	std::ifstream tFile(websiteFile);
	std::string line;

	while (std::getline(tFile, line)) {
		targets.push_back(line);
	}

	tFile.close();


	for (const Combo& current : data) {
		ConsoleTitle(std::format("RParseX 1.0.0 ~ by Serialized | Checking Targets: {}/{} | Hits Found: {}", i, comboCount, hits));
		std::ofstream combosWrite(combosFile, std::ios::app);
		if (combosWrite) combosWrite << FormatCombo(current) << "\n";

		for (const std::string& target : targets) {
			std::string targetsFile = (currentPath / (std::format("{}.txt", target))).string();

			// Check if current.Source is found in target
			if (current.Source.find(target) != std::string::npos) {
				std::cout << "Match found for " + target << std::endl;
				CreateFile(targetsFile);

				std::ofstream targetsWrite(targetsFile, std::ios::app);
				if (targetsWrite) targetsWrite << FormatCombo(current) << "\n";
			}
		}
	}
	
}

void Scraper(std::string path) {
	std::vector<std::string> paths = GetDirectoryPaths(path);
	std::vector<Combo> data = extractedData(paths);
	ScanTarget(data);
}