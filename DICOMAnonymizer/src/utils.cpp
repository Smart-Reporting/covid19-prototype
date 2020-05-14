//
// Created by stas on 21.04.20.
//
#include "utils.h"
#include <dcmtk/dcmdata/dcostrmb.h>
#include <dcmtk/dcmdata/dcwcache.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <list>


// Use DCMTK to parse a memory buffer containing a DICOM file. This
// code comes from Orthanc.
OFCondition loadFromMemoryBuffer(
    const void* buffer,                     // buffer wiht DICOM data
    size_t size,                            // size of the buffer
    std::shared_ptr<DcmFileFormat>& dicom   // output instance of dcmtk file
    )
{
    if(!buffer || size<=0)
    {
        return dcmtkError("Invalid input buffer");
    }

    DcmInputBufferStream is;
    is.setBuffer(buffer, size);
    is.setEos();

    dicom.reset(new DcmFileFormat);
    dicom->transferInit();
    OFCondition result = dicom->read(is);

    if (result.good())
    {
        result = dicom->loadAllDataIntoMemory();
        dicom->transferEnd();
    }
    if(result.bad())
    {
        dicom.reset();
    }
    return result;
}
// Saves an instance of a dcmtk file to 
// a memory buffer in format of a DICOM file.
OFCondition saveToMemoryBuffer(
    std::shared_ptr<DcmFileFormat> dicom, // input instance of a dcmtk file
    void* buffer,                         // buffer to save the data
    size_t size,                          // size of the buffer in bytes
    size_t& written                       // number of bytes written to the buffer
    )
{
    DcmOutputBufferStream out(buffer, size);
    DcmWriteCache cache;
    dicom->transferInit();
    OFCondition res = dicom->write(out, EXS_Unknown,
            EET_UndefinedLength, &cache, EGL_recalcGL);
    dicom->transferEnd();

    if(res.good())
    {
        void *bf(nullptr);
        offile_off_t writtens(0);
        out.flushBuffer(bf,writtens);
        written = (size_t)writtens;
    }

    return res;
}

char* genRandomString(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
    return s;
}

std::string generateDICOMId(const DcmTagKey& idKey)
{
    char buffer[100] = {0};
    std::string id;
    if(idKey==DCM_StudyInstanceUID){
        id = dcmGenerateUniqueIdentifier(buffer, SITE_STUDY_UID_ROOT);
    }
    else if(idKey==DCM_SeriesInstanceUID){
        id = dcmGenerateUniqueIdentifier(buffer, SITE_SERIES_UID_ROOT);
    }
    else if(idKey==DCM_SOPInstanceUID){
        id = dcmGenerateUniqueIdentifier(buffer, SITE_INSTANCE_UID_ROOT);
    }
    else if(idKey==DCM_PatientID){
        id = genRandomString(buffer, 20);
    }
    return id;
}

OFCondition modifyDataset(DcmItem* item, std::list<std::tuple<DcmTag,std::string>> tagList, int tracingLevel)
{
    OFCondition res;
    for(auto it=tagList.begin(); it!=tagList.end(); ++it)
    {
        DcmTag tag = std::get<0>(*it);
        const std::string& value = std::get<1>(*it);
        if(tracingLevel>1)
        {
            std::cout << tag.toString().c_str() << ":" << value << std::endl;
        }
        res = item->putAndInsertString(tag, value.c_str(), 1);
        if(res.bad())
        {
            std::cout << "Error by assigning new value to tag" 
                << tag.toString().c_str() << ":" << value << std::endl;
            std::cout << res.text() << std::endl;
            return res;
        }
    }
    return res;
}