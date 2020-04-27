#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <memory>

// strips DICOM file from confidential data.
OFCondition stripPrivateTags(std::shared_ptr<DcmFileFormat> file, int traceLevel);
// checks if a tag is confidential
bool isConfidential(const DcmTagKey& key);
