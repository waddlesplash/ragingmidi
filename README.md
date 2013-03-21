# Raging MIDI
![(Raging MIDI playing a file)](http://sourceforge.net/projects/ragingmidi/screenshots/Shot1.png "Raging MIDI playing a file")

The project's goal is to make a complete, open-source, cross-platform replacement of Anvil Studio.
Right now, it's little more than a powerful player.

## Getting started
To build Raging MIDI, you need the [Qt](http://qt-project.org/) and OpenGL libraries (optional, but it will eat your CPU if there is no OpenGL-2D).

You might want to read the source code documentation, which for now you can get by installing [ROBODoc](http://robodoc.sourceforge.net/) and running `robodoc --rc robodoc.rc` in the root repository directory.

## TODO
A lot. Right now there's no real todo list for all the small stuff (too much of it) but here are the major problems
 - No Mac OSX support. (QMidi doesn't support Mac, as I have no access to one. Anyone?)
 - No package generation anywhere but Windows (because I don't know anything about packaging anywhere else)
 - No BSD (OSS MIDI) support (because I can't find good docs for it)
 - No built-in synthesizer (TODO: fork `fluidsynth` to Qt/C++ and made a library out of it)
