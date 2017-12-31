#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>

using namespace std;

const int FILE_MAGIC_SIZE = 13;

enum GLOBAL_DATA_TYPE{
  MISC_STATUS = 0,
  PLAYER_LOCATION = 1,
  TEST = 2,
  GLOBAL_VARIABLES = 3,
  CREATED_OBJECTS = 4,
  EFFECTS = 5,
  WEATHER = 6,
  AUDIO = 7,
  SKYCELLS = 8,
  
  PROCESS_LISTS = 100,
  COMBAT = 101,
  INTERFACE = 102,
  ACTOR_CAUSES = 103,
  UNKNOWN_104 = 104,
  DETECTION_MANAGER = 105,
  LOCATION_METADATA = 106,
  QUEST_STATIC_DATA = 107,
  STORYTELLER = 108,
  MAGIC_FAVORITES = 109,
  PLAYERCONTROLS = 110,
  STORY_EVENT_MANAGER = 111,
  INGREDIENT_SHARED = 112,
  MENUCONTROLS = 113,
  MENUTOPICMANAGER = 114,
  
  TEMP_EFFECTS = 1000,
  PAPYRUS = 1001,
  ANIM_OBJECTS = 1002,
  TIMER = 1003,
  SYNCHRONIZED_ANIMATIONS = 1004,
  MAIN = 1005
};

struct Header{
	uint32_t version;
	uint32_t saveNumber;
	string playerName;
	uint32_t playerLevel;
	string playerLocation;
	string gameDate;
	string playerRaceEditorId;
	uint16_t uPlayerSex;
	string strPlayerSex;
	float playerCurExp;
	float playerLevelUpExp;
	uint64_t filetime;
	uint32_t screenshotWidth;
	uint32_t screenshotHeight;
};

struct PluginInfo{
  uint8_t pluginCount;
  vector<string> plugins;
};

struct FileLocationTable{
  uint32_t formIdArrayCountOffset;
  uint32_t unknownTable3Offset;
  uint32_t globalDataTable1Offset;
  uint32_t globalDataTable2Offset;
  uint32_t changeFormsOffset;
  uint32_t globalDataTable3Offset;
  uint32_t globalDataTable1Count; // 9
  uint32_t globalDataTable2Count; // 15
  uint32_t globalDataTable3Count; // 5 (bugged, does not include type 1001
  uint32_t changeFormCount;
  
  // plus 15 unused uint32s
};

struct GlobalDataTable{
  uint32_t type;
  uint32_t length;
  vector<uint8_t> data;
};

struct SkyrimSESaveReader{
	char* buffer;
	int bufferSize, bufferPos;
	
  string magic;
  int headerStartPos, headerSize;
	Header header;
  char* screenshotData;
  uint8_t formVersion;
  uint32_t pluginInfoSize;
  PluginInfo pluginInfo;
  FileLocationTable fileLocationTable;
	GlobalDataTable globalDataTable1, globalDataTable2, globalDataTable3;
  
	bool loadSaveFile(const char* filename);
	
	uint8_t readByte();
	uint16_t read2Bytes();
	uint32_t read4Bytes();
	uint64_t read8Bytes();
	float readFloat();
	string readString();
	
	void loadDataFromSaveFile(), loadHeaderData(), loadScreenshotData(), loadFormVersion(), loadPluginInfo(), loadFileLocationTable();
	void printAllData(), printHeaderData(), printScreenshotData(), printFormVersion(), printPluginInfo(), printFileLocationTable();
	
	SkyrimSESaveReader();
	~SkyrimSESaveReader();
};