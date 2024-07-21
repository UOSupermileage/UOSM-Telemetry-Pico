# Setup
1. Download [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
2. Initialize submodules

```bash
git clone https://github.com/raspberrypi/pico-sdk
cd pico-sdk
git submodule update --init 
```

3. Configure PICO_SDK_PATH environment variable. In CLion, you can go to Settings > CMake > ProfileName > Environment