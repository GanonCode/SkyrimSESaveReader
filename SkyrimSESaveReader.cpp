#include "SkyrimSESaveReader.hpp"

bool SkyrimSESaveReader::loadSaveFile(const char* filename){
	ifstream f;
	f.open(filename);
	
	if (f.fail()) return false;
	
	f.seekg(0, f.end);
	bufferSize = f.tellg();
	buffer = new char[bufferSize];
	bufferPos = 0;
	
	f.seekg(0, f.beg);
	f.read(buffer, bufferSize);
	
	f.close();
	return true;
}

uint8_t SkyrimSESaveReader::readByte(){
	return static_cast<uint8_t>(buffer[bufferPos++]);
}

uint16_t SkyrimSESaveReader::read2Bytes(){
	uint16_t a, b;
	
	a = static_cast<uint16_t>(readByte());
	b = static_cast<uint16_t>(readByte());
	
	return (a | b << 8);
}

uint32_t SkyrimSESaveReader::read4Bytes(){
	uint32_t a, b, c, d;
	
	a = static_cast<uint32_t>(readByte()), b = static_cast<uint32_t>(readByte());
	c = static_cast<uint32_t>(readByte()), d = static_cast<uint32_t>(readByte());
	
	return (a | b << 8 | c << 16 | d << 24);
}

uint64_t SkyrimSESaveReader::read8Bytes(){
	uint64_t a, b, c, d, e, f, g, h;
	
	a = static_cast<uint64_t>(readByte()), b = static_cast<uint64_t>(readByte());
	c = static_cast<uint64_t>(readByte()), d = static_cast<uint64_t>(readByte());
	e = static_cast<uint64_t>(readByte()), f = static_cast<uint64_t>(readByte());
	g = static_cast<uint64_t>(readByte()), h = static_cast<uint64_t>(readByte());
	
	return (a | b << 8 | c << 16 | d << 24 | e << 32 | f << 40 | g << 48 | h << 56);
}

float SkyrimSESaveReader::readFloat(){
	float f;
	
	uint8_t bytes[] = {readByte(), readByte(), readByte(), readByte()};
	memcpy(&f, &bytes, sizeof(f));
	
	return f;
}

string SkyrimSESaveReader::readString(){
	string str;
	uint16_t stringLength = read2Bytes();
	for (uint16_t i = 0; i < stringLength; i++)
		str.push_back(static_cast<char>(readByte()));
	return str;
}

void SkyrimSESaveReader::loadDataFromSaveFile(){
	for (int i = 0; i < FILE_MAGIC_SIZE; i++)
		magic.push_back(static_cast<char>(readByte()));
	
	if (magic != "TESV_SAVEGAME")
		cout << "Incorrect magic: " << magic << "\n";
	
	headerSize = read4Bytes();
	
	headerStartPos = bufferPos;
	loadHeaderData();
	
	if (bufferPos - headerStartPos != headerSize)
		cout << "Header length incorrect: " << headerSize << "|" << bufferPos - headerStartPos << "\n";
	
	loadScreenshotData();
	loadFormVersion();
	loadPluginInfo();
	loadFileLocationTable();
}

void SkyrimSESaveReader::loadHeaderData(){
	header.version = read4Bytes();
	header.saveNumber = read4Bytes();
	header.playerName = readString();
	header.playerLevel = read4Bytes();
	header.playerLocation = readString();
	header.gameDate = readString();
	header.playerRaceEditorId = readString();
	header.uPlayerSex = read2Bytes();
	header.strPlayerSex = header.uPlayerSex == 0 ? "Male" : "Female";
	header.playerCurExp = readFloat();
	header.playerLevelUpExp = readFloat();
	header.filetime = read8Bytes();
	header.screenshotWidth = read4Bytes();
	header.screenshotHeight = read4Bytes();
	read2Bytes(); // 2 extra bytes???
}

void SkyrimSESaveReader::loadScreenshotData(){
	int screenshotByteCount = header.screenshotWidth * header.screenshotHeight * 4;
	
	screenshotData = new char[screenshotByteCount];
	
	for (int i = 0; i < screenshotByteCount; i++)
		screenshotData[i] = readByte();
}

void SkyrimSESaveReader::loadFormVersion(){
	formVersion = readByte();
}

void SkyrimSESaveReader::loadPluginInfo(){
	pluginInfoSize = read4Bytes();
	
	for (int i = 0; i < 10; i++) readByte();
	
	pluginInfo.pluginCount = readByte();

	for (int i = 0; i < pluginInfo.pluginCount; i++)
		pluginInfo.plugins.push_back(readString());
}

void SkyrimSESaveReader::loadFileLocationTable(){
	fileLocationTable.formIdArrayCountOffset = read4Bytes();
	fileLocationTable.unknownTable3Offset = read4Bytes();
	fileLocationTable.globalDataTable1Offset = read4Bytes();
	fileLocationTable.globalDataTable2Offset = read4Bytes();
	fileLocationTable.changeFormsOffset = read4Bytes();
	fileLocationTable.globalDataTable3Offset = read4Bytes();
	fileLocationTable.globalDataTable1Count = read4Bytes();
	fileLocationTable.globalDataTable2Count = read4Bytes();
	fileLocationTable.globalDataTable3Count = read4Bytes();
	fileLocationTable.changeFormCount = read4Bytes();
}

void SkyrimSESaveReader::printAllData(){
	printHeaderData();
	
	cout << "\n";
	
	printFormVersion();
	
	cout << "\n";
	
	printPluginInfo();
	
	cout << "\n";
	
	printFileLocationTable();
	
	cout << "\n";
}

void SkyrimSESaveReader::printHeaderData(){
	cout << "================================= Header Data =================================\n";
	cout << "\tVersion: " << header.version << "\t\n";
	cout << "\tSave number: " << header.saveNumber << "\t\n";
	cout << "\tCharacter name: " << header.playerName << "\t\n";
	cout << "\tPlayer level: " << header.playerLevel << "\t\n";
	cout << "\tLocation: " << header.playerLocation << "\t\n";
	cout << "\tIn-game date: " << header.gameDate << "\t\n";
	cout << "\tPlayer race: " << header.playerRaceEditorId << "\t\n";
	cout << "\tPlayer sex: " << header.strPlayerSex << "\t\n";
	cout << "\tPlayer experience: " << header.playerCurExp << "\t\n";
	cout << "\tPlayer experience required to level up: " << header.playerLevelUpExp << "\t\n";
	cout << "\tFILETIME: " << header.filetime << "\t\n";
	cout << "\tScreenshot width: " << header.screenshotWidth << "\t\n";
	cout << "\tScreenshot height: " << header.screenshotHeight << "\t\n";
}

void SkyrimSESaveReader::printFormVersion(){
	cout << "\tForm version: " << (int)formVersion << "\n";
}

void SkyrimSESaveReader::printPluginInfo(){
	cout << "================================= Plugin Info =================================\n";
	cout << "\tPlugin count: " << (int)pluginInfo.pluginCount << "\t\n";
	cout << "\tPlugins:\n";
	for (int i = 0; i < pluginInfo.pluginCount; i++)
		cout << "\t\t" << pluginInfo.plugins[i] << "\t\n";
}

void SkyrimSESaveReader::printFileLocationTable(){
	cout << "============================= File Location Table =============================\n";
	cout << "\tForm ID array count offset: " << fileLocationTable.formIdArrayCountOffset << "\t\n";
	cout << "\tUknown table 3 offset: " << fileLocationTable.unknownTable3Offset << "\t\n";
	cout << "\tGlobal data table 1 offset: " << fileLocationTable.globalDataTable1Offset << "\t\n";
	cout << "\tGlobal data table 2 offset: " << fileLocationTable.globalDataTable2Offset << "\t\n";
	cout << "\tChange forms offset: " << fileLocationTable.changeFormsOffset << "\t\n";
	cout << "\tGlobal data table 3 offset: " << fileLocationTable.globalDataTable3Offset << "\t\n";
	cout << "\tGlobal data table 1 count: " << fileLocationTable.globalDataTable1Count << "\t\n";
	cout << "\tGlobal data table 2 count: " << fileLocationTable.globalDataTable2Count << "\t\n";
	cout << "\tGlobal data table 3 count: " << fileLocationTable.globalDataTable3Count << "\t\n";
	cout << "\tChange form count: " << fileLocationTable.changeFormCount << "\t\n";
}

SkyrimSESaveReader::SkyrimSESaveReader() : 
	buffer(nullptr), screenshotData(nullptr), bufferSize(0), bufferPos(0){
	
}

SkyrimSESaveReader::~SkyrimSESaveReader(){
	delete[] buffer;
	delete[] screenshotData;
}