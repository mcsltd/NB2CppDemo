#pragma once
#include <cstdint>

#ifdef _MSC_VER
    #define NB2MCS_EXPORT extern "C" __declspec(dllimport)
#else
    #define NB2MCS_EXPORT
#endif

#pragma pack(push, 1)

struct t_nb2Version {
    uint64_t Dll;
    uint64_t Firmware;
};

struct t_nb2Date {
    uint16_t Year;
    uint8_t Month;
    uint8_t Day;
};

struct t_nb2Information {
    uint32_t Model;             /*!< Device model ID */
    uint32_t SerialNumber;      /*!< Device serial number */
	t_nb2Date ProductionDate;  /*!< Device production date and time */
};

struct t_nb2Property {
    float Rate;              /*!< EEG sampling rate, Hz */
    float Resolution;        /*!< EEG amplitude scale coefficients, V/bit */
    float Range;             /*!< EEG input range, V */
};

struct t_nb2Mode {
	uint8_t Mode;
};

struct t_nb2DataSettings {
    uint8_t DataRate;
    uint8_t InputRange;
	uint16_t EnabledChannels;
};

struct t_nb2Adjustment {
    uint8_t MasterClock;
    uint8_t Boost;
    uint8_t Dithering;
}; 

enum Nb2EventType {
    EvButton = 0,   // Button press event
    EvActivity,     // Activity detected on X, Y, or Z direction
    EvFreeFall,     // Free fall detected
    EvOrientation,  // 6D orientation switched to: XH, XL, YH, YL, ZH, ZL (change in position portrait/landscape/face-up/face-down)
    EvStart,		// Start event
    EvCharge        // Charge event
};

enum Nb2Mode {
	Data, Impedance
};

enum Nb2Range {
	Mv150, Mv300
};

enum Nb2Rate {
	Hz125, Hz250, Hz500, Hz1000
};

struct t_nb2EventSettings {
	uint16_t EnabledEvents;
	uint16_t ActivityThreshold;
};

struct t_nb2DataStatus {
	float Rate; // data rate, SPS
	float Speed; // data speed, KB/s
	float Ratio; // compression ratio, %
	float Utilization; // BLE utilization, %
};

struct t_nb2Data {
    int Channel[16];
    uint32_t Status;
    uint32_t Counter;
};

struct t_nb2Acceleration {
    float X;
    float Y;
    float Z;
};

struct t_nb2Event {
	uint8_t Type;
	uint8_t Value;
	t_nb2Acceleration Acceleration;
	uint32_t Number; // event sequence number
	uint32_t Counter; // data sample counter
};

struct t_nb2Impedance {
	uint32_t Channel[16];
};

struct t_nb2BatteryProperties {
    uint16_t Capacity;
    uint16_t Level;
    uint16_t Voltage;
    int16_t Current;
    int16_t Temperature;
};

struct t_nb2CalibrationRange {
	int32_t Offset[16];
	int32_t Value[16];
};

struct t_nb2CalibrationImp {
	int16_t Offset[16];
	uint16_t Value[16];
};

struct t_nb2Calibration {
	t_nb2Date Date;
	float Voltage;
	float Impedance;
	t_nb2CalibrationRange Range150mV;
	t_nb2CalibrationRange Range300mV;
	t_nb2CalibrationImp CalImpedance;
};

struct t_nb2ErrorsStats {
    uint8_t ACC;
    uint8_t ADC;
    uint8_t CELL;
    uint8_t RW;
    uint8_t SYS;
};

struct t_nb2UsageStats {
    uint32_t PowerOnCount;
    uint32_t PowerOnSeconds;
    uint32_t DataSendSeconds;
    uint32_t DataWriteSeconds;
    t_nb2ErrorsStats ErrorsStats;
};

#pragma pack(pop)

enum Nb2Error {
    ErrOk = 0,
    ErrId = -1, // invalid id
    ErrFail = -2, // operation failed
    ErrParam = -3 // incorrect argument
};

NB2MCS_EXPORT int nb2EventInit(int* Callback);
NB2MCS_EXPORT int nb2GetVersion(int Id, t_nb2Version* Version);
NB2MCS_EXPORT int nb2SetOut(int Id, uint8_t State);
NB2MCS_EXPORT uint32_t nb2GetCount();
NB2MCS_EXPORT int nb2GetId(uint32_t Index);
NB2MCS_EXPORT int nb2GetSerialNumber(int Id);
NB2MCS_EXPORT int nb2Open(int Id);
NB2MCS_EXPORT int nb2Close(int Id);
NB2MCS_EXPORT int nb2GetDataStatus(int Id, t_nb2DataStatus* DataStatus);
NB2MCS_EXPORT int nb2GetBattery(int Id, t_nb2BatteryProperties* Battery);
NB2MCS_EXPORT int nb2GetImpedance(int Id, t_nb2Impedance* Impedance);
NB2MCS_EXPORT int nb2GetInformation(int Id, t_nb2Information* Information);
NB2MCS_EXPORT int nb2GetCalibration(int Id, t_nb2Calibration* Calibration);
NB2MCS_EXPORT int nb2GetCalibrated(int Id, bool* Calibrated);
NB2MCS_EXPORT int nb2GetUsageStats(int Id, t_nb2UsageStats* UsageStats);
NB2MCS_EXPORT int nb2GetProperty(int Id, t_nb2Property* Property);
NB2MCS_EXPORT int nb2SetAdjustment(int Id, t_nb2Adjustment* Adjust);
NB2MCS_EXPORT int nb2SetMode(int Id, t_nb2Mode* DataMode);
NB2MCS_EXPORT int nb2SetDataSettings(int Id, t_nb2DataSettings* DataSettings);
NB2MCS_EXPORT int nb2SetEventSettings(int Id, t_nb2EventSettings* EventSettings);
NB2MCS_EXPORT int nb2PowerOff(int Id, int WaitTime);
NB2MCS_EXPORT int nb2Start(int Id);
NB2MCS_EXPORT int nb2Stop(int Id);
NB2MCS_EXPORT int nb2GetData(int Id, t_nb2Data* Data, uint32_t Size);
NB2MCS_EXPORT int nb2GetEvent(int Id, t_nb2Event* Event, uint32_t Size);
NB2MCS_EXPORT int nb2ApiInit();
NB2MCS_EXPORT int nb2ApiDone();
