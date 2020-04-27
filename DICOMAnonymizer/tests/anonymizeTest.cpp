#include <gtest/gtest.h>
#include <fstream>
#include <array>
#include <cstring>
#include <stdio.h>
#include "utils.h"
#include "stripDataset.h"

size_t getFileSize(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    const auto begin = file.tellg();
    file.seekg (0, std::ios::end);
    const auto end = file.tellg();
    const auto fileSize = (end-begin);    
    return fileSize;
}
// Construct a path to a test image from an image name.
static std::string getTestImagePath(const std::string& imageName)
{
    const int max_path = 256;
    std::array<char,max_path> buff{0};
    auto used = readlink("/proc/self/exe", buff.data(), max_path);
    if(used <=0)
        throw std::runtime_error("readlink cannot find path to executable.");
    char* pos = std::strrchr(buff.data(),'/');
    if(!pos)
        throw std::runtime_error("Invalid path received from readlink.");
    *pos = 0;
    strcat(buff.data(),"/../../test_images/");
    strcat(buff.data(),imageName.c_str());
    std::array<char, max_path> relpath;
    if(realpath(buff.data(), relpath.data())==nullptr)
    {
        throw std::runtime_error("realpath cannot resolve image path");
    }
    std::string imagePath(relpath.data());
    return imagePath;
}

static std::shared_ptr<DcmFileFormat> loadDICOMFile(const std::string& filePath)
{
    std::ifstream imageFile(filePath, std::ifstream::binary);
    EXPECT_TRUE(imageFile.good());
    // load file data to a buffer
    std::streampos fileSize;
    imageFile.seekg(0, std::ios::end);
    fileSize = imageFile.tellg();
    imageFile.seekg(0, std::ios::beg);
    // reserve capacity
    std::vector<uint8_t> inData;
    inData.resize(fileSize);
    imageFile.read((char*)inData.data(),inData.size());
    size_t read = imageFile.gcount();
    // create a dcmtk file instance from memory buffer
    std::shared_ptr<DcmFileFormat> dicom;
    OFCondition res = loadFromMemoryBuffer(inData.data(), inData.size(), dicom);
    EXPECT_TRUE(res.good());
    EXPECT_TRUE(dicom);
    return dicom;
}

static void saveDICOMFile(std::shared_ptr<DcmFileFormat> dicom, int dataSize, const std::string& filePath)
{
    std::vector<uint8_t> outData(dataSize*2);
    size_t written = 0;
    auto res = saveToMemoryBuffer(dicom, outData.data(), outData.size(), written);
    EXPECT_TRUE(res.good());
    std::ofstream outputFile(filePath, std::ofstream::binary);
    outputFile.write((char*)outData.data(), written);
    EXPECT_TRUE(outputFile.good());
}

TEST(Anonymize, loadFile)
{
    std::string imagePath = getTestImagePath("US-MONO2-8-8x-execho.dcm");
    auto dicom = loadDICOMFile(imagePath);
    ASSERT_TRUE(dicom);
    auto dataset = dicom->getDataset();
    ASSERT_TRUE(dataset!=nullptr);
    auto metainfo = dicom->getMetaInfo();
    ASSERT_TRUE(metainfo!=nullptr);
    EXPECT_LT(0, dataset->card());
    EXPECT_LT(0, metainfo->card());
}

TEST(Anonymize, saveFile)
{
    std::string imagePath = getTestImagePath("US-MONO2-8-8x-execho.dcm");
    auto dicom = loadDICOMFile(imagePath);
    ASSERT_TRUE(dicom);
    auto fileSize = getFileSize(imagePath);
    ASSERT_LT(0,fileSize);
    std::string outputImagePath = std::tmpnam(nullptr);
    saveDICOMFile(dicom, fileSize, outputImagePath);
    fileSize = getFileSize(outputImagePath);
    ASSERT_LT(0,fileSize);
    auto dicom2 = loadDICOMFile(outputImagePath);
    fileSize = getFileSize(imagePath);
    ASSERT_TRUE(dicom2);
    remove(outputImagePath.c_str());
    // compare dicom datasets
    EXPECT_EQ(dicom->getDataset()->card(),dicom2->getDataset()->card());
    EXPECT_EQ(dicom->getMetaInfo()->card(),dicom2->getMetaInfo()->card());
}

// checks if an item contains confidential information
static bool hasConfidentialInfo(DcmItem* item)
{
    for (unsigned long i = 0; i < item->card(); i++)
    {
        DcmElement* element = item->getElement(i);
        if (element != NULL) {
            DcmTag tag(element->getTag());
            if(isConfidential(tag))
                return true;
        }
    }
    return false;
}

TEST(Anonymize, stripPrivateTags)
{
    const std::string imagePath = getTestImagePath("US-MONO2-8-8x-execho.dcm");
    auto fileSize = getFileSize(imagePath);
    ASSERT_LT(0,fileSize);
    auto dicom = loadDICOMFile(imagePath);
    ASSERT_TRUE(dicom);
    EXPECT_TRUE(hasConfidentialInfo(dicom->getDataset()));
    auto res = stripPrivateTags(dicom, 0);
    ASSERT_TRUE(res.good());
    std::string outputImagePath = std::tmpnam(nullptr);
    saveDICOMFile(dicom, fileSize, outputImagePath);
    fileSize = getFileSize(outputImagePath);
    ASSERT_LT(0,fileSize);
    auto dicom2 = loadDICOMFile(outputImagePath);
    fileSize = getFileSize(imagePath);
    ASSERT_TRUE(dicom2);
    EXPECT_FALSE(hasConfidentialInfo(dicom2->getDataset()));
    remove(outputImagePath.c_str());
}