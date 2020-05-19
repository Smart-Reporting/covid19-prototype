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
document.getElementById('generate').addEventListener('submit', function(e) 
{
    document.getElementById("studyid").value = getFakeId(0);
    document.getElementById("seriesid").value = getFakeId(1);
    document.getElementById("sopid").value = getFakeId(2);
    document.getElementById("patientid").value = getFakeId(3);
    // Prevent the actual uploading of the form
    e.preventDefault();
});

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
            // collect new ids 
            studyId = document.getElementById("studyid").value
            seriesId = document.getElementById("seriesid").value
            sopId = document.getElementById("sopid").value
            patientId = document.getElementById("patientid").value
            // Do anonymization
            var output = anonymizeFile(input, studyId, seriesId, sopId, patientId, 1);
            if (output==null)
            {
                alert('Sorry, unable to process the DICOM file');
            }
            else
            {
                // Display anonymized info on the WEB page.
                printTags(output, "after");
                console.log("Sending " + output.byteLength + " bytes");
                // Send anonymized content to the orthanc server
                sendData(output);
            }
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
