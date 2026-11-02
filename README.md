# ORBIT

ORBIT is a JUCE instrument plugin. This first milestone establishes VST3 and
standalone targets, a MIDI-responsive test oscillator, a host-automatable master
gain parameter, and parameter state save/restore. The instrument supports mono
and stereo output layouts for broader DAW compatibility. The React editor,
sampler engine, and instrument formats are later milestones.

On macOS, the VST3 bundle is built at
`build/ORBIT_artefacts/Release/VST3/ORBIT.vst3`. Install it in
`~/Library/Audio/Plug-Ins/VST3` and rescan VST3 plugins in LUNA. The CI
builds a universal arm64/x86_64 bundle, verifies its ad-hoc signature, and
runs pluginval at strictness level 5. Ad-hoc signing is for validation; public
macOS distribution requires Developer ID signing and notarization.

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