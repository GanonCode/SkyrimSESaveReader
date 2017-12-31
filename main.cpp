#include "SkyrimSESaveReader.hpp"

int main(int argc, char** argv){
	SkyrimSESaveReader reader;
	
	if (reader.loadSaveFile("SkyrimSE.ess")){
		reader.loadDataFromSaveFile();
	
		reader.printAllData();
	} else {
		cout << "Error loading save file\n";
	}
	
  return 0;
}