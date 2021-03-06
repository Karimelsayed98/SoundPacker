#include "stdafx.h"
#include "HelperFunctions.h"

double runAlgorithm(std::string inputFile, std::string algorithm, int duration, bool allowCopy, bool multithreading) {
	// Reading the Input File and Parsing the input to vector<File>
	std::vector<File> inputFiles = parseInput(inputFile);

	// TODO: Calculating the Running Time of the Algorithm

	// Running the Selected Algorithm
	std::vector<Folder> resultFolders;
	double startingTime = clock();

	if (algorithm == WORST_FIT_LS) {
		resultFolders = WorstFitLS(inputFiles, duration);
	}
	else if (algorithm == WORST_FIT_PQ) {
		resultFolders = WorstFitPQ(inputFiles, duration);
	}
	else if (algorithm == WORST_FIT_DEC_LS) {
		resultFolders = worstFitDecreasingLS(inputFiles, duration, multithreading);
	}
	else if (algorithm == WORST_FIT_DEC_PQ) {
		resultFolders = worstFitDecreasingPQ(inputFiles, duration, multithreading);
	}
	else if (algorithm == FIRST_FIT) {
		resultFolders = firstFitDecreasingLS(inputFiles, duration, multithreading);
	}
	else if (algorithm == FOLDER_FILLING){

		resultFolders = folderFilling(inputFiles, duration);
	}
	else if (algorithm == BEST_FIT) {
		resultFolders = bestFit(inputFiles, duration);
	}
	else {
		return -1;
	}

	// Calculate Time after algorithm
	double endingTime = clock();
	double runningTime = (endingTime - startingTime) / CLOCKS_PER_SEC;

	// Getting The Parent Folder Path of the Input Metadata File
	std::string parentPath = inputFile;
	while (parentPath.back() != '\\') {  // Removing the File Name from the Path
		parentPath.pop_back();
	}

	// Creating the Algorithm Output Folder
	createFolder(algorithm, parentPath + "Packed Files");

	// Naming the Folders
	for (int i = 0; i < resultFolders.size(); ++i) {
		resultFolders[i].name = Folder::getName(i + 1);
	}

	// Generating the Metadata file
	generateMetadata(resultFolders, parentPath + "Packed Files\\" + algorithm);

	// Copy the files to the destination folder if allowCopy == true
	if (allowCopy) {
		// Getting Input Files Path from The Parent Folder path
		std::string audioFilesPath = parentPath + "Audios";  // The Path now = {ParentFolder}\Audios

		for (auto folder: resultFolders) {
			createFolder(folder.name, parentPath + "Packed Files\\" + algorithm);
			for (auto file : folder.files) {
				// Copying each file from the source to it's folder
				// New Path: Packed Files\algorithm_name\F{i}
				copyFile(file.name, audioFilesPath, parentPath + "Packed Files\\" + algorithm + '\\' + folder.name);
			}
		}
	}

	// Returning the Elapsed Algorithm Time
	return runningTime;
}

bool directoryExists(std::string directory) {
	DWORD attributes = GetFileAttributesA(directory.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
		return false;

	return (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void createFolder(std::string name, std::string path) {
	std::string folderDirectory = path + '\\' + name;
	if (!directoryExists(folderDirectory)) {
		std::string command = "mkdir \"" + folderDirectory + '"';
		bool errorValue = system(command.c_str());
		if (errorValue)
			throw "Error in Creating the Folder";
	}
}

void copyFile(std::string name, std::string origin,std::string destination) {
	std::string destinationDirectory = destination + '\\' + name,
		originDirectory = origin + '\\' + name,
		command = "copy \"" + originDirectory + "\" \"" + destinationDirectory + '"';

	bool errorValue = system(command.c_str());
	if (errorValue)
		throw "Error in Copying the File";
}

void generateMetadata(std::vector<Folder> folders, std::string destination) {
	std::fstream metaDataFile(destination + '\\' + "metadata.txt", std::fstream::out);

	for (auto folder : folders) {
		metaDataFile << "|Folder: " << folder.name << '\n'
			<< '|' << std::string(2, '-') << "Content: (" << folder.files.size() << " Audio Files, Total Duration: "
			<< folder.totalDuration << " Seconds)\n";
		for (auto file : folder.files) {
			metaDataFile << '|' << std::string(4, '-') << "File: " << file.name << '\t' << "Duration: " << file.duration << " Seconds \n";
		}
		metaDataFile << "\n\n";
	}
}

std::vector<File> parseInput(std::string inputFile) {
	std::vector<File> inputFiles; int nFiles;
	std::fstream file(inputFile, std::fstream::in);
	
	file >> nFiles;
	for (int i = 0; i < nFiles; ++i) {
		std::string fileName; file >> fileName;
		std::string durationString; file >> durationString;
		inputFiles.push_back(File(fileName, parseDurationString(durationString)));
	}

	return inputFiles;
}

int parseDurationString(std::string durationString) {
	int hours, minutes, seconds;
	std::stringstream ss;

	// Parsing the Hours
	int hoursPartition = durationString.find(":");
	ss << durationString.substr(0, hoursPartition);
	ss >> hours; ss.clear();

	// Parsing the Minutes
	int minutesPartition = durationString.find(":", hoursPartition + 1);
	ss << durationString.substr(hoursPartition + 1, minutesPartition - hoursPartition - 1);
	ss >> minutes; ss.clear();

	// Parsing the Seconds
	ss << durationString.substr(minutesPartition + 1);
	ss >> seconds; ss.clear();

	return (hours * 60 * 60) + (minutes * 60) + seconds;
}
