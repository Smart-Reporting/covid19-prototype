//
// Created by stas on 21.04.20.
//
#include "utils.h"
#include <dcmtk/dcmdata/dcostrmb.h>
#include <dcmtk/dcmdata/dcwcache.h>


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

