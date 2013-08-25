# Raging MIDI
![(Raging MIDI playing a file)](http://sourceforge.net/projects/ragingmidi/screenshots/Shot1.png "Raging MIDI playing a file")

The project's goal is to make a complete, open-source, cross-platform replacement of Anvil Studio.
Right now, it's little more than a powerful player.

## Compiling
Qt libraries (either Qt4 or Qt5, whichever you have) are required on all systems. QtOpenGL is not required, but will make for some better performance. Install other dependencies listed below and use either Qt Creator or standard `qmake` make procedure.  
**NOTE: `make install` for Raging MIDI has not been implemented.**

**Other dependencies by system:**  
*Windows*: requires WinMM, development files come with MinGW. Untested on MSVC.  
*Linux*: requires ALSA. Packages: `libqt4-dev libasound2-dev fluidsynth`. To use a softsynth, run in a terminal: `fluidsynth -l -s -aalsa -o audio.alsa.device=default {PATH_TO_SF2_FILE}`  
*Mac OS X*: not supported yet, as I don't have a Mac to implement the required MIDI functions.  

You might want to read the source code documentation, which for now you can get by installing [ROBODoc](http://robodoc.sourceforge.net/) and running `robodoc --rc robodoc.rc` in the root repository directory.

## TODO
A lot. Right now there's no real todo list for all the small stuff (too much of it) but here are the major problems
 - No Mac OSX support. (QMidi doesn't support Mac, as I have no access to one. Anyone?)
 - No package generation anywhere but Windows (because I don't know anything about packaging anywhere else)
 - No BSD (OSS MIDI) support (because I can't find good docs for it)
 - No built-in synthesizer (TODO: fork `fluidsynth` to Qt/C++ and made a library out of it)
