//
// Created by stas on 21.04.20.
//

#ifndef DICOMANONYMIZER_UTILS_H
#define DICOMANONYMIZER_UTILS_H
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/ofstd/ofcond.h>
#include <memory>
// loads DICOM file data from a memory buffer and returns it as an instance of dcmtk file
OFCondition loadFromMemoryBuffer(const void* buffer, size_t size, std::shared_ptr<DcmFileFormat>& dicom);
// saves DICOM dcmtk file instance as a DICOM file to a memory buffer.
OFCondition saveToMemoryBuffer(std::shared_ptr<DcmFileFormat> dicom, void* buffer, size_t size, size_t& written);
// create error return condition
inline OFCondition dcmtkError(const char* errorMessage)
{
    return makeOFCondition(0, 0, OF_failure, errorMessage);
}
#endif //DICOMANONYMIZER_UTILS_H
