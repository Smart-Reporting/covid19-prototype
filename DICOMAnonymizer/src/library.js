/**
 * Parsing DICOM using WebAssembly
 * Copyright (C) 2017 Sebastien Jodogne <s.jodogne@gmail.com>, Osimis,
 * Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


// This is the library of the "extern" JavaScript functions that can
// be called from the C++ code

mergeInto(LibraryManager.library, 
{
  // The "AddTag()" function append one line to the "Headers" or
  // "Tags" section of the HTML page
  addTagJS: function(parentHtmlTagNameUTF8, s, confidential) 
  {  
      // Convert the C string "const char*" to a JavaScript string, and
      // escape it to avoid HTML entities
      var pref = "<div>"
      var suf = "</div>"
      if(confidential)
      {
        pref = "<div style=\"color:red\">"
      }
      const content = pref + EscapeHtml(UTF8ToString(s)) + suf;
      const parentHtmlTagName = UTF8ToString(parentHtmlTagNameUTF8);
      const parentHtmlTag = document.getElementById(parentHtmlTagName);
      if(parentHtmlTag==null)
      { 
        console.error("Cannot find HTML tag " + parentHtmlTagName + " to output information");
      }
      else
      {
        parentHtmlTag.innerHTML += content;
      }
  },
  // display error message on the WEB page
  // Each error message is added as a separated
  // div element to the "errors" HTML element
  showErrorMessageJS: function(errorMessageUTF8) 
  {  
      // Convert the C string "const char*" to a JavaScript string, and
      var pref = "<div>"
      var suf = "</div>"
      const errorMessage = pref + EscapeHtml(UTF8ToString(errorMessageUTF8)) + suf;
      const parentHtmlTagName = "errors";
      const parentHtmlTag = document.getElementById(parentHtmlTagName);
      if(parentHtmlTag==null)
      { s
        console.error(errorMessage);
      }
      else
      {
        parentHtmlTag.innerHTML += errorMessage;
      }
  }  
});
