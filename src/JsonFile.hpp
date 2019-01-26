#ifndef PARSEJSON_JSONFILE_H_
#define PARSEJSON_JSONFILE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
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
	template <typename T> void Set(const std::string& objectName, const T& inputValue);
	template <typename T> void SetArray(const std::string& objectName, const std::vector<T>& inputValueArray);
	// Appends
	template <typename T> void Add(const std::string& objectName, const T& inputValue);
	template <typename T> void AddArray(const std::string& objectName, const std::vector<T>& inputValueArray);
	// Removes
	template <typename T> void Remove(const std::string& objectName);

	bool Load(const std::string& fileName);
	bool Save(void);


private:
	rapidjson::Document* jsonDocument = nullptr;

	std::vector<std::string> SplitString(const std::string& stringToSplit, const char& splitToken);
	template <typename T> T GetDefaultValue();
	template <typename T> T GetValue(const rapidjson::Value& jsonValue);
	template <typename T> bool SetValue(rapidjson::Value& jsonValue, const T& inputValue);

};

// Constructors & Deconstructors
JsonFile::JsonFile(const std::string& fileName) {
	Load(fileName);
}
JsonFile::~JsonFile() {
	delete jsonDocument;
}

// Loading functions
bool JsonFile::Load(const std::string& fileName) {
	this->fileName = fileName;
	if (fileName != "NOT GIVEN") {
		// Check if we've already loaded the file
		if (jsonDocument != nullptr) {
			delete jsonDocument;
		}
		std::ifstream fileStream(fileName);
		rapidjson::IStreamWrapper inputStream(fileStream);
		jsonDocument = new rapidjson::Document();
		jsonDocument->ParseStream(inputStream);

		if (jsonDocument->HasParseError()) {
			std::cout << "JsonFile.hpp >>>> File: " << fileName << " was not loaded" << std::endl;
			std::cout << "JsonFile.hpp >>>> Parser Errors: " << rapidjson::GetParseError_En(jsonDocument->GetParseError()) << std::endl;
			isFileLoaded = false;
			return false;
		}
		else {
			std::cout << "JsonFile.hpp >>>> File: " << fileName << " was loaded successfully" << std::endl;
			isFileLoaded = true;
			return true;
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> File name was not supplied, no file was loaded" << std::endl;
		isFileLoaded = false;
		return false;
	}
}
bool JsonFile::Save(void) {
	if (isFileLoaded) {
		std::ofstream outFileStream(fileName);
		if (!outFileStream.is_open()) {
			std::cout << "JsonFile.hpp >>>> File: " << fileName << " could not be opened for writing" << std::endl;
			return false;
		}
		else {
			rapidjson::OStreamWrapper outputStreamWrapper(outFileStream);
			rapidjson::PrettyWriter<rapidjson::OStreamWrapper> fileWriter(outputStreamWrapper);
			jsonDocument->Accept(fileWriter);
			return true;
		}
	}
	else {
		std::cout << "JsonFile.hpp >>>> File is not loaded, cannot call Save()" << std::endl;
		return false;
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

// Get value functions, uses Templating overrides
template<typename T> inline T JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	return GetDefaultValue<T>();
}
template<> inline int JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	if (jsonValue.IsInt()) {
		return jsonValue.GetInt();
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not an Int" << std::endl;
		return GetDefaultValue<int>();
	}
}
template<> inline float JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	if (jsonValue.IsFloat()) {
		return jsonValue.GetFloat();
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a float" << std::endl;
		return GetDefaultValue<float>();
	}
}
template<> inline double JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	if (jsonValue.IsDouble()) {
		return jsonValue.GetDouble();
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a double" << std::endl;
		return GetDefaultValue<double>();
	}
}
template<> inline std::string JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	if (jsonValue.IsString()) {
		return jsonValue.GetString();
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a std::string" << std::endl;
		return GetDefaultValue<std::string>();
	}
}
template<> inline bool JsonFile::GetValue(const rapidjson::Value& jsonValue) {
	if (jsonValue.IsBool()) {
		return jsonValue.GetBool();
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
			rapidjson::Value* jsonValue = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return GetDefaultValue<T>();
					}
					jsonValue = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!jsonValue->IsArray()) {
						if (jsonValue->HasMember(splitString[i].c_str())) {
							jsonValue = &(*jsonValue)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return GetDefaultValue<T>();
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = jsonValue->Size();
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
								jsonValue = &(*jsonValue)[indexOfValue];
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
			if (jsonValue->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return GetDefaultValue<T>();
			}
			// If we've made it passed all the conditions, return our value of type <T>
			return GetValue<T>(*jsonValue);		// Return the found value or default value if not
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
			rapidjson::Value* jsonValue = nullptr;
			// Traverse the DOM to find the array we want
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					// JSON spec dictates that arrays need a root key, so we still need to get the root
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return std::vector<T>();
					}
					jsonValue = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!jsonValue->IsArray()) {
						if (jsonValue->HasMember(splitString[i].c_str())) {
							jsonValue = &(*jsonValue)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return std::vector<T>();
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = jsonValue->Size();
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
								jsonValue = &(*jsonValue)[indexOfValue];
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
			if (jsonValue->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return std::vector<T>();
			}

			// Check we haven't ended up with a key that isn't an array
			if (!jsonValue->IsArray()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is not an array" << std::endl;
				return std::vector<T>();
			}

			// iterate through the array getting each element
			for (const auto& item : jsonValue->GetArray()) {
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

// Set value functions, uses Templating overrides
template<typename T> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const T& inputValue) {
	return false;
}
template<> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const int& inputValue) {
	if (jsonValue.IsInt()) {
		jsonValue.SetInt(inputValue);
		return true;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not an Int" << std::endl;
		return false;
	}
}
template<> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const float& inputValue) {
	if (jsonValue.IsFloat()) {
		jsonValue.SetFloat(inputValue);
		return true;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a Float" << std::endl;
		return false;
	}
}
template<> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const double& inputValue) {
	if (jsonValue.IsDouble()) {
		jsonValue.SetDouble(inputValue);
		return true;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a Double" << std::endl;
		return false;
	}
}
template<> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const std::string& inputValue) {
	if (jsonValue.IsString()) {
		jsonValue.SetString(inputValue.c_str(), (rapidjson::SizeType)inputValue.length(), jsonDocument->GetAllocator());
		return true;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a String" << std::endl;
		return false;
	}
}
template<> inline bool JsonFile::SetValue(rapidjson::Value& jsonValue, const bool& inputValue) {
	if (jsonValue.IsBool()) {
		jsonValue.SetBool(inputValue);
		return true;
	}
	else {
		std::cout << "JsonFile.hpp >>>> value is not a Boolean" << std::endl;
		return false;
	}
}

// Set Functions exposed by the API, objectName should use the schema: key.key.index.value, etc.
template<typename T> inline void JsonFile::Set(const std::string& objectName, const T& inputValue) {
	// Check we've been given a key
	if (objectName != "") {
		// check the file is actually loaded
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* jsonValue = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return;
					}
					jsonValue = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!jsonValue->IsArray()) {
						if (jsonValue->HasMember(splitString[i].c_str())) {
							jsonValue = &(*jsonValue)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return;
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = jsonValue->Size();
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
								jsonValue = &(*jsonValue)[indexOfValue];
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
			if (jsonValue->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return;
			}

			// We've reached our depth in the DOM, amend the value
			if (SetValue<T>(*jsonValue, inputValue)) {
				// If we've successfully set the value, save the doc
				if (!Save()) {
					std::cout << "JsonFile.hpp >>>> Failed to save file" << std::endl;
				}
			}
			else {
				return;
			}
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
template<typename T> inline void JsonFile::SetArray(const std::string& objectName, const std::vector<T>& inputValueArray) {
	// Check we've been given a key
	if (objectName != "") {
		// check the file is actually loaded
		if (isFileLoaded) {
			std::vector<std::string> splitString = SplitString(objectName, '.');	// this gives us the stack of node names to use to traverse the json file's structure, e.g. root.head.value
			rapidjson::Value* jsonValue = nullptr;
			// Iterate through our substrings to traverse the JSON DOM
			const size_t sizeOfSplitString = splitString.size();
			for (size_t i = 0; i < sizeOfSplitString; i++) {
				if (i == 0) {
					if (!jsonDocument->HasMember(splitString.front().c_str())) {
						std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString.front() << std::endl;
						return;
					}
					jsonValue = &(*jsonDocument)[splitString.front().c_str()];	// Get our root key
				}
				else {
					if (!jsonValue->IsArray()) {
						if (jsonValue->HasMember(splitString[i].c_str())) {
							jsonValue = &(*jsonValue)[splitString[i].c_str()];
						}
						else {
							std::cout << "JsonFile.hpp >>>> Could not find key: " << splitString[i] << std::endl;
							return;
						}
					}
					else {
						// Point to the object/key/array at the indicated index in the array
						int arraySize = jsonValue->Size();
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
								jsonValue = &(*jsonValue)[indexOfValue];
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
			if (jsonValue->IsObject()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is an object" << std::endl;
				return;
			}
			// Check we haven't ended up with a key that isn't an array
			if (!jsonValue->IsArray()) {
				std::cout << "JsonFile.hpp >>>> " << objectName << " is not an array" << std::endl;
				return;
			}

			// Clear the array
			jsonValue->SetArray();
			// iterate through the passed vector and push each element to the json document
			for (const T& item : inputValueArray) {
				jsonValue->PushBack(item, jsonDocument->GetAllocator());
			}

			// If we've successfully set the value, save the doc
			if (!Save()) {
				std::cout << "JsonFile.hpp >>>> Failed to save file" << std::endl;
			}
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

// Add Functions exposed by the API, objectName should use the schema: key.key.index.value, etc.
template<typename T> inline void JsonFile::Add(const std::string & objectName, const T & inputValue) {

}
template<typename T> inline void JsonFile::AddArray(const std::string & objectName, const std::vector<T>& inputValueArray) {

}

// Remove Functions exposed by the API, objectName should use the schema: key.key.index.value, etc.
template<typename T> inline void JsonFile::Remove(const std::string & objectName) {

}
#endif