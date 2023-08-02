#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

// Checks if logs directory is valid.
bool DirectoryExists(const std::string& path) {
	DWORD attribute = GetFileAttributesA(path.c_str());
	return (attribute != INVALID_FILE_ATTRIBUTES && (attribute & FILE_ATTRIBUTE_DIRECTORY));
}

std::string GetLogsLocation() {
	std::string logsPath;

	while (true) {
		std::cout << "Enter the location of the logs: ";
		std::getline(std::cin, logsPath);

		if (!DirectoryExists(logsPath)) {
			system("cls");
			GetLogsLocation();
		}

		return logsPath;
	}
}

// Struct to store data extracted from password files.
struct Combo {
	std::string Username;
	std::string Password;
	std::string Source;
};

// Formats Combo data as a string.
std::string FormatCombo(const Combo& combo) {
	return std::format("{}:{} | {}", combo.Username, combo.Password, combo.Source);
}

// Sets the title of the terminal.
void SetConsoleTitle(const std::string& title) {
	SetConsoleTitleA(title.c_str());
}

// Obtains valid directories that include a password file.
std::vector<std::string> GetDirectoryPaths(const std::string& basePath) {
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

// Extracts data from password files and returns as a vector of Combo.
std::vector<Combo> ExtractedData(std::vector<std::string> ratPaths) {
	size_t i = 0;
	size_t r = ratPaths.size();
	std::vector<Combo> obtainedCombos;

	for (const std::string& path : ratPaths) {
		SetConsoleTitle(std::format("RParseX 1.0.0 ~ by Serialized | Scanning Directories: {}/{}", i, r));

		std::vector<std::string> fileContents;
		std::string line;
		std::ifstream inputPath(path);

		while (std::getline(inputPath, line)) {
			fileContents.push_back(line);
		}

		inputPath.close();

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

// Returns the current date and time as a string.
std::string GetCurrentDateTimeString() {
	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::tm timeinfo;
#ifdef _WIN32
	localtime_s(&timeinfo, &currentTime);
#else
	localtime_r(&currentTime, &timeinfo);
#endif
	std::ostringstream oss;
	oss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
	return oss.str();
}

// Creates an empty file with the given filename if it doesn't exist.
void CreateFile(const std::string& filename) {
	if (!fs::exists(filename)) {
		std::ofstream file(filename);
		if (file) file.close();
	}
}


// Scans targets and writes matching combos to target specific files (Websites.txt).
void ScanTarget(std::vector<Combo> data) {
	fs::path currentPath = fs::current_path();
	std::string websiteFile = (currentPath / "Websites.txt").string();
	std::string combosFile = (currentPath / "Combos.txt").string();
	CreateFile(websiteFile);
	CreateFile(combosFile);

	size_t comboCount = data.size();
	size_t i = 1;
	int hits = 0;

	// Obtain targets
	std::vector<std::string> targets;
	std::ifstream tFile(websiteFile);
	std::string line;

	while (std::getline(tFile, line)) {
		targets.push_back(line);
	}

	tFile.close();

	for (const Combo& current : data) {
		std::ofstream combosWrite(combosFile, std::ios::app);
		if (combosWrite) combosWrite << FormatCombo(current) << "\n";

		for (const std::string& target : targets) {
			std::string targetsFile = target + ".txt";

			// Check if current.Source is found in target
			if (current.Source.find(target) != std::string::npos) {
				fs::path currentPath = fs::current_path();
				std::string currentDateTime = GetCurrentDateTimeString();
				std::filesystem::path folderPath = std::filesystem::current_path() / currentDateTime;
				if (std::filesystem::create_directory(folderPath)) {
				}

				if (!fs::exists(currentPath / currentDateTime / targetsFile)) {
					std::ofstream file(currentPath / currentDateTime / targetsFile);
					if (file) file.close();
				}
				std::ofstream targetsWrite(currentPath / currentDateTime / targetsFile, std::ios::app);
				if (targetsWrite) targetsWrite << FormatCombo(current) << "\n";
				hits++;
			}
		}

		i++;
		SetConsoleTitle(std::format("RParseX 1.0.0 ~ by Serialized | Checking Targets: {}/{} | Hits Found: {}", i, comboCount, hits));
	}

	std::cout << std::format("Finished with: {} hits.", hits);
	int input;
	std::cin >> input;
}

// Main function to scrape and process data.
void Scraper(std::string path) {
	std::vector<std::string> paths = GetDirectoryPaths(path);
	std::vector<Combo> data = ExtractedData(paths);
	ScanTarget(data);
}
