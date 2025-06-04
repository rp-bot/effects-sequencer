# Effects Sequencer

An audio effect which allows you to sequence dsp modules + AI chat bots

## Debug Builds

### Option 1

```sh
mkdir -p Build && cd Build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```
### Option 2

1. Install the CMake Tools extension in VSCode
2. Navigate to the CMake tab
3. Open up the Project Outline
4. Select your desired Target
5. Right Click (or the "Compile" Icon) and Build / 

## Release Builds

### Option 1

```sh
mkdir -p Build && cd Build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
### Option 2

1. Install the CMake Tools extension in VSCode
2. Navigate to the CMake tab
3. Open up the Project Outline
4. Select your desired Target
5. Right Click (or the "Compile" Icon) and Build / 

>(optional?) According to my tests, MacOS needs another step in order to make the VST work properly. 
## Code Signing (MacOS)

```sh
codesign --force --deep --sign - ~/Library/Audio/Plug-Ins/VST3/EffectSequencer.vst3
        # ^force replaces the temporary ad-hoc signature that CMake generates, If it exists. 
```
