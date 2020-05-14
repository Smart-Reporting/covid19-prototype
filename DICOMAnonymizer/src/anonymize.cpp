#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <iostream>
#include <vector>
#include "stripDataset.h"
#include "utils.h"
#include <dcmtk/dcmdata/dcostrmb.h>
#include <dcmtk/dcmdata/dcdeftag.h>

extern "C" void addTagJS(const char* parentHtmlTag, const char* tag, bool confidential);
extern "C" void showErrorMessageJS(const char* errorMessage);


// Loop over the tags contained in a DICOM dataset, convert them as
// strings, and fill the HTML page with these strings.
static void sendTagsToJavascript(DcmItem* item, const char* parentHtmlTag)
{
    std::cout << "sendTags: " << parentHtmlTag << std::endl;
    if (item != NULL)
    {
        for (unsigned long i = 0; i < item->card(); i++)
        {
            DcmElement* element = item->getElement(i);
            if (element != NULL)
            {
                char buf[64];
                sprintf(buf, "(%04x,%04x) ", element->getGTag(), element->getETag());

                // Copy the tag to ensure const-correctness
                DcmTag tag(element->getTag());
                bool red = isConfidential(tag);  
                std::string name = std::string(buf) + std::string(tag.getTagName()) + ": ";
                
                char* value = NULL;
                if (element != NULL)
                {
                    if (element->isaString() &&
                        element->getString(value).good() &&
                        value != NULL)
                    {
                        std::string tag = name + std::string(value);
                        addTagJS(parentHtmlTag, tag.c_str(), red);
                    }
                    else
                    {
                        std::string tag = name + "Not a string";
                        addTagJS(parentHtmlTag, tag.c_str(), red);
                    }
                }
            }
        }
    }
}

// Notiy user about an error.
// The function passes the message to 
// a javascript funtion for displayng of
// the message on the WEB pagh
static void showErrorMessage(
    const char* errorMessage // error message
    )
{
    showErrorMessageJS(errorMessage);
}

// Prints all DICOM tags of a DICOM dataset to a WEB page
// Tag information is added one after another
// as child html to a parent html tag specified
// by the parentHtmlTag parameter.
extern "C" bool EMSCRIPTEN_KEEPALIVE printTags(
    const void* inputData,      // Memory buffer containing the DICOM dataset (read from a file for ex.)
    size_t inputLength,         // Length of the memory buffer in bytes.
    char* parentHtmlTag         // HTML tag name to output DICOM tag information
    )
{
    std::cout << "Html tag: " << parentHtmlTag << std::endl;
    bool ret(true);
    std::shared_ptr<DcmFileFormat> dicom;
    OFCondition res = loadFromMemoryBuffer(inputData, inputLength, dicom);
    if(!dicom || res.bad())
    {
        ret = false;
        showErrorMessage("Cannot parse the DICOM dataset.");
        showErrorMessage(res.text());
    }
    else
    {
        sendTagsToJavascript(dicom->getMetaInfo(), parentHtmlTag);
        sendTagsToJavascript(dicom->getDataset(), parentHtmlTag);
    }   
    return ret;
}

// Performes anonimyzation of a DICOM file.
// The function reads a DICOM dataset from a 
// byte array, performes anomymization and
// saves the dataset to an output buffer.
// Format of the input and ouput buffets
// corresponds DICOM file format.
extern "C" size_t EMSCRIPTEN_KEEPALIVE anonymizeFile(
    const void* inputData,  // input buffer with DICOM data
    size_t inputLength,     // length of the input buffet
    void* outputData,       // Output buffer for saving of anonymized data
    size_t outputLength,    // Size of the output buffer.
    char* studyId,          // Study id
    char* seriesId,         // Series id
    char* SOPId,            // SOP id
    char* patientId,        // Patient id
    int tracingLevel        // 0, 1, or 2: tracing level
    )
{
    typedef std::tuple<DcmTag,std::string> KeyVal;
    size_t written = 0;
    if(tracingLevel>0)
    {
        std::cout << "---------anonymizeFile Begin. Received bytes: " << inputLength << std::endl;
        std::cout << "Output buffer size: " << outputLength << std::endl;
        std::cout << "Study id: " << studyId << std::endl;
        std::cout << "Series id: " << seriesId << std::endl;
        std::cout << "SOP id: " << SOPId << std::endl;
        std::cout << "Patient id: " << patientId << std::endl;
    }
    if(inputData && inputLength)
    {
        std::shared_ptr<DcmFileFormat> dicom;
        OFCondition res = loadFromMemoryBuffer(inputData, inputLength, dicom);
        if(dicom && res.good()) 
        {
            if(tracingLevel>0)
            {
                std::cout << "DICOM file successfuly loaded" << std::endl;
            }
            // strip confedential tags
            res = stripPrivateTags(dicom, tracingLevel);
            if(res.bad())
            {
                std::cout << "Error by removing of confidential tags:" << res.text() << std::endl;
            }
            else if(tracingLevel>0)
            {
                std::cout << "Confidential tags are removed." << std::endl;
            }
            if(res.good())
            {
                // replace important DICOM ids
                std::list<KeyVal> tagList;
                tagList.push_back(KeyVal(DcmTag(DCM_StudyInstanceUID,DcmVR(EVR_UI)), studyId));
                tagList.push_back(KeyVal(DcmTag(DCM_SeriesInstanceUID,DcmVR(EVR_UI)), seriesId));
                tagList.push_back(KeyVal(DcmTag(DCM_SOPInstanceUID,DcmVR(EVR_UI)), SOPId));
                tagList.push_back(KeyVal(DcmTag(DCM_PatientID,DcmVR(EVR_LO)), patientId));
                res = modifyDataset(dicom->getDataset(), tagList, tracingLevel);
                if(res.bad())
                {
                    std::cout << "Error by modification of the dataset:" << res.text() << std::endl;
                }
                else if(tracingLevel>0)
                {
                    std::cout << "Modification is succesful" << std::endl;
                }
            }
            if(res.good())
            {
                // serialize the dataset to the memory buffer
                res = saveToMemoryBuffer(dicom, outputData, outputLength, written);
                if(res.bad())
                {
                    std::cout << "Error by dataset serialization:" << res.text() << std::endl;
                }
                else if(tracingLevel>0)
                {
                    std::cout << "Dataset serialization is succesful" << std::endl;
                }
            }
            // report error
            if(res.bad())
            {
                written = 0;
                showErrorMessage("Failed to save DICOM data to the buffer");
                showErrorMessage(res.text());
            }

        }
        else
        {
            showErrorMessage("Failed to load DICOM data from the buffer");
            showErrorMessage(res.text());
        }
    }
    else
    {
        showErrorMessage("Invalid data received.");
    }
    if(tracingLevel>0)
    {
        std::cout << "---------anonymizeFile End" << std::endl;
    }
    return written;
}

extern "C" int EMSCRIPTEN_KEEPALIVE getFakeId(
    int type,      // Id type: 0: studyId, 1: series id, 2: sop id, 3: patient id
    char* id,      // Buffer for id
    int buffLen    // Length of id buffer
    )
{
    std::string strId;
    if(type==0){
        strId = generateDICOMId(DCM_StudyInstanceUID);
    }
    else if(type==1){
        strId = generateDICOMId(DCM_SeriesInstanceUID);
    }
    else if(type==2){
        strId = generateDICOMId(DCM_SOPInstanceUID);
    }
    else if(type==3){
        strId = generateDICOMId(DCM_PatientID);
    }
    if(!strId.empty()){
        strcpy(id, strId.c_str());
    }
    return (int)strlen(id);
}

#endif