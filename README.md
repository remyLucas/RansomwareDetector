# RansomwareDetector

This project is about using windows mini-filter driver for detecting ransomware execution.

# doc

You can find in the doc folder some notes in french about windows drivers.

# driver

I made a visual studio project in the folder FsFilter1 which is a mini-filter driver that log process I/O on all the file system present.

This project is experimental and this is the first time I deal with windows driver so the code might be messy

# I/O analysis

In the graph folder I made a html/javascrip page that show graph of I/O by process and a simple program that parse raw log file from the mini-filter driver to display them on the page
