//
// Created by stas on 21.04.20.
//
#include <iostream>
#include <vector>
#include <iterator>
#include "stripDataset.h"
#include "utils.h"

void loadFormMemory(const std::string& inputPath, const std::string& outputPath)
{
    std::ifstream inputFile(inputPath, std::ifstream::binary);
    // Stop eating new lines in binary mode!!!
    inputFile.unsetf(std::ios::skipws);
    // get its size:
    std::streampos fileSize;
    inputFile.seekg(0, std::ios::end);
    fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    // reserve capacity
    std::vector<uint8_t> inData;
    inData.resize(fileSize);
    inputFile.read((char*)inData.data(),inData.size());
    size_t read = inputFile.gcount();
    std::shared_ptr<DcmFileFormat> dicom;
    OFCondition res = loadFromMemoryBuffer(inData.data(), inData.size(), dicom);
    stripPrivateTags(dicom, 2);
    std::vector<uint8_t> outData(inData.size()*2);
    size_t written = 0;
    res = saveToMemoryBuffer(dicom, outData.data(), outData.size(), written);
    std::ofstream outputFile(outputPath, std::ofstream::binary);
    outputFile.write((char*)outData.data(), written);

}
void loadFromFile(const std::string& filePath)
{
    std::shared_ptr<DcmFileFormat> dicom(new DcmFileFormat);
    OFCondition res = dicom->loadFile(filePath.c_str());
    res = dicom->loadAllDataIntoMemory();
    dicom->transferInit();
    res = dicom->saveFile("/home/stas/projects/test1.dcm");
    dicom->transferEnd();
}

int main()
{
    const std::string inputFile = "/home/stas/projects/1.2.840.113704.1.111.7064.1210149189.4528.dcm";
    const std::string outputFile = "/home/stas/projects/test2.dcm";
    loadFormMemory(inputFile, outputFile);
    return 0;
}
