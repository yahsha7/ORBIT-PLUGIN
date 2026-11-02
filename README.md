# ORBIT

ORBIT is a JUCE instrument plugin. This first milestone establishes VST3 and
standalone targets, a MIDI-responsive test oscillator, a host-automatable master
gain parameter, and parameter state save/restore. AU is added automatically on
macOS. The React editor, sampler engine, and instrument formats are later
milestones.

The project uses the JUCE source tree in its parent directory when available;
otherwise it looks for an installed JUCE package. From this directory,
configure and build with:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target ORBIT_VST3 ORBIT_Standalone OrbitTests
ctest --test-dir build --output-on-failure
```

To use a JUCE source tree elsewhere, configure with
`-DORBIT_JUCE_SOURCE_DIR=/path/to/JUCE`. To use an installed JUCE package,
configure with `-DORBIT_JUCE_SOURCE_DIR=` and make its CMake package
discoverable.

JUCE is dual-licensed under the AGPLv3 and a commercial license. Review
`../LICENSE.md` and the JUCE license terms before distributing a product that
uses JUCE.