# Keyboard Synthesizer using PSOC Creator
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/amaurilopez90/Embedded-Systems/master/LICENSE)

**Inspiration:** Music has always been of particular interest to me, and being able to produce sounds similar to an instrument has always been an idea. This keyboard based music synthesizer serves as a preliminary project idea that wil later evolve into a sampled sound sythesizer used for my senior project design. This systhesizer was developed onto a Cypress PSOC board using PSOC Creator, utilizing tactile push buttons for user input and auxilary speaker from VDAC out for output. 

**Summary of Implementation Methedology:** This device outputs a sound similar to that of a flute or electric guitar, depending on the selected mode, to the notes aligned to a C-Major scale supporting 8 semitones or half-steps. This device waits for user input via GPIO button press, and depending on the mode, then generates a sign or square wave at a set frequency (corresponding to which button is pressed) to be output through a digital-to-analog converter. The square waves generated seem to mimic the sounds typically made by an electric guitar, and the sign wave that of a flute. 

