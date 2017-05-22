# coding=utf-8
# Script to upload files to Dropbox

# Import correct libraries
import base64
import sys
from temboo.core.session import TembooSession
from temboo.Library.Dropbox.Files import Upload

print str(sys.argv[1])

# Encode image
with open(str(sys.argv[1]), "rb") as image_file:
    encoded_string = base64.b64encode(image_file.read())

# Declare Temboo session and Choreo to upload files
session = TembooSession('ratulahmed', 'myFirstApp', 'EooH6NGW7ZAYe9iqsxnNAhkV7jPehjgW')
uploadFileChoreo = Upload(session)

# Get an InputSet object for the choreo
uploadFileInputs = uploadFileChoreo.new_input_set()

# Set inputs
#uploadFileInputs.set_AppSecret("l625itj68ec4pyy")
uploadFileInputs.set_AccessToken("RyzY2OBYynAAAAAAAAABhY5nYfF1q1dmQ3RAH0tvUMxW3vSuN48vFpE2L2yKg7YB")
uploadFileInputs.set_Path(str(sys.argv[1]))
#uploadFileInputs.set_AccessTokenSecret("yourTokenSecret")
#uploadFileInputs.set_AppKey("ekz2iughorivllz")
uploadFileInputs.set_FileContent(encoded_string)
#uploadFileInputs.set_Root("sandbox")

# Execute choreo
uploadFileResults = uploadFileChoreo.execute_with_results(uploadFileInputs)