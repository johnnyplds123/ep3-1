# Build environment and tools
  - Operating system:
    - `Windows 8` or higher version
    - `Ubuntu 16.04` or higher version

  - Compiler
    - ARM compiler `v5.06` or higher version
    - Contact ARM dealer for ARM compiler

  - Configuration parser and script
     - Python `2.7.11` or higher version
     - Visit http://www.python.org website to download Python.

# How to build (Windows)
  Run **m** batch file in the project base directory.
```
  -------------------------------------------------------------------------------------
  Usage: m  [option]
  -------------------------------------------------------------------------------------
    Option:
    ramdisk   Build RAMDISK firmware
    uart      Build UART download firmware
    pcie      Build PCIe download firmware
    aging     Build Aging firmware
    batch     Batch build firmware
    test      Test build for gitlab-ci

  -------------------------------------------------------------------------------------
  Clean example:
  -------------------------------------------------------------------------------------
    clean
```
# How to build (Linux)
  Run **python ./build.by** script in the project base directory.
```
  -------------------------------------------------------------------------------------
  Usage: python ./build.py [option]
  -------------------------------------------------------------------------------------
    Option:
    ramdisk   Build RAMDISK firmware
    uart      Build UART download firmware
    pcie      Build PCIe download firmware
    aging     Build Aging firmware
    batch     Batch build firmware
    test      Test build for gitlab-ci
```

# Output files
  - _out\bin\boot.axf     Real view debugger file for **Boot Code**
  - _out\bin\cpu0.axf     Real view debugger file for **CPU0**
  - _out\bin\cpu1.axf     Real view debugger file for **CPU1**
  - _out\bin\cpu2.axf     Real view debugger file for **CPU2**     
  - `Bin\Eldora.bin` LiteON downloadable firmware image

# How to burn code
  - Blank serial flash or boot fail from serial flash boot mode
  - Boot from UART download boot
  - Download UARTXXX.bin file by XModem protocol
  - After finishing download, MON> prompt came out from serial
  - Send **downfw** Serial Monitor command
  - Send **Eldora.bin** file by XModem protocol.
  - Set boot mode to serial flash boot.
  - Turn the power off and turn on.

   You don't need to enter UART boot mode again to download new code to serial flash.
   Use **downfw** command.
