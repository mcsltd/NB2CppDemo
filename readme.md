## NB2CppDemo
Demo program for working with the NB2 device family.

NB2-EEG16 is 16-channel DC mobile EEG, part of NeoRecCap Mobile System:\
https://mcscap.com/catalog/mobile-system-neoreccap/

NB2-EEG21 is 21-channel DC mobile EEG, part of NeoRecCap Mobile System:\
https://mcscap.com/catalog/mobile-system-neoreccap/

Full device description and technical specification: \
https://mcscap.com/upload/iblock/7ca/u1cowtzbssus5xs40adj6yj3nobatq38/003_5_201-NeoRec-cap-Data-sheet.pdf

## Description
This program is used to:
 - search and open NB2 device;
 - show technical information about device (serial number, prodaction date, software version and etc);
 - configure device (sets data rate, adc input range, enabled channels);
 - eeg asquition and peak-to-peak signal amplitude calculation;
 - channels impedance registration;
 - device state registration (battery change, ble utilization and etc).

## Requirements
 - OS: Windows 10/11
 - Memory: 256 Mb RAM
 - 64 Mb free space on disk
 - BLE adapter with Bluetooth 5.0 protocol support
 
 
## Usage
`NB2CppDemo.exe <mode> <data-rate> <input-range> <chs-enabled>`\
` <mode>        ` working mode: eeg (default), impedance, battery, start-record, stop-record or help\
` <data-rate>   ` eeg sampling rate in herz: 125 (default), 250, 500 or 1000\
` <input-range> ` adc input range in mV: 150 (default) or 300\
` <chs-enabled> ` comma-separated numbers of channels that ase used for eeg/impedance asquisition; \
` ` ` ` ` ` all channels are enabled by default; space in enumeration are not allowed\
All arguments are optional (see default values).\
Press `q` and `enter` for exit.

## Examples

1. Show help
```
> NB2CppDemo.exe help
NB2CppDemo - demo program for working with the NB2 device
Medical Computer Systems Ltd., 2022

Usage:
NB2CppDemo.exe <mode> <data-rate> <input-range> <chs-enabled>
 <mode>         working mode: eeg (default), impedance, battery, start-record, stop-record or help
 <data-rate>    eeg sampling rate in herz: 125 (default), 250, 500 or 1000
 <input-range>  adc input range in mV: 150 (default) or 300
 <chs-enabled>  comma-separated numbers of channels that ase used for eeg/impedance asquisition;
                all channels are enabled by default; space in enumeration are not allowed
All arguments are optional (see default values).
```

2. EEG and event asquisition
```
> NB2CppDemo.exe eeg 250 150 2,3,6 
Device configuration: data rate 250 Hz, input range 150 mV, enabled channels 2,3,6
Search devices ...
Device found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
Model: NB2-EEG16
Serial number: 4921
Production date: 8.12.2021
EEG p-p (uV):     0   1.1   1.1     0     0   1.1     0     0     0     0     0     0     0     0     0     0
Event 0 counter 0 type start value 0
Event 1 counter 1 type orientation value 16
EEG p-p (uV):     0   1.1   1.1     0     0   1.1     0     0     0     0     0     0     0     0     0     0
EEG p-p (uV):     0   1.1   1.1     0     0   1.1     0     0     0     0     0     0     0     0     0     0
```

3. Impedance asquisition
```
> NB2CppDemo.exe impedance
Device configuration: data rate 125 Hz, input range 150 mV, enabled channels all
Search devices ...
Device found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
Model: NB2-EEG16
Serial number: 4921
Production date: 8.12.2021
Impedances (kOhm): 5e+01 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
Impedances (kOhm): 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
Impedances (kOhm): 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
```

4. Device state registration
```
> NB2CppDemo.exe status
Device configuration: data rate 125 Hz, input range 150 mV, enabled channels all
Search devices ...
Device found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
Model: NB2-EEG16
Serial number: 4921
Production date: 8.12.2021
00:00:00 battery 98.7%, ble utilization    0%, errors 2 ACC 18 ADC 0 CELL 0 RW 0 SYS
00:01:00 battery 98.6%, ble utilization 10.2%, errors 2 ACC 18 ADC 0 CELL 0 RW 0 SYS
```


5. Start recording to SD-card
```
> NB2CppDemo.exe start-record
Device configuration: data rate 125 Hz, input range 150 mV, enabled channels all
Search devices ...
Device found, opening ...
Version: firmware 1.0.211229 dll 1.0.21084.0
Model: NB2-EEG21
Serial number: 1025
Production date: 22.4.2022
Filename (up to 8 symbols): test7865
Patient Id  (up to 16 symbols): 12-32
Patient Name (up to 64 symbols): Test Patient
Patient Birth Date: 12.12.2003
Patient Gender M/F: F
Record start successfully
```

6. Stop recording to SD-card
```
> NB2CppDemo.exe stop-record
Device configuration: data rate 125 Hz, input range 150 mV, enabled channels all
Search devices ...
Device found, opening ...
Version: firmware 1.0.211229 dll 1.0.21084.0
Model: NB2-EEG21
Serial number: 1025
Production date: 22.4.2022
Record stop successfully
```
