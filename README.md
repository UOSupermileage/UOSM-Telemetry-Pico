# Setup
```bash
# Clone the project
git clone git@github.com:UOSupermileage/UOSM-Telemetry-Pico.git

# Go into the folder
cd UOSM-Telemetry-Pico

# This might take a while...
git submodule update --init --recursive 
```

# Project Structure
## Dependencies
The project depends on the FreeRTOS kernel and the Pico SDK. They are both submodules for the project.

The project also depends on UOSM-Core. This is added as a subtree to allow version control.
