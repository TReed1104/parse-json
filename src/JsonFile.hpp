#ifndef PARSEJSON_JSONFILE_H_
#define PARSEJSON_JSONFILE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

class JsonFile {
public:
	bool isFileLoaded = false;
	std::string fileName;

	JsonFile(const std::string& fileName = "NOT GIVEN");
	~JsonFile();

	// Gets
	template <typename T> T Get(const std::string& objectName);
	template <typename T> std::vector<T> GetArray(const std::string& objectName);
	// Sets
	template <typename T> void Set(const std::string& objectName, const T& value);
	template <typename T> void SetArray(const std::string& objectName, const std::vector<T>& valueArray);
	// Appends
	template <typename T> void Add(const std::string& objectName, const T& value);
	template <typename T> void AddArray(const std::string& objectName, const std::vector<T>& valueArray);


private:
	rapidjson::Document* jsonDocument;

	bool Load(const std::string& fileName);
	std::vector<std::string> SplitString(const std::string& stringToSplit, const char& splitToken);
	template <typename T> T GetDefaultValue();
	template <typename T> T GetValue(const rapidjson::Value& object);
	template <typename T> T SetValue(const rapidjson::Value& object, const T& value);

};

// Constructors & Deconstructors
JsonFile::JsonFile(const std::string& fileName) {
	this->fileName = fileName;
	if (fileName != "NOT GIVEN") {
		isFileLoaded = Load(fileName);
	}
	else {
		isFileLoaded = false;
		std::cout << "JsonFile.hpp >>>> File name was not supplied, no file was loaded" << std::endl;
	}
}
JsonFile::~JsonFile() {
	delete jsonDocument;
}

// Loading functions
bool JsonFile::Load(const std::string& fileName) {
	std::ifstream fileStream(fileName);
	rapidjson::IStreamWrapper inputStream(fileStream);
	jsonDocument = new rapidjson::Document();
	jsonDocument->ParseStream(inputStream);

	if (jsonDocument->HasParseError()) {
		std::cout << "JsonFile.hpp >>>> File: " << fileName << " was not loaded" << std::endl;
		std::cout << "JsonFile.hpp >>>> Parser Errors: " << rapidjson::GetParseError_En(jsonDocument->GetParseError()) << std::endl;
		return false;
	}
	else {
		std::cout << "JsonFile.hpp >>>> File: " << fileName << " was loaded successfully" << std::endl;
		return true;
	}
}

// Splits a string using the given splitToken, E.g. ""The.Cat.Sat.On.The.Mat" splits with token '.' into Vector[6] = {The, Cat, Sat, On, The, Mat};
std::vector<std::string> JsonFile::SplitString(const std::string& stringToSplit, const char& splitToken) {

	std::vector<std::string> splitString;	// Stores the split sections of string for the return.
	std::string currentSplit = "";			// Stores the current section being split off.

	size_t sizeOfStringArray = stringToSplit.size();			// .Size() of a const so it never changes and we store it once.
	for (size_t i = 0; i < sizeOfStringArray; i++) {
		char currentChar = stringToSplit.at(i);
		if (currentChar == splitToken) {
			splitString.push_back(currentSplit);
			currentSplit = "";
		}
		else {
			currentSplit += currentChar;
		}

		if (i == sizeOfStringArray - 1 && currentChar != splitToken) {
			// Catches the final section of string as there might not be a follow up split token.
			splitString.push_back(currentSplit);
		}
	}

	return splitString;
}

// Get Default value Functions, uses Templating
template<typename T> inline T JsonFile::GetDefaultValue() {
	return 0;
}
template<> inline int JsonFile::GetDefaultValue() {
	return 0;
}
template<> inline float JsonFile::GetDefaultValue() {
	return 0.0f;
}
template<> inline double JsonFile::GetDefaultValue() {
	return 0.0;
}
template<> inline std::string JsonFile::GetDefaultValue() {
	return "";
}
template<> inline bool JsonFile::GetDefaultValue() {
	return false;
}

// Get value functions, uses Templating
template<typename T> inline T JsonFile::GetValue(const rapidjson::Value& object) {
	return GetDefaultValue<T>();
}
template<> inline int JsonFile::GetValue(const rapidjson::Value& object) {
	int result;
	if (object.IsInt()) {
		result = object.GetInt();	// Get the value to return
		return result;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not an Int" << std::endl;
		return GetDefaultValue<int>();
	}
}
template<> inline float JsonFile::GetValue(const rapidjson::Value& object) {
	float result;
	if (object.IsFloat()) {
		result = object.GetFloat();	// Get the value to return
		return result;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a float" << std::endl;
		return GetDefaultValue<float>();
	}
}
template<> inline double JsonFile::GetValue(const rapidjson::Value& object) {
	double result;
	if (object.IsDouble()) {
		result = object.GetDouble();	// Get the value to return
		return result;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a double" << std::endl;
		return GetDefaultValue<double>();
	}
}
template<> inline std::string JsonFile::GetValue(const rapidjson::Value& object) {
	std::string result;
	if (object.IsString()) {
		result = object.GetString();	// Get the value to return
		return result;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a std::string" << std::endl;
		return GetDefaultValue<std::string>();
	}
}
template<> inline bool JsonFile::GetValue(const rapidjson::Value& object) {
	bool result;
	if (object.IsBool()) {
		result = object.GetBool();	// Get the value to return
		return result;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a boolean" << std::endl;
		return GetDefaultValue<bool>();
	}
}

// Get Functions exposed by the API, objectName should use the schema: key.key.index.value, etc.
template<typename T> inline T JsonFile::Get(const std::string& objectName) {
	// Check we've been given a key
	if (objectName != "") {
		// check the file is actually loaded
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* value = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return GetDefaultValue<T>();
					}
					value = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!value->IsArray()) {
						if (value->HasMember(splitString[i].c_str())) {
							value = &(*value)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return GetDefaultValue<T>();
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = value->Size();
						int indexOfValue = 0;
						// try and convert the substring to an int, if not return default
						try {
							indexOfValue = std::stoi(splitString[i]);	// convert from our substring to our indexer
						}
						catch (...) {
							std::cout << "JsonFile.hpp >>>> " << objectName << " " << splitString[i] << " is invalid as an index value" << std::endl;
							return GetDefaultValue<T>();
						}
						// Check the value is accessible in the bounds of the array
						if (arraySize > 0) {
							if (arraySize > indexOfValue) {
								value = &(*value)[indexOfValue];
							}
							else {
								std::cout << "JsonFile.hpp >>>> " << objectName << " index: " << indexOfValue << " is out of bounds" << std::endl;
								return GetDefaultValue<T>();
							}
						}
						else {
							std::cout << "JsonFile.hpp >>>> " << objectName << " Array is empty" << std::endl;
							return GetDefaultValue<T>();
						}
					}
				}
			}
			// Check we haven't ended up with a JSON object instead of a value
			if (value->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return GetDefaultValue<T>();
			}
			// If we've made it passed all the conditions, return our value of type <T>
			return GetValue<T>(*value);		// Return the found value or default value if not
		}
		else {
			std::cout << "JsonFile.hpp >>>> File is not loaded, cannot call Get<T>()" << std::endl;
			return GetDefaultValue<T>();
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> No key was defined for Get<T>() to use for traversal" << std::endl;
		return GetDefaultValue<T>();
	}
}
template<typename T> inline std::vector<T> JsonFile::GetArray(const std::string& objectName) {
	// Check we've been given a key
	if (objectName != "") {
		std::vector<T> result;
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* value = nullptr;
			// Traverse the DOM to find the array we want
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					// JSON spec dictates that arrays need a root key, so we still need to get the root
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return std::vector<T>();
					}
					value = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!value->IsArray()) {
						if (value->HasMember(splitString[i].c_str())) {
							value = &(*value)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return std::vector<T>();
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = value->Size();
						int indexOfValue = 0;
						// try and convert the substring to an int, if not return default
						try {
							indexOfValue = std::stoi(splitString[i]);	// convert from our substring to our indexer
						}
						catch (...) {
							std::cout << "JsonFile.hpp >>>> " << objectName << " " << splitString[i] << " is invalid as an index value" << std::endl;
							return result;
						}
						// Check the value is accessible in the bounds of the array
						if (arraySize > 0) {
							if (arraySize > indexOfValue) {
								value = &(*value)[indexOfValue];
							}
							else {
								std::cout << "JsonFile.hpp >>>> " << objectName << " index: " << indexOfValue << " is out of bounds" << std::endl;
								return std::vector<T>();
							}
						}
						else {
							std::cout << "JsonFile.hpp >>>> " << objectName << " Array is empty" << std::endl;
							return std::vector<T>();
						}
					}
				}
			}
			// Check we haven't ended up with a JSON object instead of a value
			if (value->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return std::vector<T>();
			}

			// Check we haven't ended up with a key that isn't an array
			if (!value->IsArray()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is not an array" << std::endl;
				return std::vector<T>();
			}

			// iterate through the array getting each element
			for (const auto& item : value->GetArray()) {
				result.push_back(GetValue<T>(item));
			}

			return result;
		}
		else {
			std::cout << "JsonFile.hpp >>>> File is not loaded, cannot call Get<T>()" << std::endl;
			return std::vector<T>();
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> No key was defined for GetArray<T>() to use for traversal" << std::endl;
		return std::vector<T>();
	}
}

// Set Functions, uses Templating
template<typename T> inline void JsonFile::Set(const std::string& objectName, const T& value) {
	// Check we've been given a key
	if (objectName != "") {
		// check the file is actually loaded
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* value = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return;
					}
					value = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!value->IsArray()) {
						if (value->HasMember(splitString[i].c_str())) {
							value = &(*value)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return;
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = value->Size();
						int indexOfValue = 0;
						// try and convert the substring to an int, if not return default
						try {
							indexOfValue = std::stoi(splitString[i]);	// convert from our substring to our indexer
						}
						catch (...) {
							std::cout << "JsonFile.hpp >>>> " << objectName << " " << splitString[i] << " is invalid as an index value" << std::endl;
							return;
						}
						// Check the value is accessible in the bounds of the array
						if (arraySize > 0) {
							if (arraySize > indexOfValue) {
								value = &(*value)[indexOfValue];
							}
							else {
								std::cout << "JsonFile.hpp >>>> " << objectName << " index: " << indexOfValue << " is out of bounds" << std::endl;
								return;
							}
						}
						else {
							std::cout << "JsonFile.hpp >>>> " << objectName << " Array is empty" << std::endl;
							return;
						}
					}
				}
			}
			// Check we haven't ended up with a JSON object instead of a value
			if (value->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return;
			}
			
			// We've reached our depth in the DOM, amend the value
			return;	// For break pointing
		}
		else {
			std::cout << "JsonFile.hpp >>>> File is not loaded, cannot call Get<T>()" << std::endl;
			return;
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> No key was defined for Get<T>() to use for traversal" << std::endl;
		return;
	}
}
template<typename T> inline void JsonFile::SetArray(const std::string& objectName, const std::vector<T>& valueArray) {
	// Check we've been given a key
	if (objectName != "") {
		// check the file is actually loaded
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* value = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return;
					}
					value = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!value->IsArray()) {
						if (value->HasMember(splitString[i].c_str())) {
							value = &(*value)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return;
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = value->Size();
						int indexOfValue = 0;
						// try and convert the substring to an int, if not return default
						try {
							indexOfValue = std::stoi(splitString[i]);	// convert from our substring to our indexer
						}
						catch (...) {
							std::cout << "JsonFile.hpp >>>> " << objectName << " " << splitString[i] << " is invalid as an index value" << std::endl;
							return;
						}
						// Check the value is accessible in the bounds of the array
						if (arraySize > 0) {
							if (arraySize > indexOfValue) {
								value = &(*value)[indexOfValue];
							}
							else {
								std::cout << "JsonFile.hpp >>>> " << objectName << " index: " << indexOfValue << " is out of bounds" << std::endl;
								return;
							}
						}
						else {
							std::cout << "JsonFile.hpp >>>> " << objectName << " Array is empty" << std::endl;
							return;
						}
					}
				}
			}
			// Check we haven't ended up with a JSON object instead of a value
			if (value->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return;
			}
			// Check we haven't ended up with a key that isn't an array
			if (!value->IsArray()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is not an array" << std::endl;
				return;
			}

			// We've reached our depth in the DOM, amend the value
			return;	// For break pointing
		}
		else {
			std::cout << "JsonFile.hpp >>>> File is not loaded, cannot call Get<T>()" << std::endl;
			return;
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> No key was defined for Get<T>() to use for traversal" << std::endl;
		return;
	}
}
#endif