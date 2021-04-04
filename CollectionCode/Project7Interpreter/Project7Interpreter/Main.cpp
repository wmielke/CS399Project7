#include <cstdio>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
//#include <fileapi.h>
//#include <Windows.h>


#define TEST 1


std::string tabs = "";
size_t switchCaseBrace = 0;
size_t cases = 0;
size_t switchLine = 0;
bool inSwitch = false;


std::string GoThroughFile(const std::string& path)
{
	//std::cout << "file path: " << path << std::endl;

	size_t lastDash = path.find_last_of("\\");
	lastDash = (lastDash == std::string::npos) ? 0 : lastDash + 1;
	const std::string fileName = path.substr(lastDash);
	std::string output = tabs + fileName + ":\n";
	
	std::ifstream fileText(path);

	size_t ifAmount = 0, commentNum = 0, lineNum = 1;

	tabs += "\t";

	/// check each line of the file
	/// and skip all empty lines
	std::string line;
	while (std::getline(fileText, line))
	{
		++lineNum;
		if (!line.empty())
		{
			/// erase all of the white spaces at the start of the line
			size_t spaceAmount = 0;
			while (line[spaceAmount] == ' ' || line[spaceAmount] == '\t')
				spaceAmount++;
			if (spaceAmount)
				line.erase(0, spaceAmount);

			if (line[0] == '/' && line[1] == '/')
				++commentNum;
			if (line[0] == 'i' && line[1] == 'f')
				++ifAmount;
			if (inSwitch)
			{
				std::string::size_type pos = 0;
				std::string target = "case ";
				while ((pos = line.find(target, pos)) != std::string::npos)
				{
					++cases;
					pos += target.length();
				}
				pos = 0;
				while (pos != line.size())
				{
					switchCaseBrace += (line[pos] == '{' ? 1 : (line[pos] == '}' ? -1 : 0) );
					pos++;
				}
				if (!switchCaseBrace)
				{
					inSwitch = false;
					output += tabs + "switch line " + std::to_string(switchLine) + ": cases " + std::to_string(cases) + "\n";
					cases = 0;
				}
			}
			if (line.find("switch") != std::string::npos)
			{
				switchLine = lineNum;
				inSwitch = true;
			}
		}
	}

	if (ifAmount)
		output += tabs + "if's: " + std::to_string(ifAmount) + "\n";
	if (commentNum)
		output += tabs + "if's: " + std::to_string(commentNum) + "\n";

	tabs = tabs.substr(0, tabs.size() - 1);

	return output;
}


std::string GoThroughDirectory(const std::string& path)
{
	//std::cout << "directory path: " << path << std::endl;

	size_t lastDash = path.find_last_of('\\');
	lastDash = (lastDash == std::string::npos) ? 0 : lastDash + 1;
	const std::string fileName = path.substr(lastDash);
	std::string output = tabs + fileName + ":\n";

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string const entryPath = entry.path().string();
		//std::cout << "current path: " << entryPath << std::endl;
		if (entryPath.find(".") == std::string::npos)	/// => this is a directory
		{
			if (entryPath.find("Dockerfile") == std::string::npos && entryPath.find("LICENSE") == std::string::npos)
			{
				tabs += "\t";
				output += GoThroughDirectory(entryPath);
				tabs = tabs.substr(0, tabs.size() - 1);
			}
		}
		/// if file is .h or .c/.cpp => go through it and collect the information about it
		else if (!(entryPath.find(".h") == std::string::npos
				&& entryPath.find(".c") == std::string::npos))
		{
			tabs += "\t";
			output += GoThroughFile(entryPath);
			tabs = tabs.substr(0, tabs.size() - 1);
		}
	}

#if !TEST
	/// if the output is the same as the original
	/// then there were no .h or .c/.cpp files
	if (!output.compare(fileName + "\n"))
		return "";
#endif

	return output;
}


int main()
{
	std::string output = GoThroughDirectory("CodeBases/VVVVVV-master/VVVVVV-master");
	std::cout << "\n\n\n" << output << std::endl;

	//std::system("pause");

	return 0;
}

