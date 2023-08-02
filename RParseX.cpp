#include <iostream>
#include <Windows.h>
#include <string>
#include <filesystem>
#include <vector>

std::vector<std::string> GetDirectoryPaths(std::string);
void Scraper(std::string);
void SetConsoleTitle(const std::string& title);
bool DirectoryExists(const std::string& path);
std::string GetLogsLocation();

int main()
{
	SetConsoleTitle("RParseX 1.0.0 ~ by Serialized");
	std::string logsPath = GetLogsLocation();
	Scraper(logsPath);
}
