// HTML escape map
// for dilplaying of a text
// on a  WEB page
var ENTITY_MAP = {
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#39;',
    '/': '&#x2F;',
    '`': '&#x60;',
    '=': '&#x3D;'
  };
  // Function escapes html charachters before
  // displaying of the text on a WEB paghe.
  function EscapeHtml (string) {
    // http://stackoverflow.com/a/12034334/881731
    return String(string).replace(/[&<>"'`=\/]/g, function(s) {
      return ENTITY_MAP[s];
    });
  }

// A wrapper for the printTags WebAssembly function.
// Displays all DICOM tags from a DICOM file
// inside of a HTML element with id parentHtmlTag. 
// Parameters:
// input: UInt8Array with content of a DICOM file.
// parentHtmlTag: id of a HTML element where the information
// should be shown (as child div elements)
function printTags(input, parentHtmlTag)
{
    const done = Module.ccall(
        'printTags',    // Name of the C++ function
        'number',           // Return value (a Boolean indicating success)
        [                   // The list of arguments
            'array',    
            'number',
            'string'
        ],
        [                   // The value of the arguments
            input,
            input.byteLength,
            parentHtmlTag
        ]
    );
    return done!=0;
}

// Javascript wrapper for the "anonymizeFile" function from
// the WebAssembly. The function accepts a DICOM file content
// (as a UInt8Array), removes confidential DICOM tags and
// saves the dataset in an output buffer.
// Parameters:
// input (UInt8Array): content of a DICOM file to be anonymized.
// output: buffer for output anonymized content. Must be allocated
//         in the WebAssembly module heap.
// bufferLen (number): size of the output buffer in bytes.
// tracingLevel (number): tracing level. Controls output of tracing
//                        information to the console.
//                        0: No information
//                        1: Function call only
//                        2: Detailed information.  
function anonymizeFile(input, output, bufferLen, tracingLevel)
{
    const done = Module.ccall(
        'anonymizeFile',    // Name of the C++ function
        'number',           // Return value (a Boolean indicating success)
        [                   // The list of arguments
            'array',    
            'number',
            'number',
            'number',
            'number'
        ],
        [                   // The value of the arguments
            input,
            input.byteLength,
            output,
            bufferLen,
            tracingLevel
        ]
    );
    return done;
}
// Concatenates a text to the protocol element of the WEB
// page. Shows result of communication with a DICOM server.
function updateProtocol(text)
{
   document.getElementById("protocol").innerHTML += text + "<br/>";
}

// Shows respond of the DICOM server on the WEB page.
function updateRequestResult(text)
{
    document.getElementById("response").innerHTML = text;
}

// Send data to a DICOM server.
// Parameters:
// dicomData (Uint8Array): content of a DICOM file
function sendData(dicomData)
{
    var url = "http://localhost/orthanc/instances";
    updateProtocol("Sending data. Data size:" + dicomData.byteLength);
    var oReq = new XMLHttpRequest();
    oReq.withCredentials = true;
    oReq.open("POST", url, false, "orthanc", "orthanc");
    oReq.onload = function(e){
        console.log(oReq.response)
        updateProtocol("Response code:" + oReq.status);
        updateRequestResult(oReq.response);
    }
    oReq.send(dicomData);
    updateProtocol("Data is sent");

}
// This event is triggered when the user uploads a DICOM file
document.getElementById('upload').addEventListener('submit', function(e) 
{
    // Check whether a file has actually been selected
    var fileInput = document.getElementById('dicom');
    if('files' in fileInput && fileInput.files.length == 1)
    {
        // Create a reader to receive the file selected by the HTML form
        // https://developer.mozilla.org/fr/docs/Web/API/FileReader/onload
        var reader = new FileReader();
        reader.onload = function(event) 
        {
            // Put input data (content of a DICOM file) to a Uint8Array
            var dicom = this.result;
            console.log("Received " + dicom.byteLength + " bytes.")
            var input = new Uint8Array(dicom);
            // Print DICOM tags of received data to the WEB page
            printTags(input, "before");
            // Allocate memofy for the output data
            const bytesPerElement = Module.HEAPU8.BYTES_PER_ELEMENT;
            const newBufferLen = dicom.byteLength*2;
            var outputBuffer = Module._malloc(newBufferLen*bytesPerElement);
            // Do anonymization
            var newLen = anonymizeFile(input, outputBuffer, newBufferLen, 1);
            if (newLen<=0)
            {
                alert('Sorry, unable to process the DICOM file');
            }
            else
            {
                // Put output data to a Uint8Array
                var output = new Uint8Array(Module.HEAPU8.buffer, outputBuffer, newLen);
                // Display anonymized info on the WEB page.
                printTags(output, "after");
                console.log("Sending " + newLen + " bytes");
                // Send anonymized content to the orthanc server
                sendData(output);
            }
            Module._free(outputBuffer);
        };
        // Instruct JavaScript to load the file as an ArrayBuffer
        reader.readAsArrayBuffer(fileInput.files[0]);

    }
    else
    {
        alert('Please select one DICOM file');
    }
    // Prevent the actual uploading of the form
    e.preventDefault();
});
  