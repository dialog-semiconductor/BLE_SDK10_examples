#!/usr/bin/env python

# ****************************************************************************************
#
# @file bootstrap.py
#
# @brief LCD Controller Demo application
#
# This software ("Software") is owned by Dialog Semiconductor. By using this Software
# you agree that Dialog Semiconductor retains all intellectual property and proprietary
# rights in and to this Software and any use, reproduction, disclosure or distribution
# of the Software without express written permission or a license agreement from Dialog
# Semiconductor is strictly prohibited. This Software is solely for use on or in
# conjunction with Dialog Semiconductor products.
#
# EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
# REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
# IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
# SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
#
# ****************************************************************************************

import os
import sys
import zipfile
import io
import urllib.request
import shutil

print("DA1469x emWin Demo bootstrap")

emWinSrcFolder  = '.'
emWinDestFolder = 'emwin_lib'

EMWIN_ARCHIVE_URL = "https://www.dialog-semiconductor.com/emwin_library_latest"

# go to the application folder
os.chdir(os.path.dirname(os.path.abspath(__file__)))

if not os.path.exists(emWinDestFolder):
    print("Downloading emWin library")
    response = urllib.request.urlopen(EMWIN_ARCHIVE_URL)
    emWinArchive = response.read()
    z = zipfile.ZipFile(io.BytesIO(emWinArchive))

    z.extractall('.')




