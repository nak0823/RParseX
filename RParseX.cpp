#include <iostream>
#include <Windows.h>
#include <string>
#include <filesystem>
#include <vector>

std::vector<std::string> GetDirectoryPaths(std::string);
void Scraper(std::string);
void ConsoleTitle(const std::string& title);

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

int main()
{
	ConsoleTitle("RParseX 1.0.0 ~ by Serialized");
	std::string logsPath = GetLogsLocation();
	Scraper(logsPath);
}
