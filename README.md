## Eq
<div>
A simple but fully functional equalizer plugin made with the Juce framework, customizing Juce's LookAndFeel class and the behavior of some components. Available as VST3 (Linux, Mac and Windows) and AudioUnit (Mac). Work in progress.
</div>
<br>
<div align="center">
    <img src="https://github.com/witte/Eq/workflows/linux/badge.svg" />
    <img src="https://github.com/witte/Eq/workflows/mac/badge.svg" />
    <img src="https://github.com/witte/Eq/workflows/windows/badge.svg" />
</div>
<br>
<div align="center">
    <img src="Screenshot.png"/>
</div>
<br>

## Dependencies
Juce: https://github.com/WeAreROLI/JUCE ('juce6' branch, already included as a submodule)
<br>
<br>

## Building
```
git clone https://github.com/witte/Eq.git --recurse-submodules
cd Eq
cmake -S . -B build
cmake --build build        # Plugin will be at 'build/Eq_artefacts/VST3'
```
<br>

## Credits
The spectrum analyzer has code from https://github.com/ffAudio/Frequalizer for the fifo's and the overall analyzer's curve drawing.

The main font is Steve Matteson's Open Sans Condensed:<br>
https://fonts.google.com/specimen/Open+Sans+Condensed

Icons from Fefanto's FontAudio:<br>
https://github.com/fefanto/fontaudio
<br>
<br>

## License
GPL-3.0


