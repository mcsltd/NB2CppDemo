#include <nb2mcs/nb2mcs.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <future>

// command line arguments processing
// settings passed by command line arguments
struct ProgramSettings {
	enum ProgramMode { Eeg, Impedance, Status, Help, StartRecord, StopRecord };
	ProgramSettings() :
		Mode(Eeg), DataRate(Hz125), InputRange(Mv150), EnabledChannels(0x001FFFFF) {}
	ProgramMode Mode;
	Nb2Rate DataRate;
	Nb2Range InputRange;
	uint32_t EnabledChannels;
};

ProgramSettings::ProgramMode modeArg(const std::string& mode) {
	if (mode == "eeg") return ProgramSettings::ProgramMode::Eeg;
	if (mode == "impedance") return ProgramSettings::ProgramMode::Impedance;
	if (mode == "status") return ProgramSettings::ProgramMode::Status;
	if (mode == "start-record") return ProgramSettings::ProgramMode::StartRecord;
	if (mode == "stop-record") return ProgramSettings::ProgramMode::StopRecord;
	if (mode == "help" || mode == "--help" || mode == "-h")
		return ProgramSettings::ProgramMode::Help;
	throw std::exception(("Unknown program mode: " + mode).c_str());
}

Nb2Rate dataRateArg(const std::string& rate) {
	if (rate == "125")  return Hz125;
	if (rate == "250")  return Hz250;
	if (rate == "500")  return Hz500;
	if (rate == "1000") return Hz1000;
	throw std::exception(("Unknown rate: " + rate).c_str());
}

Nb2Range inputRangeArg(const std::string& range) {
	if (range == "150") return Mv150;
	if (range == "300") return Mv300;
	throw std::exception(("Unknown range: " + range).c_str());
}

uint32_t enabledChannelsArg(const std::string& channels) {
	uint32_t enabled = 0;
	std::string::size_type offset = 0;
	std::string::size_type pos = std::string(channels).find(',');
	while (offset != std::string::npos) {
		enabled |= 1 << (std::stoi(channels.substr(offset, pos - offset)) - 1);
		offset = pos < std::string::npos ? pos + 1 : pos;
		pos = std::string(channels).find(',', offset);
	}
	return enabled;
}

void showUsage() {
	std::cout << "NB2CppDemo - demo program for working with the NB2 device" << std::endl;
	std::cout << "Medical Computer Systems Ltd., 2022" << std::endl << std::endl;
	std::cout << "Usage: " << std::endl;
	std::cout << "NB2CppDemo.exe <mode> <data-rate> <input-range> <chs-enabled>" << std::endl;
	std::cout << " <mode>         working mode: eeg (default), impedance, status, start-record, stop-record or help" << std::endl;
	std::cout << " <data-rate>    eeg sampling rate in herz: 125 (default), 250, 500 or 1000" << std::endl;
	std::cout << " <input-range>  adc input range in mV: 150 (default) or 300" << std::endl;
	std::cout << " <chs-enabled>  comma-separated numbers of channels that ase used for eeg/impedance asquisition;" << std::endl;
	std::cout << "                all channels are enabled by default; space in enumeration are not allowed" << std::endl;
	std::cout << "All arguments are optional (see default values)." << std::endl << std::endl;
}

void showPorgramSettings(int argc, const char* argv[]) {
	std::cout << "Device configuration: data rate " << (argc > 2 ? argv[2] : "125") << " Hz, " <<
		"input range " << (argc > 3 ? argv[3] : "150") << " mV, " <<
		"enabled channels " << (argc > 4 ? argv[4] : "all") << std::endl;
}

ProgramSettings processCommandLineArguments(int argc, const char* argv[]) {
	try {
		ProgramSettings sets;
		if (argc > 1) sets.Mode = modeArg(argv[1]);
		if (argc > 2) sets.DataRate = dataRateArg(argv[2]);
		if (argc > 3) sets.InputRange = inputRangeArg(argv[3]);
		if (argc > 4) sets.EnabledChannels = enabledChannelsArg(argv[4]);
		return sets;
	}
	catch (const std::exception& ex) {
		std::cerr << "Bad command line arguments: " << ex.what() << std::endl;
		showUsage();
		std::exit(-1);
	}
}


// return value check for all nb2 functions
#define CHECK(x) check((#x), (x))
int check(const std::string& function, int ret) {
	if(ret < 0) {
		switch(ret) {
			case Nb2Error::ErrId: throw std::exception(("Invalid device id: " + function).c_str());
			case Nb2Error::ErrParam: throw std::exception(("Invalid function parameters :" + function).c_str());
			case Nb2Error::ErrFail: throw std::exception(("Call function fail :" + function).c_str());
			case Nb2Error::ErrObtained: throw std::exception(("Call function obtained :" + function).c_str());
			case Nb2Error::ErrSupport: throw std::exception(("Call function unsupported :" + function).c_str());
			case Nb2Error::ErrRecordExists: throw std::exception(("Call function when record exists :" + function).c_str());
			default: throw std::exception(("Unidentified error " + std::to_string(ret) + ": " + function).c_str());
		}
	}
	return ret;
}

// nb2GetData and nb2GetEvents functions return the 
// count of bytes copied to buffer or error code,
// transform into the number of EEG or event samples
int itemCount(int returnValue, size_t itemSize) {
	if(returnValue > 0) {
		if(returnValue % itemSize != 0) {
			throw std::exception("Bad data/event size in bytes");
		}
		return int (returnValue / itemSize);
	}
	return returnValue;
}

// peak-to-peak amplitude of signal in Volts over a period of time
float signalAmplitude(const t_nb2Property& prop, const int32_t* data, size_t sampleCount, size_t sampleSize, size_t channel) {
	if(sampleCount == 0) {
		return 0.f;
	}
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	for (size_t i = 0; i < sampleCount; i++) {
		const int value = data[i * sampleSize + channel];
		if (max < value) {
			max = value;
		}
		if (min > value) {
			min = value;
		}
	}
	return ((float)max - min) * prop.Resolution;
}

// number of lost samples per time period
size_t lostSamples(const int32_t* data, size_t sampleCount, size_t sampleSize) {
	static size_t expectedCounter = 0;
	size_t lostSamples = 0;
	for(size_t i = 0; i < sampleCount; i++) {
		const uint32_t currentCounter = reinterpret_cast<const uint32_t*>(data)[i * sampleSize + sampleSize - 1];
		if(currentCounter == 0) { // counter reset
			expectedCounter = 0;
		}
		lostSamples += currentCounter - expectedCounter;
		expectedCounter = currentCounter + 1;
	}
	return lostSamples;
}

void searchDevice() {
	std::cout << "Search devices ..." << std::endl;
	int count = 0;
	while(count == 0) {
		// returns the number of devices, may vary from call to call,
		// devices are not found immediately (up to a few seconds)
		count = CHECK(nb2GetCount());
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

std::string versionPrettyStringFirmware(uint64_t version) {
	return std::to_string((version >> 48) % 0x10000) + '.' +
		std::to_string((version >> 32) % 0x10000) + '.' +
		std::to_string(version % 0x100000000);
}

std::string versionPrettyStringDll(uint64_t version) {
	return std::to_string((version >> 48) % 0x10000) + '.' +
		std::to_string((version >> 32) % 0x10000) + '.' +
		std::to_string((version >> 16) % 0x10000) + '.' +
		std::to_string(version % 0x10000);
}

std::string datePrettyString(const t_nb2Date& date) {
	return std::to_string(date.Day) + '.' + std::to_string(date.Month) + '.' + std::to_string(date.Year);
}

std::string modelPrettyString(unsigned int model) {
	if(model == 1902) return "NB2-EEG21";
	if(model == 1904) return "NB2-EEG21S";
	if(model == 1900 || model == 1905) return "NB2-EEG16";
	return std::to_string(model);
}

void showInfoAboutDevice(int id) {
	// software versions
	t_nb2Version version; CHECK(nb2GetVersion(id, &version));
	std::cout << "Version: firmware " << versionPrettyStringFirmware(version.Firmware)
		<< " dll " << versionPrettyStringDll(version.Dll) << std::endl;

	// device production info
	t_nb2Information info; CHECK(nb2GetInformation(id, &info));
	std::cout << "Model: " << modelPrettyString(info.Model) << std::endl;
	std::cout << "Serial number: " << info.SerialNumber << std::endl;
	std::cout << "Production date: " << datePrettyString(info.ProductionDate) << std::endl;
}

void configureDevice(int id, const ProgramSettings& settings) {
	// EEG acquisition settings
	t_nb2DataSettings dsets;
	dsets.DataRate = settings.DataRate; // sampling rate
	dsets.EnabledChannels = settings.EnabledChannels; // enabled channels bitset, all channels enabled
	dsets.InputRange = settings.InputRange; // ADC range
	CHECK(nb2SetDataSettings(id, &dsets));

	// event settings
	t_nb2EventSettings esets;
	esets.ActivityThreshold = 1; // accelerometer sensitivity, from 0 to 2, use 1 by default
	esets.EnabledEvents = 0x003F; // detected event types bitset, all events will be detected
	CHECK(nb2SetEventSettings(id, &esets));

	// mode - EEG or impedance acquisition
	t_nb2Mode mode;
	mode.Mode = settings.Mode == Nb2Mode::Impedance ? Nb2Mode::Impedance : Nb2Mode::Data; 
	CHECK(nb2SetMode(id, &mode));
}

std::string eventTypePrettyString(Nb2EventType etype) {
	switch(etype) {
		case EvButton: return "button_press";
		case EvActivity: return "activity";
		case EvFreeFall: return "free_fall";
		case EvOrientation: return "orientation";
		case EvStart: return "start";
		case EvCharge: return "charge";
		default: return "unknown_" + std::to_string(etype);
	}
}

void processDataAndEvents(int id) {
	// nb2GetProperty gets info about physical characteristics of channels
	t_nb2Property prop; CHECK(nb2GetProperty(id, &prop));
	t_nb2Possibility poss; CHECK(nb2GetPossibility(id, &poss));
	const size_t sampleSize = poss.ChannelsCount + 2;

	std::vector<int32_t> data(2000u * sampleSize); // EEG data buffer, 2 second for maximal data rate
	t_nb2Event events[100]; // event buffer for short time period
	std::cout.precision(3);

	// EEG acquisition while user doesn't press q and enter, read data and events one time per second
	const std::future<void> future = std::async([] { while (std::cin.get() != 'q'); });
	while(future.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {

		// EEG samples processing, pass buffer size in bytes
		const size_t sampleCount = CHECK(itemCount(nb2GetData(id, data.data(), uint32_t(data.size() * sizeof(int32_t))), sampleSize));
		std::cout << "EEG p-p (uV):";
		for(size_t channel = 0; channel < poss.ChannelsCount; ++channel) {
			std::cout << std::fixed << std::setw(6) << std::setprecision(3)
				<< signalAmplitude(prop, data.data(), sampleCount, sampleSize, channel) * 1e6;
		}
		
		// lost samples processing
		if(const size_t lost = lostSamples(data.data(), sampleCount, sampleSize)) {
			std::cout << " lost " << lost << " samples";
		}
		std::cout << std::endl;

		// event processing, pass buffer size in bytes
		const size_t eventCount = CHECK(itemCount(nb2GetEvent(id, events, sizeof(events)), sizeof(*events)));
		for(size_t i = 0; i < eventCount; ++i) {
			std::cout << "Event " << events[i].Number << " counter " << events[i].Counter
				<< " type " << eventTypePrettyString(Nb2EventType(events[i].Type))
				<< " value " << std::to_string(events[i].Value) << std::endl;
		}
	}
}

void showTimeString(std::chrono::seconds::rep time) {
	std::cout << std::setfill('0') << std::setw(2) << time / 3600 << ":";
	std::cout << std::setfill('0') << std::setw(2) << time / 60 % 60 << ":";
	std::cout << std::setfill('0') << std::setw(2) << time % 60;
	std::cout << std::setfill(' ');
}

void processImpedances(int id) {
	t_nb2Possibility poss; CHECK(nb2GetPossibility(id, &poss));
	std::vector<uint32_t> impedance(poss.ChannelsCount);

	// impedance acquisition while user doesn't press q and enter, read data one time per second
	const std::future<void> future = std::async([] { while(std::cin.get() != 'q'); });
	while(future.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {

		// impedance value in Ohm for all channels, MAX_UINT - channel not connected
		CHECK(nb2GetImpedance(id, (uint8_t*) impedance.data()));
		std::cout << "Impedances (kOhm):";
		for(size_t channel = 0; channel < poss.ChannelsCount; ++channel) {
			std::cout << std::setw(6) << std::setprecision(1) << impedance[channel] * 1e-3;
		}
		std::cout << std::endl;
	}
}

void processStatus(int id) {
	t_nb2DataStatus sdata;
	t_nb2BatteryProperties sbattery;
	t_nb2UsageStats susage;

	// status registration while user doesn't press q and enter, read data one time per minute
	bool first = true;
	const auto start = std::chrono::steady_clock::now();
	std::future<void> future = std::async([] { while (std::cin.get() != 'q'); });
	while (first || future.wait_for(std::chrono::seconds(60)) != std::future_status::ready) {

		CHECK(nb2GetDataStatus(id, &sdata));
		CHECK(nb2GetBattery(id, &sbattery));
		showTimeString(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count());
		std::cout << " battery " << std::fixed << std::setw(5) << std::setprecision(1) << sbattery.Level / 10.f
			<< "%, ble utilization " << std::fixed << std::setw(5) << std::setprecision(1) << sdata.Utilization;
		if (nb2GetUsageStats(id, &susage) != Nb2Error::ErrSupport) {
			std::cout << "%, errors " << int(susage.ErrorsStats.ACC) << " ACC " << int(susage.ErrorsStats.ADC) << " ADC "
				<< int(susage.ErrorsStats.CELL) << " CELL " << int(susage.ErrorsStats.RW) << " RW "
				<< int(susage.ErrorsStats.SYS) << " SYS";
		}
		std::cout << std::endl;
		first = false;
	}
}

uint32_t inputPatientDateOfBirth(const char* invite) {
	std::cout << invite;
	std::tm tm;
	std::cin >> std::get_time(&tm, "%d.%m.%Y");
	return uint32_t(std::mktime(&tm));
}

uint8_t inputPatientGender(const char* invite) {
	std::cout << invite;
	std::string input;
	std::cin >> input;
	return uint8_t(input == "F");
}

void processStartRecord(int id) {
	t_nb2Record record;
	record.time = uint32_t(std::time(nullptr));
	std::cout << "Filename:"; std::cin.getline((char*)record.filename, sizeof(record.filename));
	std::cout << "Patient Id:"; std::cin.getline((char*)record.patient.id, sizeof(record.patient.id));
	std::cout << "Patient Name:"; std::cin.getline((char*)record.patient.name, sizeof(record.patient.name));
	record.patient.dateOfBirth = inputPatientDateOfBirth("Patient Birth Date: ");
	record.patient.gender = inputPatientGender("Patient Gender M/F: ");
	CHECK(nb2RecordStart(id, &record));
}

void processStopRecord(int id) {
	CHECK(nb2RecordStop(id));
}

int main(int argc, const char* argv[]) {
	try {
		ProgramSettings settings = processCommandLineArguments(argc, argv);
		if (settings.Mode == ProgramSettings::Help) {
			showUsage();
			return 0;
		}
		showPorgramSettings(argc, argv);

		// start device search, library resources initialization
		CHECK(nb2ApiInit());

		searchDevice();

		// open device with number 0
		std::cout << "Device 0 found, opening ..." << std::endl;
		int id = CHECK(nb2GetId(0));
		CHECK(nb2Open(id));

		showInfoAboutDevice(id);
		configureDevice(id, settings);

		// EEG or impedance acquisition start
		CHECK(nb2Start(id));

		// EEG, events or impedances processing
		if (settings.Mode == ProgramSettings::Impedance) processImpedances(id);
		else if (settings.Mode == ProgramSettings::Eeg) processDataAndEvents(id);
		else if (settings.Mode == ProgramSettings::Status) processStatus(id);
		else if (settings.Mode == ProgramSettings::StartRecord) processStartRecord(id);
		else if (settings.Mode == ProgramSettings::StopRecord) processStopRecord(id);

		// EEG or impedance acquisition stop
		CHECK(nb2Stop(id));

		// manual power off device after 2 seconds
		// (if not calling - automatic power off after 3 minutes)
		CHECK(nb2PowerOff(id, 2));

		// close device
		CHECK(nb2Close(id));

		// free library resources
		CHECK(nb2ApiDone());
	}
	catch(const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -1;
	}
	catch(...) {
		std::cerr << "Unknown error" << std::endl;
		return -1;
	}
	return 0;
}
