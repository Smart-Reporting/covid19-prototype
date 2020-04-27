#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <iostream>
#include <vector>
#include "stripDataset.h"
#include "utils.h"
#include <dcmtk/dcmdata/dcostrmb.h>

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
    int tracingLevel        // 0, 1, or 2: tracing level
    )
{
    size_t written = 0;
    if(tracingLevel>0)
    {
        std::cout << "---------anonymizeFile Begin. Received bytes: " << inputLength << std::endl;
        std::cout << "Output buffer size: " << outputLength << std::endl;
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
            stripPrivateTags(dicom, tracingLevel);
            res = saveToMemoryBuffer(dicom, outputData, outputLength, written);
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
#endif