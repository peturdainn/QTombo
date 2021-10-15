# QTombo - a Tombo implementation in Qt

## What?
My very first mobile device was a WinCE thing, and soon I discovered somebody (Tomohisa Hirami) had written an interesting note taking tool that could even encrypt: Tombo.
The project page is still up but activity has stopped.
https://tombo.osdn.jp/En/

So yeah, WinCE and a Windows client were nice and then I moved to Linux. The binary ran in Wine but soon I started to create a Linux client for myself. Because why not, and the original project was open source.

So here it is, a Linux client that can deal with the original tombo tree and file format.
That sounds complicated but it really isn't: A Tombo tree is just a directory tree, and the files are just text files. The encrypted files are blowfish encrypted with a header in front. Out of lazyness I re-used that bit from the original sources but later when creating a web version (PHPTombo) I wrote the header code too. Maybe one day I port it to C++

Find the PHP version for your webserver at https://github.com/peturdainn/PHPTombo

## Getting started
Open the project in Qt Creator 5 and build it :)

Then pick a place where you want to create your tree, and click the settings icon (next to About) to select that location.

When creating a file, there's a checkbox to make in encrypted. Each file can have its own key. Do not specify an extension.

Sorry, search is not implemented yet.
