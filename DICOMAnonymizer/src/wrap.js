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
// studyId: string with fake study id to replace the original id
// seriesId: string with fake series id to replace the original id
// sopId: string with fake sop id to replace the original id
// patientId: string with fake patient id to replace the original id
// tracingLevel (number): tracing level. Controls output of tracing
//                        information to the console.
//                        0: No information
//                        1: Function call only
//                        2: Detailed information.  
function anonymizeFile(input, studyId, seriesId, sopId, patientId, tracingLevel)
{
    // Allocate memofy for the output data
    const newBufferLen = input.byteLength*2;
    var output = Module._malloc(newBufferLen);
    var outputData = null;

    const newLen = Module.ccall(
        'anonymizeFile',    // Name of the C++ function
        'number',           // Return value (a Boolean indicating success)
        [                   // The list of arguments
            'array',    
            'number',
            'number',
            'number',
            'string',
            'string',
            'string',
            'string',
            'number'
        ],
        [                   // The value of the arguments
            input,
            input.byteLength,
            output,
            newBufferLen,
            studyId,
            seriesId,
            sopId,
            patientId,
            tracingLevel
        ]
    );
    if(newLen>0)
    {
        // Put output data to a Uint8Array
        var tempArray = new Uint8Array(Module.HEAPU8.buffer, output, newLen);
        outputData = tempArray.slice(0);
    }
    Module._free(output);
    return outputData;
}

function getFakeId(type)
{
    // Allocate memofy for the output data
    const idMaxLen = 100;
    var idBuffer = Module._malloc(idMaxLen);
    var id = null;

    const idLen = Module.ccall(
        'getFakeId',    // Name of the C++ function
        'number',           // Return value (a Boolean indicating success)
        [                   // The list of arguments
            'number',
            'number',    
            'number'
        ],
        [                   // The value of the arguments
            type,
            idBuffer,
            idMaxLen
        ]
    );
    if(idLen>0)
    {
        // Put output data to a Uint8Array
        id = Module.UTF8ToString(idBuffer);
    }
    Module._free(idBuffer);
    return id;
}

  