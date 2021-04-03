#include <cstdio>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>


#define TEST 1


std::string tabs = "";


std::string GoThroughFile(const std::string& path)
{
	//std::cout << "file path: " << path << std::endl;

	size_t lastDash = path.find_last_of("\\");
	lastDash = (lastDash == std::string::npos) ? 0 : lastDash + 1;
	const std::string fileName = path.substr(lastDash);
	std::string output = tabs + fileName + ":\n";
	
	std::ifstream fileText(path);

	size_t ifAmount = 0, caseAmount = 0, commentNum = 0, lineNum = 1;

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

		}
	}

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
			tabs += "\t";
			output += GoThroughDirectory(entryPath);
			tabs = tabs.substr(0, tabs.size() - 1);
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
	std::string output = GoThroughDirectory("test");
	std::cout << "\n\n\n" << output << std::endl;

	//std::system("pause");

	return 0;
}

