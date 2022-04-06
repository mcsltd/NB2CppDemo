## NB2CppDemo
Demo program for working with the NB2-EEG16 device.

## Description
This program is used to:
 - search and open NB2-EEG16 device
 - show technical information about device (serial number, prodaction date, software version and etc)
 - configure device (sets data rate, adc input range, enabled channels)
 - eeg asquition and peak-to-peak signal amplitude calculation
 - channels impedance registration
 - battery state registration (battery change and etc)
 
 
## Usage
`NB2CppDemo.exe <mode> <data-rate> <input-range> <chs-enabled>`
` <mode>        ` program working mode: eeg (default), impedance, battery or help
` <data-rate>   ` eeg sampling rate in herz: 125 (default), 250, 500 or 1000
` <input-range> ` adc input range in mV: 150 (default) or 300
` <chs-enabled> ` comma-separated numbers of channels that ase used for eeg/impedance asquisition; all channels are enabled by default; space in enumeration are not allowed
All arguments are optional (see default values).

## Examples

1. Show help
```
> NB2CppDemo.exe help
NB2CppDemo - demo program for working with the NB2-EEG16 device
Medical Computer Systems Ltd., 2022

Usage:
NB2CppDemo.exe <mode> <data-rate> <input-range> <chs-enabled>
 <mode>         program working mode: eeg (default), impedance, battery or help
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
Device 0 found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
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
Device 0 found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
Serial number: 4921
Production date: 8.12.2021
Impedances (kOhm): 5e+01 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
Impedances (kOhm): 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
Impedances (kOhm): 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06 4e+06
```

4. Battery state registration
```
> NB2CppDemo.exe status
Device configuration: data rate 125 Hz, input range 150 mV, enabled channels all
Search devices ...
Device 0 found, opening ...
Version: firmware 1.0.211217 dll 1.0.19379.0
Serial number: 4921
Production date: 8.12.2021
00:00:00 battery 98.7%, ble utilization    0%, errors 2 ACC 18 ADC 0 CELL 0 RW 0 SYS
00:01:00 battery 98.6%, ble utilization 10.2%, errors 2 ACC 18 ADC 0 CELL 0 RW 0 SYS
```
