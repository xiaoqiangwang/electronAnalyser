//================================================================================================
/* file:		electronAnalyser.cpp
 *
 * description:	Implementation of EPICS areaDetector driver for data acquisition using
 * 				VG Scienta Electron Analyser EW4000. It uses SESWrapper software to communicate
 * 				with instrument firmware which in turn controls the electron analyser.
 *
 * project:		ElectronAnalyser
 *
 * $Author:		Fajin Yuan $
 * 				Diamond Light Source Ltd
 *
 * $Revision:	1.0.0 $
 *
 * $Log:		electronAnalyser.cpp $
 *
 * Revision 1.0.0 2010/12/06 12:45:25 fajinyuan
 * initial creation of the project
 */

// Standard includes
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdafx.h"
#include <atlstr.h>
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

// EPICS includes
#include <epicsString.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsTimer.h>

// areaDetector includes
#include <ADDriver.h>

// include device-specific API
#include "wseswrappermain.h"
#include "werror.h"

#define MAX_MESSAGE_SIZE 256
#define MAX_FILENAME_LEN 256

#include <io.h>   // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <iostream>
#include <string>
using namespace std;

using std::string;

//const CString SES_ROOT = getenv("SES_ROOT");
#define AD_STATUS_EXTENSION_START_POINT ADStatusWaiting+1

/** Enumeration for Electron analyser Run Mode */
typedef enum
{
	Normal,
	AddDimension
} runMode_t;

typedef std::vector<std::string> NameVector;
typedef std::vector<double> DoubleVector;

static const char *driverName = "electronAnalyser";

/**
 * Driver class for VG Scienta Electron Analyzer EW4000 System. It uses SESWrapper to communicate to the instrument library, which
 * in turn depends on the installation of SES (i.e. working directory) and the name of the instrument configuration file at (@p workingDir\data\).
 *
 * Please note only one program can be run at the same time, either ses.exe or this IOC.
 */
class ElectronAnalyser: public ADDriver
{
public:
    ElectronAnalyser(const char *workingDir, const char *instrumentFile,
    		int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory, int priority, int stackSize);
    virtual ~ElectronAnalyser();
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual);
    virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize);
    void report(FILE *fp, int details);
    epicsEventId startEventId;
    epicsEventId stopEventId;
    void electronAnalyserTask();
private:
    WSESWrapperMain *ses;
    SESWrapperNS::WAnalyzerRegion analyzer;
    SESWrapperNS::WDetectorRegion detector;
    SESWrapperNS::WDetectorInfo detectorInfo;
    asynStatus acquireData(void *pData);
    asynStatus getDetectorTemperature(float *temperature);
    virtual void init_device(const char *workingDir, const char *instrumentFile);
    void delete_device();
    virtual void updateStatus();

    // Anaylzer specific parameters
    virtual asynStatus getKineticEnergy(double *kineticEnergy);
    virtual asynStatus setKineticEnergy(const double kineticEnergy);
    virtual asynStatus getElementVoltage(const char *elementName, double *voltage);
    virtual asynStatus setElementVoltage(const char *elementName, const double voltage);
    virtual asynStatus zeroSupplies();

    virtual asynStatus setAcquisitionMode(const bool b);
    virtual asynStatus getAcquisitionMode(bool *b);
    virtual asynStatus setEnergyMode(const bool b);
    virtual asynStatus getEnergyMode(bool *b);

    virtual asynStatus start();
    virtual asynStatus stop();

    //HW specific methods
    virtual asynStatus resetInstrument();
    virtual asynStatus testCommunication();

    virtual asynStatus getLensModeList(NameVector * pLensModeList);
    virtual asynStatus getPassEnergyList(DoubleVector * pPassEnergyList);
    virtual asynStatus getElementSetLlist(NameVector * pElementSetList);

    bool isError(int & err, const char * functionName);

    // access methods to properties - defined in WSESWrapperBase class
    virtual asynStatus getLibDescription(char *value, int &size);
    virtual asynStatus getLibVersion(char *value, int &size);
    virtual asynStatus getLibError(int index, char *value, int &size);
    virtual asynStatus getLibWorkingDir(char *value, int &size);
    virtual asynStatus setLibWorkingDir(const char *value);
    virtual asynStatus getInstrumentStatus(int *value);
    virtual asynStatus getAlwaysDelayRegion(bool *value);
    virtual asynStatus setAlwaysDelayRegion(const bool *value);
    virtual asynStatus getAllowIOWithDetector(bool *value);
    virtual asynStatus setAllowIOWithDetector(const bool *value);
    virtual asynStatus getInstrumentModel(char *value, int &size);
    virtual asynStatus getInstrumentSerialNo(char *value, int &size);
    virtual asynStatus getDetectorInfo(SESWrapperNS::DetectorInfo *value);
    virtual asynStatus getDetectorRegion(SESWrapperNS::DetectorRegion *value);
    virtual asynStatus setDetectorRegion(const SESWrapperNS::DetectorRegion *value);
    virtual asynStatus getElementSetCount(int & value);
    virtual asynStatus getElementSet(int index, char * elementSet, int & size);
    virtual asynStatus setElementSet(const char * elementSet);
    virtual asynStatus getLensModeCount(int & value);
    virtual asynStatus getLensMode(int index, char * lensMode, int & size);
    virtual asynStatus setLensMode(const char * lensMode);
    virtual asynStatus getPassEnergyCount(int & value);
    virtual asynStatus getPassEnergy(int index, double &passEnergy );
    virtual asynStatus setPassEnergy(const double * passEnergy);
    virtual asynStatus getAnalyzerRegion(SESWrapperNS::WAnalyzerRegion *value);
    virtual asynStatus setAnalyzerRegion(const SESWrapperNS::WAnalyzerRegion *value);
    virtual asynStatus getUseExternalIO(bool *value);
    virtual asynStatus setUseExternalIO(const bool *value);
    virtual asynStatus getUseDetector(bool *value);
    virtual asynStatus setUseDetector(const bool *value);
    virtual asynStatus getRegionName(char *value, int &size);
    virtual asynStatus setRegionName(const char *value);
    virtual asynStatus getTempFileName(char *value, int &size);
    virtual asynStatus setTempFileName(const char *value);
    virtual asynStatus getResetDataBetweenIterations(bool * value);
    virtual asynStatus setResetDataBetweenIterations(const bool * value);

    // access methods to Data Parameters
    virtual asynStatus getAcqChannels(int & channels);
    virtual asynStatus getAcqSlices(int & slices);
    virtual asynStatus getAcqIterations(int & iterations);
    virtual asynStatus getAcqIntensityUnit(char * intensityUnit, int & size);
    virtual asynStatus getAcqChannelUnit(char * channelUnit, int & size);
    virtual asynStatus getAcqSliceUnit(char * sliceUnit, int & size);
    virtual asynStatus getAcqChannelScale(double * pSpectrum, int & size);
    virtual asynStatus getAcqSliceScale(double * pSpectrum, int & size);
    virtual asynStatus getAcqSpectrum(double * pSumData, int & size);
    virtual asynStatus getAcqImage(double * pData, int & size);
    virtual asynStatus getAcqSlice(int index, double * pSliceData, int size);
    virtual asynStatus getAcqRawImage(int * pImage, int &size);
    virtual asynStatus getAcqCurrentStep(int &currentStep);
    virtual asynStatus getAcqElapsedTime(double &elapsedTime);
    virtual asynStatus getAcqIOPorts(int &ports);
    virtual asynStatus getAcqIOSize(int &dataSize);
    virtual asynStatus getAcqIOIterations(int &iterations);
    virtual asynStatus getAcqIOUnit(char * unit, int & size);
    virtual asynStatus getAcqIOScale(double * scale, int & size);
    virtual asynStatus getAcqIOSpectrum(int index, double * pSpectrum, int & size);
    virtual asynStatus getAcqIOData(double * pData, int & size);
    virtual asynStatus getAcqIOPortName(int index, char * pName, int & size);

    WError *werror;
    string sesWorkingDirectory;
    string instrumentFilePath;

	float m_dTemperature;
	bool m_bAllowIOWithDetector;
	bool m_bAlwaysDelayRegion;
	runMode_t m_RunMode;
	NameVector m_Elementsets;
	NameVector m_LensModes;
	DoubleVector m_PassEnergies;
	char * m_sCurrentElementSet;
	char * m_sCurrentLensMode;
	double m_dCurrentPassEnergy;
	bool m_bUseExternalIO;
	bool m_bUseDetector;
	bool m_bResetDataBetweenIterations;
};
/* end of Electron Analyser class description */

/** A bit of C glue to make the config function available in the startup script (ioc shell) */
extern "C" int electronAnalyserConfig(const char *workingDir, const char *instrumentFile,
		int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory, int priority,	int stackSize)
{
	new ElectronAnalyser(workingDir, instrumentFile, maxSizeX, maxSizeY, maxBuffers, maxMemory,
			priority, stackSize);
	return asynSuccess;
}

static void electronAnalyserTaskC(void *drvPvt)
{
	ElectronAnalyser *pPvt = (ElectronAnalyser *) drvPvt;
	pPvt->electronAnalyserTask();
}

/** Define Electron Analyser driver specific parameters */
typedef enum
{
	//Properties
	LibDescription 			/**< (asynOctet,    	r/o) the library description*/
		= ADLastStdParam,
	LibVersion,				/**< (asynOctet,    	r/o) the library version*/
	LibWorkingDir,			/**< (asynOctet,    	r/w) the woring directory of the current application*/
	InstrumentStatus,		/**< (asynInt32,    	r/o) the status of instrument specified at SesNS::InstrumentStatus.*/
	AlwaysDelayRegion, 		/**< (asynInt32,    	r/w) apply region delay even when HV supplies are not changed (0=No, 1=YES).*/
	AllowIOWithDetector,	/**< (asynInt32,    	r/w) allow simultanouse acquisition of both external IO and detector (0=No, 1=YES).*/
	InstrumentSerialNo,		/**< (asynOctet,    	r/o) the instrument serial number*/
	//Detector Info
	TimerControlled,		/**< (asynInt32,    	r/o) Specifies whether the detector is controlled by a timer (@c true) or frame rate (@c false)(0=No, 1=YES).*/
	XChannels,				/**< (asynInt32,    	r/o) Specifies the number of X channels currently shown on the detector.*/
	YChannels,				/**< (asynInt32,    	r/o) Specifies the number of Y channels (slices) currently shown on the detector.*/
	MaxSlices,				/**< (asynInt32,    	r/o) Specifies the maximum number of Y channels (slices).*/
	MaxChannels,			/**< (asynInt32,    	r/o) Specifies the maximum number of X channels.*/
	FrameRate,				/**< (asynInt32,    	r/o) Specifies the frame rate (frames/s).*/
	ADCPresent,				/**< (asynInt32,    	r/o) Specifies whether the detector contains an ADC (0=No, 1=YES).*/
	DiscPresent,			/**< (asynInt32,    	r/o) Specifies whether the detector contains a discriminator (0=No, 1=YES).*/
	// Detector Region
	DetectorFirstXChannel,	/**< (asynInt32,    	r/w) Specifies the first X channel to be used on the detector. */
	DetectorLastXChannel,	/**< (asynInt32,    	r/w) Specifies the last X channel to be used on the detector. */
	DetectorFirstYChannel,	/**< (asynInt32,    	r/w) Specifies the first Y channel to be used on the detector. */
	DetectorLastYChannel,	/**< (asynInt32,    	r/w) Specifies the last Y channel to be used on the detector. */
	DetectorSlices,			/**< (asynInt32,    	r/w) Specifies the current number of Y channels (slices). */
	DetectorMode,			/**< (asynInt32,    	r/w) Specifies whether the detector is running in ADC mode (1=YES), or Pulse Counting mode (0=No).*/
	DetectorDiscriminatorLevel,/**< (asynInt32,    	r/w) Specifies the detector discriminator level.*/
	DetectorADCMask,		/**< (asynInt32,    	r/w) Specifies the detector ADC mask. */
	// Analyzer Region
	AnalyzerAcquisitionMode,/**< (asynInt32,    	r/w) Determines if the region will be measured in fixed (1=YES) or swept (0=NO) mode. */
	AnalyzerHighEnergy,		/**< (asynFloat64,  	r/w) Specifies the high-end kinetic energy (eV) for swept mode acquisition. */
	AnalyzerLowEnergy,		/**< (asynFloat64,  	r/w) Specifies the low-end kinetic energy (eV) for swept mode acquisition. */
	AnalyzerCenterEnergy,	/**< (asynFloat64,  	r/w) Specifies the center energy (eV) for fixed mode acquisition (the low and high end energies is calculated from this value and the current pass energy). */
	AnalyzerEnergyStep,		/**< (asynFloat64,  	r/w) Specifies the energy step size (eV) for swept mode acquisition. */
	AnalyzerDwellTime,		/**< (asynInt32,  	r/w) Specifies the dwell time (ms) for fixed or swept mode acquisition. */
	// Energy Scale
	EnergyMode,				/**< (asynInt32,    	r/w) Determines if the energy scale is in Kinetic (1=YES) or Binding (0=NO) mode. */
	RunMode,				/**< (asynInt32, 		r/w) selects how software should perform the acquisition and save data.*/

	ElementSetCount,		/**< (asynInt32,    	r/o) the number of installed element sets.*/
	ElementSet,				/**< (asynInt32,    	r/w) select an element set from the list of the installed element sets*/
	LensModeCount,			/**< (asynInt32,		r/o) the number of available lens modes.*/
	LensMode,				/**< (asynInt32,    	r/w) select an lens mode from the list of available lens modes*/
	PassEnergyCount,		/**< (asynInt32,		r/o) the number of available pass energies for the current lens mode.*/
	PassEnergy,				/**< (asynInt32, 	 	r/w) select an pas energy from the list of available pass energies for the current lens mode.*/
	UseExternalIO,			/**< (asynInt32,    	r/w) enable or disable the external IO interface (0=No, 1=YES).*/
	UseDetector, 			/**< (asynInt32,    	r/w) enable or disable the detector (0=No, 1=YES).*/
	RegionName, 			/**< (asynOctet,    	r/w) the name of the current region (max 32 characters)*/
	TempFileName,			/**< (asynOctet,    	r/w) the name of the temporary file created when performing acquisition*/
	ResetDataBetweenIterations, /**< (asynInt32,    r/w) allow reset of spectrum and external IO data between each iteration. (0=No, 1=YES).*/
	// Data Parameters
	AcqChannels,			/**< (asynInt32,		r/o) the number of channels in acquired data*/
	AcqSlices, 				/**< (asynInt32,		r/o) the number of slices in acquired data*/
	AcqIterations, 			/**< (asynInt32,		r/o) the number of iterations since last call to initAcqisition()*/
	AcqIntensityUnit, 		/**< (asynOctet,		r/o) the unit of intensity scale (e.g. "counts/s")*/
	AcqChannelUnit, 		/**< (asynOctet,		r/o) the unit of channel scale (e.g. "eV")*/
	AcqSliceUnit, 			/**< (asynOctet,		r/o) the unit of slice scale (e.g. "mm")*/
	AcqSpectrum, 			/**< (asynFloat64Array,	r/o) the integrated spectrum*/
	AcqImage, 				/**< (asynFloat64Array,	r/o) the 2D matrix of acquired data*/
	AcqSlice, 				/**< (asynFloat64Array,	r/o) access one slice from acquired data. The AcqSliceNumber defines which slice to access.*/
	AcqSliceNumber, 		/**< (asynInt32,		r/w) the index parameters that specifies which slice to access by AcqSlice*/
	AcqChannelScale, 		/**< (asynFloat64Array,	r/o) the channel scale*/
	AcqSliceScale, 			/**< (asynFloat64Array,	r/o) the slice scale*/
	AcqRawImage,			/**< (asynInt32Array,	r/o) the last image taken by the detector*/
	AcqCurrentStep,			/**< (asynInt32,    	r/o) the current step in a swept mode acquisition.*/
	AcqElapsedTime, 		/**< (asynFloat64,  	r/o) the elapsed time (ms) since last call to StartAcquisition()*/
	AcqIOPorts,				/**< (asynInt32, 		r/o) the number of ports available from external IO interface measurements*/
	AcqIOSize,				/**< (asynInt32, 		r/o) the size of each vector from external IO data*/
	AcqIOIterations,		/**< (asynInt32, 		r/o) the number of times the external IO data has been acquired*/
	AcqIOUnit,				/**< (asynOctet, 		r/o) the unit of the external IO data vectors*/
	AcqIOScale,				/**< (asynFloat64Array, r/o) the scale of the external IO data*/
	AcqIOSpectrum,			/**< (asynFloat64Array, r/o) data from one of the available ports in the external IO interface. AcqIOPortIndex parameter specifies which port to access. The size of the data is AcqIOSize*/
	AcqIOPortIndex,			/**< (asynInt32, 		r/w) specify the port index in the external IO interface to access data*/
	AcqIOData,				/**< (asynFloat64Array, r/o) a matrix of all data from the available ports in the external IO interface. The size of the data is AcqIOPorts * AcqIOSize.*/
	AcqIOPortName,			/**< (asynOctet, 		r/o) the name of the IO port indicated by AcqIOPortIndex parameter*/

	ADLastDriverParam
} electronAnalyserParam_t;


/** Define a table that maps enum-string.
 * The string is used in the database to reference a certain parameter */
static asynParamString_t electronAnalyserParamString[] =
{
		{LibDescription,	"LIB_DESCRIPTION"},
		{LibVersion,		"LIB_VERSION"},
		{LibWorkingDir,		"LIB_WORKING_DIR"},
		{InstrumentStatus,	"INSTRUMENT_STATUS"},
		{AlwaysDelayRegion, "ALWAYS_DELAY_REGION"},
		{AllowIOWithDetector,"ALLOW_IO_WITH_DETECTOR"},
		{InstrumentSerialNo,"INSTRUMENT_SERIAL_NUMBER"},
		//Detector Info
		{TimerControlled,	"TIMER_CONTROLLED"},
		{XChannels,			"X_CHANNELS"},
		{YChannels,			"Y_CHANNELS"},
		{MaxSlices,			"MAX_SLICES"},
		{MaxChannels,		"MAX_CHANNELS"},
		{FrameRate,			"FRAME_RATE"},
		{ADCPresent,		"ADC_PRESENT"},
		{DiscPresent,		"DISC_PRESENT"},
		// Detector Region
		{DetectorFirstXChannel,"FIRST_X_CHANNEL"},
		{DetectorLastXChannel, "LAST_X_CHANNEL"},
		{DetectorFirstYChannel,"FIRST_Y_CHANNELS"},
		{DetectorLastYChannel, "LAST_Y_CHANNELS"},
		{DetectorSlices,	   "DETECTOR_SLICES"},
		{DetectorMode,			"DETECTOR_MODE"},
		// Analyzer Region
		{AnalyzerAcquisitionMode,"ACQISITION_MODE"},
		{AnalyzerHighEnergy,	"HIGH_ENERGY"},
		{AnalyzerLowEnergy,		"LOW_ENERGY"},
		{AnalyzerCenterEnergy,	"CENTER_ENERGY"},
		{AnalyzerEnergyStep,	"ENERGY_STEP"},
		{AnalyzerDwellTime,		"DWELL_TIME"},
		// Energy Scale
		{EnergyMode,			"ENERGY_MODE"},
		{RunMode,				"EUN_MODE"},
		{ElementSetCount,		"ELEMENT_SET_COUNT"},
		{ElementSet,			"ELEMENT_SETS"},
		{LensModeCount,			"LENS_MODE_COUNT"},
		{LensMode,				"LENS_MODES"},
		{PassEnergyCount,		"PASS_ENERGY_COUNT"},
		{PassEnergy,			"PASS_ENERGIES"},
		{UseExternalIO,			"USE_EXTERNAL_IO"},
		{UseDetector, 			"USE_DETECTOR"},
		{RegionName, 			"REGION_NAME"},
		{TempFileName,			"TEMP_FILE_NAME"},
		{ResetDataBetweenIterations, "RESET_DATA_BETWEEN_ITERATIONS"},
		// Data Parameters
		{AcqChannels,			"ACQ_CHANNELS"},
		{AcqSlices, 			"ACQ_SLICES"},
		{AcqIterations, 		"ACQ_ITERATIONS"},
		{AcqIntensityUnit, 		"ACQ_INTENSITY_UNIT"},
		{AcqChannelUnit, 		"ACQ_CHANNEL_UNIT"},
		{AcqSliceUnit, 			"ACQ_SLICE_UNIT"},
		{AcqSpectrum, 			"ACQ_SPECTRUM"},
		{AcqImage, 				"ACQ_IMAGE"},
		{AcqSlice, 				"ACQ_SLICE"},
		{AcqSliceNumber, 		"ACQ_SLICE_INDEX"},
		{AcqChannelScale, 		"ACQ_CHANNEL_SCALE"},
		{AcqSliceScale, 		"ACQ_SLICE_SCALE"},
		{AcqRawImage,			"ACQ_RAW_IMAGE"},
		{AcqCurrentStep,		"ACQ_CURRENT_STEP"},
		{AcqElapsedTime, 		"ACQ_ELAPSED_TIME"},
		{AcqIOPorts,			"ACQ_IO_PORTS"},
		{AcqIOSize,				"ACQ_IO_SIZE"},
		{AcqIOIterations,		"ACQ_IO_ITERATIONS"},
		{AcqIOUnit,				"ACQ_IO_UNIT"},
		{AcqIOScale,			"ACQ_IO_SCALE"},
		{AcqIOSpectrum,			"ACQ_IO_SPECTRUM"},
		{AcqIOPortIndex,		"ACQ_IO_PORT_INDEX"},
		{AcqIOData,				"ACQ_IO_DATA"},
		{AcqIOPortName,			"ACQ_IO_PORT_NAME"}

};

// A little trick to get the number of parameters...
#define NUM_ELECTRONANALYSER_PARAMS (sizeof(electronAnalyserParamString)/sizeof(electronAnalyserParamString[0]))

ElectronAnalyser::~ElectronAnalyser()
{
}
ElectronAnalyser::ElectronAnalyser(const char *workingDir, const char *instrumentFile,
		int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory, int priority,
		int stackSize) :
	ADDriver(portName, 1, ADLastDriverParam, maxBuffers, maxMemory, 0, 0, /* No interfaces beyond those set in ADDriver.cpp */
	ASYN_CANBLOCK, 1, //asynflags (CANBLOCK means separate thread for this driver)
			priority, stackSize) // thread priority and stack size (0=default)
{
	int status = asynSuccess;
	const char *functionName = "ElectronAnalyser";
	int eaStatus = WError::ERR_OK;
	char message[MAX_MESSAGE_SIZE];
	int size;
	char * value;
	werror = WError::instance();
	// Create the epicsEvents for signaling to the Electron Analyser task when acquisition starts and stops
	this->startEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->startEventId)
	{
		printf("%s:%s epicsEventCreate failure for start event\n", driverName,
				functionName);
		return;
	}
	this->stopEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->stopEventId)
	{
		printf("%s:%s epicsEventCreate failure for stop event\n", driverName,
				functionName);
		return;
	}
	printf("%s:%s: Initialising SES library.\n", driverName, functionName);
	this->init_device(workingDir, instrumentFile);

	// initialise state variables from SES library
	getDetectorTemperature(&m_dTemperature);
	getAllowIOWithDetector(&m_bAllowIOWithDetector);
	getAlwaysDelayRegion(&m_bAlwaysDelayRegion);
	getDetectorInfo(&detectorInfo);
	getDetectorRegion(&detector);
	getAnalyzerRegion(&analyzer);
	m_RunMode = Normal;
	getElementSetLlist(&m_Elementsets);
	getLensModeList(&m_LensModes);
	getPassEnergyList(&m_PassEnergies);
	getElementSet(-1, m_sCurrentElementSet, size);
	getLensMode(-1, m_sCurrentLensMode, size);
	getPassEnergy(-1,m_dCurrentPassEnergy);
	getUseExternalIO(&m_bUseExternalIO);
	getUseDetector(&m_bUseDetector);
	getResetDataBetweenIterations(&m_bResetDataBetweenIterations);

	/* Set some default values for parameters */
	// the setup panel parameters
	status |= setStringParam(ADManufacturer, "VG Scienta");
	getInstrumentModel(0, size);
	value = new char[size];
	getInstrumentModel(value, size);
	status |= setStringParam(ADModel, value);
	getLibDescription(0, size);
	value=new char[size];
	getLibDescription(value, size);
	status |= setStringParam(LibDescription, value);
	getLibVersion(0, size);
	value = new char[size];
	getLibVersion(value, size);
	status |= setStringParam(LibVersion, value);
	getLibWorkingDir(0, size);
	value=new char[size];
	getLibWorkingDir(value,size);
	status |= setStringParam(LibWorkingDir, value);
	getInstrumentSerialNo(0, size);
	value =  new char[size];
	getInstrumentSerialNo(value, size);
	status |= setStringParam(InstrumentSerialNo, value);

	// the Readout panel parameters
	status |= setIntegerParam(ADMaxSizeX, detectorInfo.xChannels_);
	status |= setIntegerParam(ADMaxSizeY, detectorInfo.yChannels_);
	status |= setIntegerParam(ADMinX, detector.firstXChannel_);
	status |= setIntegerParam(ADMinY, detector.lastXChannel_);
	status |= setIntegerParam(ADSizeX, detector.firstYChannel_ - detector.firstXChannel_);
	status |= setIntegerParam(ADSizeY, detector.lastYChannel_ - detector.firstYChannel_);
		//status |= setIntegerParam(NDArraySize, 0);
	//status |= setIntegerParam(NDDataType,  NDInt32);

	// the Collect panel
	status |= setDoubleParam(ADAcquireTime, analyzer.dwellTime_/1000.0);
	status |= setDoubleParam(ADAcquirePeriod, 0.0);
	status |= setIntegerParam(ADNumImages, 1);
	status |= setIntegerParam(ADNumExposures, 1); // number of frames per image
	status |= setIntegerParam(ADImageMode, ADImageSingle);
	status |= setIntegerParam(ADTriggerMode, ADTriggerInternal);
	updateStatus();
	status |= setStringParam(ADStatusMessage, message);

	status |= setIntegerParam(NDAutoIncrement, 1);

	status |= setDoubleParam(ADTemperature, m_dTemperature);

	// Electron analyzer specific parameters
	status |= setIntegerParam(AlwaysDelayRegion, m_bAlwaysDelayRegion?1:0);
	status |= setIntegerParam(AllowIOWithDetector, m_bAllowIOWithDetector?1:0);
	status |= setIntegerParam(UseDetector, m_bUseDetector?1:0);
	status |= setIntegerParam(UseDetector, m_bUseExternalIO?1:0);

	if (status)
	{
		printf("%s:%s: unable to set detector parameters\n", driverName,
				functionName);
		return;
	}

	printf("  Starting up polling task...\n");
	/* Create the thread that updates the images */
	status = (epicsThreadCreate("ElectronAnalyserTask",
			epicsThreadPriorityMedium, epicsThreadGetStackSize(
					epicsThreadStackMedium),
			(EPICSTHREADFUNC) electronAnalyserTaskC, this) == NULL);
	if (status)
	{
		printf("%s:%s epicsThreadCreate failure for image task\n", driverName,
				functionName);
		return;
	}
}
/** Task to grab image off the Frame Grabber and send them up to areaDetector.
 *
 *  This function runs the polling thread.
 *  It is started in the class constructor and must not return until the IOC stops.
 *
 */
void ElectronAnalyser::electronAnalyserTask()
{
	int status = asynSuccess;
	int acquire;
	int nbytes;
	int numImages, numImagesCounter, imageCounter, imageMode;
	int arrayCallbacks;
	double acquireTime, acquirePeriod, delay;
	epicsTimeStamp startTime, endTime;
	double elapsedTime;
	NDArray *pImage;
	int dims[2];
	NDDataType_t dataType;
	float temperature;
	const char *functionName = "electronAnalyserTask";

	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: thread started!\n", driverName, functionName);

	this->lock();
	while (1)
	{
		/* Is acquisition active? */
		getIntegerParam(ADAcquire, &acquire);

		/* If we are not acquiring then wait for a semaphore that is given when acquisition is started */
		if (!acquire)
		{
			setStringParam(ADStatusMessage, "Waiting for acquire command");
			setIntegerParam(ADStatus, ADStatusIdle);
			getDetectorTemperature(&temperature);
			setDoubleParam(ADTemperature, temperature);
			callParamCallbacks();
			/* Release the lock while we wait for an event that says acquire has started, then lock again */
			this->unlock();
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: waiting for acquire to start\n", driverName, functionName);
			status = epicsEventWait(this->startEventId);
			this->lock();
			getIntegerParam(ADAcquire, &acquire);
		}
		/* We are acquiring. */
		epicsTimeGetCurrent(&startTime);
		getDetectorTemperature(&temperature);
		setDoubleParam(ADTemperature, temperature);

		/* Get the exposure parameters */
		getDoubleParam(ADAcquireTime, &acquireTime);
		getDoubleParam(ADAcquirePeriod, &acquirePeriod);

		/* Get the acquisition parameters */
		//getIntegerParam(ADTriggerMode, &triggerMode);
		getIntegerParam(ADNumImages, &numImages);

		setIntegerParam(ADStatus, ADStatusAcquire);
		callParamCallbacks();

		/* get an image buffer from the pool */
		getIntegerParam(NDArraySizeX, &dims[0]);
		getIntegerParam(NDArraySizeY, &dims[1]);
		getIntegerParam(NDDataType, (int *) &dataType);
		pImage = this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);

		/* We release the mutex when acquire image, because this may take a long time and
		 * we need to allow abort operations to get through */
		this->unlock();
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: collect data from electron analyser\n", driverName, functionName);
		status = this->acquireData(pImage->pData);
		this->lock();
		/* If there was an error jump to bottom of the loop */
		if (status)
		{
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: probelm in collecting data from electron analyser \n",driverName, functionName);
			setStringParam(ADStatusMessage,
					"Failed to collect data from electron analyser.");
			acquire = 0;
			pImage->release();
			continue;
		}

		nbytes = (dims[0] * dims[1]) * sizeof(dataType);
		pImage->dims[0].size = dims[0];
		pImage->dims[1].size = dims[1];

		/* Set a bit of areadetector image/frame statistics... */
		getIntegerParam(ADNumImages, &numImages);
		getIntegerParam(NDArrayCounter, &imageCounter);
		getIntegerParam(ADNumImagesCounter, &numImagesCounter);
		getIntegerParam(ADImageMode, &imageMode);
		getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
		numImagesCounter++;
		imageCounter++;
		setIntegerParam(ADNumImagesCounter, numImagesCounter);
		setIntegerParam(NDArrayCounter, imageCounter);
		setIntegerParam(NDArraySize, nbytes);

		pImage->uniqueId = imageCounter;
		pImage->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;

		/* Get any attributes that have been defined for this driver */
		this->getAttributes(pImage->pAttributeList);

		pImage->report(2); // print debugging info

		if (arrayCallbacks)
		{
			/* Must release the lock here, or we can get into a deadlock, because we can
			 * block on the plugin lock, and the plugin can be calling us */
			this->unlock();
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: calling NDArray callback\n", driverName, functionName);
			doCallbacksGenericPointer(pImage, NDArrayData, 0);
			this->lock();
		}
		/* Free the image buffer */
		pImage->release();

		/* check to see if acquisition is done */
		if ((imageMode == ADImageSingle) || ((imageMode == ADImageMultiple)
				&& (numImagesCounter >= numImages)))
		{
			setIntegerParam(ADNumExposuresCounter, 0);
			setIntegerParam(ADNumImagesCounter, 0);
			setIntegerParam(ADAcquire, 0);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition completed\n", driverName, functionName);
		}
		/* Call the callbacks to update any changes */
		callParamCallbacks();
		getIntegerParam(ADAcquire, &acquire);

		/* If we are acquiring then sleep for the acquire period minus elapsed time. */
		if (acquire)
		{
			epicsTimeGetCurrent(&endTime);
			elapsedTime = epicsTimeDiffInSeconds(&endTime, &startTime);
			delay = acquirePeriod - elapsedTime;
			if (delay >= 0.0)
			{
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: delay=%f\n",driverName, functionName, delay);
				/* We set the status to indicate we are in the period delay */
				setIntegerParam(ADStatus, ADStatusWaiting);
				callParamCallbacks();
				this->unlock();
				status = epicsEventWaitWithTimeout(this->stopEventId, delay);
				this->lock();
			}
		}
	}
}

/** Grab image off the Frame Grabber.
 * This function expects that the driver to locked already by the caller.
 *
 */
asynStatus ElectronAnalyser::acquireData(void *pData)
{
	asynStatus status = asynSuccess;
	const char *functionName = "acquireData";

	return status;
}

/** Called when asyn clients call pasynInt32->write().
 * Write integer value to the drivers parameter table.
 * \param pasynUser
 * \param value
 * \return asynStatus Either asynError or asynSuccess
 */
asynStatus ElectronAnalyser::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	int status = asynSuccess;
	int function = pasynUser->reason;
	const char *functionName = "writeInt32";
	char * message = new char[MAX_MESSAGE_SIZE];
	int size = 0;

	// parameters for functions
	int adstatus;

	status = setIntegerParam(function, value);
	getIntegerParam(ADStatus, &adstatus);

	switch (function)
	{
	case ADAcquire:
		if (value && (adstatus == ADStatusIdle))
		{
			/* Send an event to wake up the electronAnalyser task.  */
			epicsEventSignal(this->startEventId);
		}
		if (!value && (adstatus != ADStatusIdle))
		{
			/* Stop acquiring ( abort any hardware processings ) */
			epicsEventSignal(this->stopEventId);
		}
		break;
	case AlwaysDelayRegion:
		if (value) {
			m_bAlwaysDelayRegion = true;
		} else {
			m_bAlwaysDelayRegion = false;
		}
		this->setAlwaysDelayRegion(&m_bAlwaysDelayRegion);
		break;
	case AllowIOWithDetector:
		if (value)
		{
			m_bAllowIOWithDetector = true;
		} else {
			m_bAllowIOWithDetector = false;
		}
		this->setAllowIOWithDetector(&m_bAllowIOWithDetector);
		break;
	case DetectorFirstXChannel:
		if (value < 0 || value > detectorInfo.xChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between 0 and %d", detectorInfo.xChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.firstXChannel_=value;
			setIntegerParam(ADMinX, detector.firstXChannel_);
		}
		break;
	case DetectorLastXChannel:
		if (value < detector.firstXChannel_ || value > detectorInfo.xChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between %d and %d", detector.firstXChannel_,detectorInfo.xChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.lastXChannel_=value;
			setIntegerParam(ADSizeX, detector.lastXChannel_- detector.firstXChannel_);
		}
		break;
	case DetectorFirstYChannel:
		if (value < 0 || value > detectorInfo.yChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between 0 and %d", detectorInfo.yChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.firstYChannel_=value;
			setIntegerParam(ADMinY,detector.firstYChannel_);
		}
		break;
	case DetectorLastYChannel:
		if (value < detector.firstYChannel_ || value > detectorInfo.yChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between %d and %d", detector.firstYChannel_,detectorInfo.yChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.lastYChannel_=value;
			setIntegerParam(ADSizeY, detector.lastYChannel_ - detector.firstYChannel_);
		}
		break;
	case DetectorSlices:
		if (value < 1 || value > detectorInfo.maxSlices_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between 1 and %d", detectorInfo.maxSlices_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.slices_=value;
		}
		break;
	case DetectorMode:
		if (value)
		{
			// use Detector ADC mode
			detector.adcMode_ = true;
		}
		else
		{
			// use Pulse counting mode
			detector.adcMode_ = false;
		}
		break;
	case DetectorDiscriminatorLevel:
		//TODO any constrains?
		detector.discLevel_=value;
		break;
	case DetectorADCMask:
		//TODO any constrains?
		detector.adcMask_=value;
		break;
	case AnalyzerAcquisitionMode:
		if (value)
		{
			// use fixed mode
			analyzer.fixed_ = true;
		}
		else
		{
			// use swept mode
			analyzer.fixed_ = false;
		}
		break;
	case EnergyMode:
		if (value)
		{
			// use Kinetic energy scale
			analyzer.kinetic_ = true;
		}
		else
		{
			// use Binding energy scale
			analyzer.kinetic_ = false;
		}
		break;
	case AnalyzerDwellTime:
		if (value <= 0) {
			epicsSnprintf(message, sizeof(message), "Analyzer dwell time must be > 0");
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			analyzer.dwellTime_ = value;
			setDoubleParam(ADAcquireTime, value/1000.0);
		}
		break;
	case RunMode:  // driver parameters that determine how data to be saved into a file.
		if (value == 1)
			m_RunMode = AddDimension;
		else
			m_RunMode = Normal;
		break;
	case ElementSet: // need to map MEDM screen value to SES library values
		getElementSetCount(size);
		if (value < size) {
			const char * elementSet = m_Elementsets.at(value).c_str();
			// set element set to Library
			this->setElementSet(elementSet);
		} else {
			// out of index
			epicsSnprintf(message, sizeof(message), "set 'Element_Set' failed, index must be between 0 and %d", size);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		}
		getElementSet(-1, m_sCurrentElementSet, size);
		break;
	case LensMode:
		getLensModeCount(size);
		if (value <size){
			const char * lensMode = m_LensModes.at(value).c_str();
			this->setLensMode(lensMode);
		} else {
			epicsSnprintf(message, sizeof(message), "set 'Lens_Mode' failed, index must be between 0 and %d", size);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		}
		getLensMode(-1, m_sCurrentLensMode, size);
		break;
	case PassEnergy:
		getPassEnergyCount(size);
		if (value < size) {
			const double passEnergy = m_PassEnergies.at(value);
			this->setPassEnergy(&passEnergy);
		} else {
			epicsSnprintf(message, sizeof(message), "set 'Pass_Energy' failed, index must be between 0 and %d", size);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		}
		getPassEnergy(-1,m_dCurrentPassEnergy);
		break;
	case UseExternalIO:
		if (value) {
			m_bUseExternalIO = true;
		} else {
			m_bUseExternalIO = false;
		}
		this->setUseExternalIO(&m_bUseExternalIO);
		break;
	case UseDetector:
		if (value) {
			m_bUseDetector = true;
		} else {
			m_bUseDetector = false;
		}
		this->setUseExternalIO(&m_bUseDetector);
		break;
	case ResetDataBetweenIterations:
		if (value) {
			m_bResetDataBetweenIterations = true;
		} else {
			m_bResetDataBetweenIterations = false;
		}
		this->setResetDataBetweenIterations(&m_bResetDataBetweenIterations);
		break;
	case AcqSliceNumber:
		// no action, value used by get slice function
		break;
	case AcqIOPortIndex:
		// no action, value used by get IO spectrum call and get port name call.
		break;
	case ADMinX:
		if (value < 0 || value > detectorInfo.xChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between 0 and %d", detectorInfo.xChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.firstXChannel_=value;
			setIntegerParam(DetectorFirstXChannel, detector.firstXChannel_);
		}
		break;
	case ADMinY:
		if (value < 0 || value > detectorInfo.yChannels_) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be between 0 and %d", detectorInfo.yChannels_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.firstYChannel_=value;
			setIntegerParam(DetectorFirstYChannel,detector.firstYChannel_);
		}
		break;
	case ADSizeX:
		if (value > detectorInfo.xChannels_ - detector.firstXChannel_ ) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be less than %d", detectorInfo.xChannels_-detector.firstXChannel_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.lastXChannel_=value - detector.firstXChannel_;
			setIntegerParam(DetectorLastXChannel, detector.lastXChannel_);
		}
		break;
	case ADSizeY:
		if (value > detectorInfo.yChannels_ - detector.firstYChannel_ ) {
			epicsSnprintf(message, sizeof(message), "set failed, value must be less than %d", detectorInfo.yChannels_ - detector.firstYChannel_);
			setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, message);
		} else {
			detector.lastYChannel_=value - detector.firstYChannel_;
			setIntegerParam(DetectorLastYChannel, detector.lastYChannel_);
		}
		break;
	default:
		/* If this is not a parameter we have handled call the base class */
		if (function < ADLastStdParam)
			status = ADDriver::writeInt32(pasynUser, value);
		break;
	}

	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();

	if (status)
	{
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s: error, status=%d function=%d, value=%d\n",
				driverName, functionName, status, function, value);
		return asynError;
	}
	else
	{
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%d\n",
				driverName, functionName, function, value);
		return asynSuccess;
	}
	return asynSuccess;
}

/** Write double value to the drivers parameter table.
 * \param pasynUser
 * \param value
 * \return asynStatus Either asynError or asynSuccess
 */
asynStatus ElectronAnalyser::writeFloat64(asynUser *pasynUser,
		epicsFloat64 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *functionName = "writeFloat64";
	char * message = new char[MAX_MESSAGE_SIZE];
	int adstatus;

	/* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
	 * status at the end, but that's OK */
	status = setDoubleParam(function, value);
	getIntegerParam(ADStatus, &adstatus);
	switch (function)
	{
	case AnalyzerHighEnergy:
		analyzer.highEnergy_ = value;
		break;
	case AnalyzerLowEnergy:
		analyzer.lowEnergy_ = value;
		break;
	case AnalyzerCenterEnergy:
		analyzer.centerEnergy_ = value;
		break;
	case AnalyzerEnergyStep:
		analyzer.energyStep_ = value;
		break;
	case ADAcquireTime:
		analyzer.dwellTime_ = int(value * 1000);
		setIntegerParam(AnalyzerDwellTime, analyzer.dwellTime_);
		break;
	default:
		/* If this parameter belongs to a base class call its method */
		if (function < ADLastStdParam)
			status = ADDriver::writeFloat64(pasynUser, value);
		break;
	}
	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();
	if (status)
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s error, status=%d function=%d, value=%f\n",
				driverName, functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%f\n",
				driverName, functionName, function, value);
	return status;
}
/** Called when asyn clients call pasynOctet->write().
 * This function performs actions for some parameters, including ADFilePath, etc.
 * For all parameters it sets the value in the parameter library and calls any registered callbacks..
 * \param[in] pasynUser pasynUser structure that encodes the reason and address.
 * \param[in] value Address of the string to write.
 * \param[in] nChars Number of characters to write.
 * \param[out] nActual Number of characters actually written. */
asynStatus ElectronAnalyser::writeOctet(asynUser *pasynUser, const char *value,
		size_t nChars, size_t *nActual)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *functionName = "writeOctet";
	char * message = new char[MAX_MESSAGE_SIZE];
	int adstatus;

	/* Set the parameter in the parameter library. */
	status = (asynStatus) setStringParam(function, (char *) value);
	getIntegerParam(ADStatus, &adstatus);

	switch (function)
	{
	case LibWorkingDir:
		if (adstatus == ADStatusIdle)
		{
			if (access(value, 0) == 0)
			{ // check if directory exist before set it.
				struct stat st;
				stat(value, &st);

				if (st.st_mode & S_IFDIR)
				{
					epicsSnprintf(message, sizeof(message),
							"Library working directory is set to %s", value);
					setStringParam(ADStatusMessage, message);
					asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: %s", driverName, functionName, message);
					status = this->setLibWorkingDir(value);
				}
				else
				{
					epicsSnprintf(message, sizeof(message),
							"%s is a file not a directory.", value);
					setStringParam(ADStatusMessage, message);
					asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: %s", driverName, functionName, message);
				}
			}
			else
			{
				epicsSnprintf(
						message,
						sizeof(message),
						"Library working directory specified %s does not exist.",
						value);
				setStringParam(ADStatusMessage, message);
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: %s", driverName, functionName, message);
			}
		}
		break;
	case RegionName:
		if (adstatus == ADStatusIdle)
		{
			status = this->setRegionName(value);
		}
		break;
	case TempFileName:
		if (adstatus == ADStatusIdle)
		{
			status = this->setTempFileName(value);
		}
		break;

	default:
		/* If this parameter belongs to a base class call its method */
		if (function < ADLastStdParam)
			status = ADDriver::writeOctet(pasynUser, value, nChars, nActual);
		break;
	}

	/* Do callbacks so higher layers see any changes */
	status = (asynStatus) callParamCallbacks();

	if (status)
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
				"%s:%s: status=%d, function=%d, value=%s", driverName,
				functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%s\n",
				driverName, functionName, function, value);
	*nActual = nChars;
	return status;
}

/** Report status of the driver for debugging/testing purpose. Can be invoked from ioc shell.
 * Prints details about the driver if details>0.
 * It then calls the ADDriver::report() method.
 * \param[in] fp File pointed passed by caller where the output is written to.
 * \param[in] details If >0 then driver details are printed.
 */
void ElectronAnalyser::report(FILE *fp, int details)
{
	fprintf(fp, "electronAnalyser detector %s\n", this->portName);
	if (details > 0)
	{
		int nx, ny, dataType;
		getIntegerParam(ADSizeX, &nx);
		getIntegerParam(ADSizeY, &ny);
		getIntegerParam(NDDataType, &dataType);
		fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
		fprintf(fp, "  Data type:         %d\n", dataType);
	}
	/* Invoke the base class method */
	ADDriver::report(fp, details);
}

/** Map the drivers enum parameters to their string representation in electronAnalyserParamString[].
 * This is a boilerplate function and not need to change it at all. */
asynStatus ElectronAnalyser::drvUserCreate(asynUser *pasynUser,
		const char *drvInfo, const char **pptypeName, size_t *psize)
{
	asynStatus status;
	//const char *functionName = "drvUserCreate";
	status = this->drvUserCreateParam(pasynUser, drvInfo, pptypeName, psize,
			electronAnalyserParamString, NUM_ELECTRONANALYSER_PARAMS);

	/* If not, then call the base class method, see if it is known there */
	if (status)
		status = ADDriver::drvUserCreate(pasynUser, drvInfo, pptypeName, psize);
	return (status);
}


asynStatus ElectronAnalyser::getDetectorTemperature(float * temperature)
{
	int eaStatus = 0;
	asynStatus status = asynSuccess;
	const char * functionName = "getDetectorTemperature";
	//TODO  get temperature from analyser
	*temperature = 20.0;
	return status;
}

/**
 * @brief check if call to instrument API return error.
 *
 * If the call returns error status, it converts the status code to error message, add to Asyn log and
 * sets the Area Detector's ADStatusMessage field, update any client.
 *
 * @param[in] err - the return status code
 * @param[in] functionName - the function name where WSESWrapper API call is made.
 * @return true if error in call return, false if err==0.
 */
bool ElectronAnalyser::isError(int & err, const char * functionName)
{
    if(err!=0){
		string msg = WError::instance()->message(err);
		this->setStringParam(ADStatusMessage,msg.c_str());
		callParamCallbacks();
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, msg.c_str());
		return true;
	}
    return false;
}

/*!
 * device descruction.
 *
 * This method will abort any currently executing acquisition, and close the SESInstrument.dll library.
 * It will be called at device destruction or at init command.
 */
void ElectronAnalyser::delete_device()
{
	//	Delete device's allocated object
	if (ses)
	{
		ses->finalize();
		delete ses;
	}
	// delete all cached variables
	if (!sesWorkingDirectory.empty())
	{
		sesWorkingDirectory.clear();
	}
	if (!instrumentFilePath.empty())
	{
		instrumentFilePath.clear();
	}
	if (werror != NULL) {
		werror->release();
	}
}

/**
 * create and initialise the device annd instrument software library. Must be called in constructor.
 */
void ElectronAnalyser::init_device(const char *workingDir, const char *instrumentFile)
{
	const char * functionName = "init_device()";
	char *  message = new char[MAX_MESSAGE_SIZE];
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: create device", driverName, functionName);
	// Initialise variables to default values
	sesWorkingDirectory = workingDir;
	instrumentFilePath = sesWorkingDirectory.append("\\data\\").append(instrumentFile);

	// Get connection to the SES wrapper
	ses = new WSESWrapperMain(workingDir);
	int err = ses->setProperty("lib_working_dir", 0, workingDir);
	err |= ses->initialize(0);
	if (err)
	{
		this->setIntegerParam(ADStatus, ADStatusError);
		epicsSnprintf(message, sizeof(message), "SES library initialisation failed: %s", werror->message(err));
		this->setStringParam(ADStatusMessage,message);
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s\n", driverName, functionName, message);
	}
	else
	{
		err = ses->loadInstrument(instrumentFilePath.c_str());
		if (err)
		{
			this->setIntegerParam(ADStatus, ADStatusError);
			epicsSnprintf(message, sizeof(message), "LoadInstrument file: %s failed; %s.", instrumentFilePath, werror->message(err));
			this->setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s. ", driverName, functionName, message);
		}
		else
		{
			if (ses->isInitialized())
			{
				this->setIntegerParam(ADStatus, ADStatusIdle);
				this->setStringParam(ADStatusMessage,"SES library initialisation completed.");
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: SES library initialisation completed.", driverName, functionName);
			}
			else
			{
				this->setIntegerParam(ADStatus, ADStatusError);
				this->setStringParam(ADStatusMessage,"SES initialisation failed");
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: SES nitialisation failed", driverName, functionName);
			}
		}
	}
	callParamCallbacks();
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: - out", driverName, functionName);
}

/**
 * update database and EDM screen status for the instrument.
 */
void ElectronAnalyser::updateStatus()
{
	const char * functionName = "updateStatus()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int status=0;
	this->getInstrumentStatus(&status);
	switch(status)
	{
	case SesNS::Normal:
		this->setIntegerParam(ADStatus, ADStatusIdle);
		this->setStringParam(ADStatusMessage,"Analyser READY.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Analyser READY.", driverName, functionName);
		break;
	case  SesNS::Running:
		this->setIntegerParam(ADStatus, ADStatusAcquire);
		this->setStringParam(ADStatusMessage,"Analyser BUSY.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Analyser BUSY.", driverName, functionName);
		break;
	case  SesNS::AcqError :
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"Acquisition was interrupted with an error.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: Acquisition was interrupted with an error.", driverName, functionName);
		break;
	case SesNS::NonOperational:
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"The library is not operational. Resetting may resolve the issue.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: The library is not operational. Resetting may resolve the issue.", driverName, functionName);
		break;
	case SesNS::NotInitialized :
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"The SESInstrument library has not been initialized (the GDS_Initialize function has not been called).");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: The SESInstrument library has not been initialized (the GDS_Initialize function has not been called).", driverName, functionName);
		break;
	}

	int step = 0;
	int dummy = 0;
	this->getAcqCurrentStep(step);

	int channels = 0;
	this->getAcqChannels(channels);
	if(step>channels){
		step = channels;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Current step: %d", driverName, functionName, step);
	callParamCallbacks();
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
}

/*!
 * Reads the current kinetic energy from the SESInstrument library.
 *
 * @param[out] kineticEnergy Pointer to a double to be modified with the current kinetic energy.
 * @return asynError if kinetic energy can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getKineticEnergy(double *kineticEnergy)
{
	const char * functionName = "getKineticEnergy(double *kineticEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getKineticEnergy(kineticEnergy);
    if(isError(err, functionName)) return asynError;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/*!
 * Changes the kinetic energy. Use this member function when initAcquisition() and startAcquisition() is not
 * going to be called, e.g. when controlling the Scienta analyzer with an external, third-party detector to make
 * measurements.
 *
 * @param[in] kineticEnergy The kinetic energy (in eV) to be set.
 * @return asynError if kinetic energy can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setKineticEnergy(const double kineticEnergy)
{
	const char * functionName = "setKineticEnergy(const double kineticEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err =  ses->setKineticEnergy(kineticEnergy);
    if(isError(err, functionName)) return asynError;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Reads the current voltage from analyzer element @p elementName.
 *
 * @param[in] elementName A string specifying the name of the element to be read.
 * @param[out] voltage A pointer to a variable that will receive the voltage (in V).
 *
 * @return asynError if loadInstrument() has not been called, or if the element could not be read
 * 			(e.g. the element @p elementName does not exist), otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getElementVoltage(const char *elementName, double *voltage)
{
	const char * functionName = "getElementVoltage(const char *elementName, double *voltage)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getElementVoltage(elementName, voltage);
    if(isError(err, functionName)) return asynError;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/**
 * @brief Changes the voltage of analyzer element @p elementName.
 *
 * @param[in] elementName A string specifying the name of the element to be modified.
 * @param[in] voltage The new voltage (in V).
 *
 * @return asynError if loadInstrument() has not been called, or if the element voltage could not be set
 * 			(e.g. the element @p elementName does not exist), otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setElementVoltage(const char *elementName, const double voltage)
{
	const char * functionName = "setElementVoltage(const char *elementName, const double voltage)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err =  ses->setElementVoltage(elementName, voltage);
    if(isError(err, functionName)) return asynError;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Changes the acquisition mode of analyzer region definition.
 *
 * This setting only applies to hardware library at initAcquisition().
 *
 * @param[in] b @c true for fixed, @c false for swept
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setAcquisitionMode(const bool b){
	this->analyzer.fixed_=b;
	return asynSuccess;
}
/**
 * @brief gets the current acquisition mode of the analyzer
 * @param[out] b @c true for fixed, @c false for swept
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getAcquisitionMode(bool *b){
	*b = this->analyzer.fixed_;
	return asynSuccess;
}
/**
 * @brief Changes the energy mode of analyzer region definition.
 *
 * This setting only applies to hardware library at initAcquisition().
 *
 * @param[in] b @c true for kinetic energy, @c false for binding energy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setEnergyMode(const bool b){
	this->analyzer.kinetic_=b;
	return asynSuccess;
}
/**
 * @brief gets the current energy mode of the analyzer
 * @param b @c true for kinetic energy, @c false for binding energy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getEnergyMode(bool *b){
	*b=this->analyzer.kinetic_;
	return asynSuccess;
}

/// ######################## integration methods ######################
/**
 * @brief start acquisition
 *
 * This method sets detector region and analzer region, then initialise acquisition before start.
 * This method also changes the EPICS areaDetector status to @p ADStatusAcquire on successfully start,
 * or @p ADStatusError if failed to start.
 *
 * @return asynError if @p detector_region, @p analyzer_region, @fn initAcquisition(false, false), or
 * @fn startAcquisition() failed, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::start()
{
	const char * functionName = "start()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	// set acquisition parameters on the wrapper
	int err=ses->setProperty("detector_region", 0, &detector);
	if (isError(err, functionName)) {
		return asynError;
	}
	err = ses->setProperty("analyzer_region", 0, &analyzer);
	if (isError(err, functionName)) {
		return asynError;
	}

	err = ses->initAcquisition(false, false);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition initialisation completed.", driverName, functionName);
	setStringParam(ADStatusMessage, "acquisition initialisation completed.");
	callParamCallbacks();
	int steps=0;
	double dtime=0;
	double minEnergyStep=0;
	err=ses->checkAnalyzerRegion(&analyzer, &steps, &dtime, &minEnergyStep);
	if (isError(err, functionName)) {
		return asynError;
	} else {
		char * message = new char[MAX_MESSAGE_SIZE];
		epicsSnprintf(message, sizeof(message),"Number of steps: %d; Dwell time: %f; minimum energy step: %f.",steps, dtime, minEnergyStep );
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: %s.", driverName, functionName, message);
		setStringParam(ADStatusMessage, message);
		callParamCallbacks();
	}
	//TODO  should this be here?
	err = ses->startAcquisition();
	setIntegerParam(ADStatus, ADStatusAcquire);
	if (isError(err, functionName)) {
		setIntegerParam(ADStatus, ADStatusError);
		callParamCallbacks();
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: start acquisition.", driverName, functionName);
	setStringParam(ADStatusMessage, "start acquisition.");
	callParamCallbacks();
	return asynSuccess;
}
/**
 * @brief stop acquisition.
 *
 * This method changes the EPICS areaDetector status to @p ADStatusIdle on success.
 *
 * @return asynError if stop failed, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::stop()
{
	const char * functionName = "stop()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->stopAcquisition();
	if (isError(err, functionName)) {
		setStringParam(ADStatusMessage, "error stop acquisition.");
		callParamCallbacks();
		return asynError;
	}
	setIntegerParam(ADStatus, ADStatusIdle);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition stopped.", driverName, functionName);
	setStringParam(ADStatusMessage, "acquisition stopped.");
	callParamCallbacks();
	return asynSuccess;
}
/**
 * @brief Resets the instrument and puts it in a default state.
 *
 * @return asynError if reset failed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::resetInstrument()
{
	const char * functionName = "resetInstrument()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err =ses->resetHW();
	if (isError(err, functionName)) {
		setStringParam(ADStatusMessage, "error reset instrument.");
		callParamCallbacks();
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Sets all voltage elements to zero.
 *
 * @return asynError if the voltages could not be set, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::zeroSupplies()
{
	const char * functionName = "resetSupplies()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err =ses->zeroSupplies();
	if (isError(err, functionName)) {
		setStringParam(ADStatusMessage, "error reset supplies to zero.");
		callParamCallbacks();
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Tests the hardware to check communication status.
 *
 * @return asynError if communication failed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::testCommunication()
{
	const char * functionName = "testCommunication()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->testHW();
	if (isError(err, functionName)) {
		setStringParam(ADStatusMessage, "error test communication.");
		callParamCallbacks();
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the list of available lens modes.
 *
 * @param [out] pLensModeList - pointer to the list held in a vector of string.
 * @return asynError if @p lens_mode_count or @p lens_mode_from_index properties can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getLensModeList(NameVector *pLensModeList)
{
	const char * functionName = "getLensModeList(std::vector<string> *pLensModeList)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int max =0;
	int err = ses->getProperty("lens_mode_count",0,&max);
	if (isError(err, functionName)) {
		return asynError;
	}

	for(int i=0; i<max;i++)
	{
		char* lens = 0;
		int size  = 0;
		err = ses->getProperty("lens_mode", i, lens, size); // ther is not @c lens_mode_from_index defined in the wrapper
		if (isError(err, functionName)) {
			return asynError;
		}
		lens =  new char[size];
		err = ses->getProperty("lens_mode",i,lens, size);
		if (isError(err, functionName)) {
			delete [] lens;
			return asynError;
		}
		pLensModeList->push_back(lens);
		delete [] lens;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the list of installed element sets.
 *
 * @param [out] pEelementSetList - pointer to the list held in the vector of string
 * @return asynError if @p element_set_count or @p element_set_from_index properties can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getElementSetLlist(NameVector *pElementSetList)
{
	const char * functionName = "getElementSetLlist(std::vector<string> *pElementSetList)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int max =0;
	int err = ses->getProperty("element_set_count",0,&max);
	if (isError(err, functionName)) {
		return asynError;
	}

	for(int i=0; i<max;i++)
	{
		char* set = 0;
		int size  = 0;
		err = ses->getProperty("element_set", i, set, size); // there is no @c element_set_from_index defined in the wrapper
		if (isError(err, functionName)) {
			return asynError;
		}
		set =  new char[size];
		err = ses->getProperty("element_set",i,set, size);
		if (isError(err, functionName)) {
			delete [] set;
			return asynError;
		}
		pElementSetList->push_back(set);
		delete [] set;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets list of available pass energies for the current lens mode.
 *
 * @param [out] pPassEnergyList - pointer to the list held in a vector of double
 * @return asynError if @p pass_energy_count or @p pass_energy_from_index properties can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getPassEnergyList(DoubleVector *pPassEnergyList)
{
	const char * functionName = "getPassEnergyList(std::vector<double> *pPassEnergyList)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int max =0;
	int err = ses->getProperty("pass_energy_count",0,&max);
	if (isError(err, functionName)) {
		return asynError;
	}

	for(int i=0; i<max;i++)
	{
		double passE = 0;
		err = ses->getProperty("pass_energy",i,&passE); // there is no @c pass_energy_from_index defined in wrapper
		if (isError(err, functionName)) {
			return asynError;
		}
		pPassEnergyList->push_back(passE);
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/***********************************************************************************//**
 * 			Access methods for properties defined in WSESWapperBase
 * *********************************************************************************/
/**
 * @brief get the description of the library.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length for the description.
 *
 * @param[out] value - A @c char* buffer to be filled with the description. Can be 0 (NULL).
 * @param[in,out] size - If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns asynSuccess.
 */
asynStatus ElectronAnalyser::getLibDescription(char *value, int &size)
{
	const char * functionName = "getLibDescription(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	ses->getProperty("lib_description", 0, value, size);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the version of the library.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length for the version.
 *
 * @param[out] value A @c char* buffer to be filled with the version. The syntax is @code <major>.<minor>.<build> @endcode.
 *              Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns asynSuccess.
 */
asynStatus ElectronAnalyser::getLibVersion(char *value, int &size)
{
	const char * functionName = "getLibVersion(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	ses->getProperty("lib_version", 0, value, size);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the error message corresponding to the @p index error code.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length for
 * the error indicated by @p index.
 *
 * @param[in] index An error code, e.g. from a previously returned function. If @p index is not a valid error code,
 *              the error string is "Unknown Error".
 * @param[out] value A @c char* buffer to be filled with the error message connected to error code @p index. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns asynSuccess;
 */
asynStatus ElectronAnalyser::getLibError(int index, char *value, int &size)
{
	const char * functionName = "getLibError(int index, char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	ses->getProperty("lib_error", index, value, size);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the working directory for the current application.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length.
 *
 * @param[out] value - A @c char* buffer to be filled with the full path of the current working directory. Can be 0 (NULL).
 * @param[in,out] size - If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if @c lib_working_dir can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getLibWorkingDir(char *value, int &size)
{
	const char * functionName = "getLibWorkingDir(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("lib_working_dir", 0, value);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief set the working directory for the current application.
 *
 * Changes the working directory for the library.
 *
 * @param[in] value A null-terminated C string that contains the path to the new working directory.
 *
 * @return asynError if @c lib_working_dir  can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setLibWorkingDir(const char *value)
{
	const char * functionName = "setLibWorkingDir(const char *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("lib_working_dir", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the status of the instrument.
 *
 * @param[out] value Pointer to a 32-bit integer. Possible values are given by SesNS::InstrumentStatus.
 *
 * @return asynError if @c instrument_status can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getInstrumentStatus(int *value)
{
	const char * functionName = "getInstrumentStatus(int *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("instrument_status", 0, value);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get enable (c true) or disable (@c false) state for the region delay.
 *
 * The @p value parameter must be a pointer to a 1-byte boolean.
 * If @p value is 1 or @c true, there is a delay before starting a measurement.
 *
 * @param[out] value - Pointer to a 1-byte boolean.
 *
 * @return asynError If WSESWrapperMain::loadInstrument() has not been called, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAlwaysDelayRegion(bool *value)
{
	const char * functionName = "getAlwaysDelayRegion(bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("always_delay_region", 0, value);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief enable (c true) or disable (@c false) the region delay to be applied even when HV supplies are not changed.
 *
 * If @c true (1), a delay will be made before starting the acquisition of a region.
 *
 * @param[in] value - A 1-byte boolean of value @c true (1) or @c false (0).
 *
 * @return asynError if the library has not been successfully initialized, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setAlwaysDelayRegion(const bool *value)
{
	const char * functionName = "setAlwaysDelayRegion(const bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("always_delay_region", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get enable (c true) or disable (@c false) state for simultaneous external I/O communication with detector communication.
 *
 * The @p value parameter must be a pointer to a 1-byte boolean.
 *
 * If @p value is 1 or @c true, external I/O signalling and detector can be used simultaneously.
 *
 * @param[out] value - Pointer to a 1-byte boolean.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAllowIOWithDetector(bool *value)
{
	const char * functionName = "getAllowIOWithDetector(bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("allow_io_with_detector", 0, value);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief enable (c true) or disable (@c false) simultaneous external I/O communication with detector communication.
 *
 * @param[in] value A 1-byte boolean of value @c true (1) or @c false (0).
 *
 * @return WError::ERR_NOT_INITIALIZED if the library has not been successfully initialized, otherwise WError::ERR_OK.
 */
asynStatus ElectronAnalyser::setAllowIOWithDetector(const bool *value)
{
	const char * functionName = "setAllowIOWithDetector(const bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("allow_io_with_detector", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the name of currently installed instrument.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length.
 * If WSESWrapperMain::loadInstrument() has not been called, @p value is usually empty, but do not rely on it.
 *
 * @param[out] value - A @c char* buffer to be filled with the name of the instrument model. Can be 0 (NULL).
 * @param[in,out] size - If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if @c instrument_model can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getInstrumentModel(char *value, int &size)
{
	const char * functionName = "getInstrumentModel(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("instrument_model", 0, value, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the serial number of the currently installed instrument.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length.
 * If WSESWrapperMain::loadInstrument() has not been called, @p value is usually empty, but do not rely on it.
 *
 * @param[out] value - A @c char* buffer to be filled with the name of the instrument serial number. Can be 0 (NULL).
 * @param[in,out] size - If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if @c instrument_serial_no can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getInstrumentSerialNo(char *value, int &size)
{
	const char * functionName = "getInstrumentSerialNo(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("instrument_serial_no", 0, value, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the DetectorInfo struct.
 *
 * @param[out] value - A pointer to a SESWrapperNS::DetectorInfo structure.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getDetectorInfo(SESWrapperNS::DetectorInfo *value)
{
	const char * functionName = "getDetectorInfo(SESWrapperNS::DetectorInfo *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);

	int err = ses->getProperty("detector_info", 0, value);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the DetectorRegion structure.
 *
 * @param[out] value - A pointer to a SESWrapperNS::DetectorRegion structure.
 *
 * @return asynError if WSesWrapperMain::loadInstrument() has not been called,
 *         otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getDetectorRegion(SESWrapperNS::DetectorRegion *value)
{
	const char * functionName = "getDetectorRegion(SESWrapperNS::DetectorRegion *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("detector_region", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the Detector Region struct.
 *
 * This will set up a new ROI (Region of Interest) for the acquisition.
 *
 * @param[in] value - A pointer to a SESWrapperNS::DetectorRegion structure.
 *
 * @return asynError if WSESWrapper::loadInstrument() has not been called, or
 *         an error is detected in the detector region, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setDetectorRegion(const SESWrapperNS::DetectorRegion *value)
{
	const char * functionName = "setDetectorRegion(const SESWrapperNS::DetectorRegion *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("detector_region", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/**
 * @brief Getter for the @c element_set_count property.
 *
 * The @p value parameter must be a pointer to a 32-bit integer.
 *
 * @param[out] value A 32-bit integer that will contain the number of available element sets.
 * 				This value can later be used as an index to the @c element_set_from_index property getter.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getElementSetCount(int & value)
{
	const char * functionName = "getElementSetCount(int & value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("element_set_count", 0, &value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the current element set or pass mode.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length for the requested element set.
 *
 * @param[in] index - If set to -1, obtains the current element set. If 0 <= @p index < @c element_set_count, obtains the element set name for that index.
 * @param[out] elementSet - A @c char* buffer to be filled with an element set. Can be 0 (NULL).
 * @param[in,out] size - If @p elementSet is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting buffer.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called,
 *         failed to report the current element set, or if index is out-of-range, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getElementSet(int index, char * elementSet, int & size)
{
	const char * functionName = "getElementSet(int index, char * elementSet, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("element_set", index, elementSet, size);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief sets the element set for next acquisition.
 *
 * @param elementSet - A null-terminated string that specifies the name of the new element set.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, or
 *         		if the given element set is not available, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setElementSet(const char * elementSet)
{
	const char * functionName = "setElementSet(const char * elementSet)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("element_set", -1, elementSet);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the number of available lens modes
 *
 * The @p value parameter must be a pointer to a 32-bit integer.
 *
 * @param[out] value - A 32-bit integer that will contain the number of available lens modes.
 * 				This value can later be used as an index to the @c lens_mode_from_index property getter.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getLensModeCount(int & value)
{
	const char * functionName = "getLensModeCount(int & value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("lens_mode_count", 0, &value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the lens mode.
 *
 * If the @p value parameter is 0, @p size will be modified to return the required buffer length for the requested lens mode.

 * @param[in] index - If set to -1, obtains the current lens mode. If 0 <= @p index < @c lens_mode_count, obtains the lens mode name for that index.
 * @param[out] lensMode - A @c char* buffer to be filled with the current lens mode. Can be 0 (NULL).
 * @param[in,out] size - If @p lensMode is 0, this parameter is modified with the required size of the buffer, otherwise it is
 *             used to determine the size of @c value.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called,
 *         failed to report the current lens mode, or if index is out-of-range, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getLensMode(int index, char * lensMode, int & size)
{
	const char * functionName = "getLensMode(int index, char * lensMode, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("lens_mode", index, lensMode, size);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief sets the lens mode for next acquisition.
 *
 * This function reloads the pass energy list when successful.
 * This requires updates for the pass energy lists for the calling application.
 *
 * @param [in] lensMode - A null-terminated string that specifies the name of the new lens mode.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, or
 *         if the given lens mode is not available, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setLensMode(const char * lensMode)
{
	const char * functionName = "setLensMode(const char * lensMode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("lens_mode", -1, lensMode);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/*!
 * Get the number of available pass energies for the current lens mode.
 *
 * The @p value parameter must be a pointer to a 32-bit integer.
 * @note The number of available pass energies is dependent on the current lens mode. If you change the lens
 *       mode, you need to update your internal list of pass energies, beginning with this function.
 *
 * @param[out] value - A 32-bit integer that will contain the number of available pass energies. This value can later
 * 				be used as an index to the @c pass_mode_from_index property getter.
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getPassEnergyCount(int & value)
{
	const char * functionName = "getPassEnergyCount(int & value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("pass_energy_count", 0, &value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the pass energy.
 *
 * @param[in] index - If set to -1, obtains the current pass energy.
 * 					  If 0 <= @p index < @c pass_energy_count, obtains the pass energy for that index.
 * @param [out] passEnergy - A pointer to a @c double to be filled with the pass energy.
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, failed to report the current pass energy,
 * 					 or if @p index is out-of-range, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getPassEnergy(int index, double &passEnergy )
{
	const char * functionName = "getPassEnergy(int index, double &passEnergy )";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("pass_energy", index, &passEnergy);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the pass energy for next acquisition.
 *
 * If the pass energy to set is taken from a list of pass energies, make sure to update that list
 * after modifying the lens mode and before calling this function.
 *
 * @param [in] passEnergy - A pointer to a @c double (8-byte floating point).
 *
 * @return asynError if WSESWrapperMain::loadInstrument() has not been called, or
 * 			if the given pass energy is not available, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setPassEnergy(const double * passEnergy)
{
	const char * functionName = "setPassEnergy(const double * passEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("pass_energy", -1, passEnergy);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the @c analyzer_region property.
 *
 * @param[out] value - A pointer to a SESWrapperNS::WAnalyzerRegion structure.
 *
 * @return asynError if property @c analyzer_region can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAnalyzerRegion(SESWrapperNS::WAnalyzerRegion *value)
{
	const char * functionName = "getUseExternalIO(bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("analyzer_region", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the @c analyzer_region property.
 *
 * These settings may be modified by the library when calling WSesWrapper::checkRegion().
 *
 * The WAnalyzerRegion structure contains the region setting s for the next acquisition which is used in a call to
 * WSESWrapperMain::initAcquisition().
 *
 * There is no immediate checking done of the validity of the given values.
 * If such a check is required before starting acquisition, the WSESWrapperMain::checkAnalyzerRegion() can be called.
 *
 * @param[in] value - A pointer to a WAnalyzerRegion structure.
 *
 * @return asynError if property @c analyzer_region can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setAnalyzerRegion(const SESWrapperNS::WAnalyzerRegion *value)
{
	const char * functionName = "setAnalyzerRegion(const SESWrapperNS::WAnalyzerRegion *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("analyzer_region", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the boolean property that indiactes if the external I/O interface is used (@c true) or not (@c false).
 *
 * This is often used with spin detectors.
 *
 * @param[out] value A pointer to a 1-byte boolean. If @c value is 0 (or @c false), no communication with the external
 *              I/O card is made. One example of an external I/O card is the DAQ cards from National Instruments.
 *
 * @return asynError if property @c use_external_io can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getUseExternalIO(bool *value)
{
	const char * functionName = "getUseExternalIO(bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("use_external_io", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the boolean property @c use_external_io property to enable (@c true) or disable (@c false)
 * the external I/O interface.
 *
 * Modify this to toggle the use of the external I/O card, if present.
 * This is often used with spin detectors.
 *
 * @param[in] value A pointer to a 1-byte boolean.
 *
 * @return asynError if property @c use_external_io can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setUseExternalIO(const bool *value)
{
	const char * functionName = "setUseExternalIO(const bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("use_external_io", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the boolean property that indicates if the detector is used (@c true) or not (@c false).
 *
 * @param[out] value - A pointer to a 1-byte boolean. If @c value is 0 (or @c false), no communication with the detector
 *              is made.

 * @return asynError if property @c use_detector can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getUseDetector(bool *value)
{
	const char * functionName = "getUseDetector(bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("use_detector", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the boolean property @c use_detector to enable or disable the detector.
 *
 * Modify this to toggle the use of the detector. Can be used in combination with the @c use_external_io property.
 *
 * @param[in] value - A pointer to a 1-byte boolean.
 *
 * @return asynError if property @c use_detector can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setUseDetector(const bool *value)
{
	const char * functionName = "setUseDetector(const bool *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("use_detector", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the name of the current region.
 *
 * @param[out] value - A @c char* buffer to be filled with the name of the region. Can be 0 (NULL).
 * @param[in,out] size If @p value is 0, this parameter is modified with the required length of the region name, otherwise
 *             it is used to determine the size of @p value.
 *
 * @return asynError if property @c region_name can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getRegionName(char *value, int &size)
{
	const char * functionName = "getRegionName(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("region_name", 0, value, size);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief set the name of the current region. The name is limited to max 32 characters.
 *
 * @param[in] value - A null-terminated C string that specifies the region name. The name is limited to 32 characters,
 *              including the null character.
 *
 * @return asynError if property @c region_name can not be set, otherwise asynSuccess.
  */
asynStatus ElectronAnalyser::setRegionName(const char *value)
{
	const char * functionName = "setRegionName(const char *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("region_name", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the name of the temporary file created when performing an acquisition.
 *
 * @param[out] value - A @c char* buffer to be filled with the name of the temporary working file created during acquisition.
 *              Can be 0 (NULL).
 * @param[in,out] size - If @p value is 0, this parameter is modified with the required length of the file name, otherwise
 *             it is used to determine the size of @p value.
 *
 * @return asynError if property @c temp_file_name can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getTempFileName(char *value, int &size)
{
	const char * functionName = "getTempFileName(char *value, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("temp_file_name", 0, value, size);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the name of the temporary file created when performing an acquisition.
 *
 * @param[in] value - A null-terminated C string that specifies the name of the temporary file created during acquisition.
 *
 * @return asynError if property @c temp_file_name can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setTempFileName(const char *value)
{
	const char * functionName = "setTempFileName(const char *value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("temp_file_name", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Get the value of the boolean property @c reset_data_between_iterations which enable (@c true) or disable (@c false)
 * reset of spectrum and external I/O data between each iteration.
 *
 * @param[out] value A pointer to a 1-byte boolean. If @p value is 0 (or @c false), data will be accumulated
 *                   between each call to startAcquisition unless initAcquisition is called. If it is 1 (or
 *                   @c true), all data is reset to 0 between the iterations, even if initAcquisition is not called.
 *
 * @return asynError if property @c reset_data_between_iterations can not be read, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getResetDataBetweenIterations(bool * value)
{
	const char * functionName = "getResetDataBetweenIterations(bool * value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("reset_data_between_iterations", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief Set the boolean property @c reset_data_between_iterations to enable (@c true) or disable (@c false)
 * reset of spectrum and external I/O data between each iteration.
 *
 *This method operates faster than calling WSESWrapperMain::initAcquisition().

 * @param[in] reset - A pointer to a 1-byte boolean. If @p value is 0 (or @c false), data will be accumulated
 *                   between each call to startAcquisition unless initAcquisition is called. If it is 1 (or
 *                   @c true), all data is reset to 0 between the iterations, even if initAcquisition is not called.
 *
 * @return asynError if property @c reset_data_between_iterations can not be set, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::setResetDataBetweenIterations(const bool * value)
{
	const char * functionName = "setResetDataBetweenIterations(const bool * value)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("reset_data_between_iterations", 0, value);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/*******************************************************************************************//**
 *               Access methods for Data Parameters defined in WSESWrapperMain class
 *******************************************************************************************/
/**
 * @brief geta the number of channels in acquired data.
 *
 * If no acquisition has been performed, the number of slices is 0.
 *
 * @param[out] channels - A pointer to a 32-bit integer that will be modified with the number of slices in the acquired spectrum.
 * @return asynError if @p acq_channels can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqChannels(int & channels)
{
	const char * functionName = "getAcqChannels(int & channels)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the number of slices in acquired data.
 *
 * If no acquisition has been performed, the number of slices is 0.
 *
 * @param[out] slices - A pointer to a 32-bit integer that will be modified with the number of slices in the acquired spectrum.
 * @return asynError if @p acq_slices can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqSlices(int & slices)
{
	const char * functionName = "getAcqSlices(int & slices)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_slices", 0, &slices, dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the number of calls to startAcquisition() iterations that have passed
 * since last call to initAcquisition().
 *
 * The number of iterations is incremented by 1 when startAcquisition() is called.
 * The counter is reset when initAcquisition() is called.
 * If no acquisition has been performed, the number of iterations is 0.
 *
 * @param[out] iterations - A pointer to a 32-bit integer that will be modified with the number of iterations that have elapsed since
 *             the last call of initAcquisition().
 *
 * @return asynError if @p acq_iterations can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIterations(int & iterations)
{
	const char * functionName = "getAcqIterations(int & iterations)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_iterations", 0, &iterations, dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the unit of intensity scale, e.g. "count/s"
 *
 * The intensity unit is a string of up to 32 characters (including the null termination)
 * defining the unit of the intensity axis in the acquired spectrum.

 * @param[out] intensityUnit - A @c char* buffer to be filled with the intensity unit. Can be 0 (NULL).
 * @param[in,out] size - If @p intensityUnit is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAcqIntensityUnit(char * intensityUnit, int & size)
{
	const char * functionName = "getAcqIntensityUnit(char * intensityUnit, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_intensity_unit", 0, intensityUnit, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the unit of channel scale, e.g. "eV"
 *
 * The channel unit is a string of up to 32 characters (including the null termination)
 * defining the unit of the energy axis in the acquired spectrum.

 * @param[out] channelUnit -  A @c char* buffer to be filled with the channel unit. Can be 0 (NULL).
 * @param[in,out] size - If @p channelUnit is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAcqChannelUnit(char * channelUnit, int & size)
{
	const char * functionName = "getAcqChannelUnit(char * channelUnit, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_channel_unit", 0, channelUnit, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the unit of slice scale, e.g. "mm"
 *
 * The slice unit is a string of up to 32 characters (including the null termination)
 * defining the unit of the Y axis in the acquired spectrum image.
 *
 * @param[out] sliceUnit - A @c char* buffer to be filled with the channel unit. Can be 0 (NULL).
 * @param[in,out] size - If @p sliceUnit is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess.
 */
asynStatus ElectronAnalyser::getAcqSliceUnit(char * sliceUnit, int & size)
{
	const char * functionName = "getAcqSliceUnit(char * sliceUnit, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_slice_unit", 0, sliceUnit, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the integrated spectrum - i.e. the sum of data.
 *
 * The spectrum is a vector of doubles (8-byte floating point values) with the integrated intensities of all slices.
 *
 * @param [out] pSumData - An array of doubles that will be filled with the integrated spectrum. Can be 0 (NULL).
 * @param[in,out] size If @p pSumData is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqSpectrum(double * pSumData, int & size)
{
	const char * functionName = "getAcqSpectrum(double * pSumData, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_spectrum", 0, pSumData, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the 2D matrix of acquired data  - i.e. image
 *
 * This method should only be call after start acquisition, otherwise the result is undefined.
 *
 * @param [out] pData - An array of doubles that will be filled with the acquired image. Can be 0 (NULL).
 * @param[in,out] size If @p pData is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqImage(double * pData, int & size)
{
	const char * functionName = "getAcqImage(double * pData, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_image", 0, pData, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/*!
 * @brief gets one slice specified by @p index parameter from the acquired data.
 *
 * The index parameters specifies the slice to access.
 *
 * @param[in] index Specifies which slice to extract from the spectrum. Must be between 0 and the number of slices - 1.
 * @param[out] pSliceData An array of doubles that will be filled with the elements of the slice. Can be 0 (NULL).
 * @param[in,out] size If @p pSliceData is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed or if @p index is out-of-bounds, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqSlice(int index, double * pSliceData, int size)
{
	const char * functionName = "getAcqSlice(int index, double * pSliceData, int size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_slice", index, pSliceData, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the channel scale. Defines an array of doubles where each element
 * corresponds to an energy channel. The scale is always in kinetic energy.
 *
 * @param [out] pSpectrum - An array of doubles that will be filled with the channel scale. Can be 0 (NULL).
 * @param[in,out] size If @p pSpectrum is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return asynError if no acquisition has been performed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqChannelScale(double * pSpectrum, int & size)
{
	const char * functionName = "getAcqChannelScale(double * pSpectrum, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_channel_scale", 0, pSpectrum, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the slice scale. Defines an array of doubles where each element
 * corresponds to one position in the Y axis.
 *
 * @param [out] pSpectrum - An array of doubles that will be filled with the Y axis scale (the slices). Can be 0 (NULL).
 * @param[in,out] size If @p pSpectrum is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 * @return asynError if no acquisition has been performed,, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqSliceScale(double * pSpectrum, int & size)
{
	const char * functionName = "getAcqSliceScale(double * pSpectrum, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_slice_scale", 0, pSpectrum, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/*!
 * @brief get the raw image - the last image taken by the detector.
 *
 * Defines a byte array that will span the data in one frame taken from the detector.
 * If the detector is not based on frames/images, this variable will not be available.
 *
 * One frame usually has a size of xChannels * yChannels * byteSize, where xChannels and yChannels can be obtained
 * from the @c detector_info property. The byteSize variable is the number of bytes used per element in the image,
 * which is 1 for 8-bit images, or 2 for 16-bit images.
 *
 * @param [out] pImage - An array of bytes (unsigned char*) that will be filled with a snapshot of the detector image.
 * 						 Can be 0 (NULL).
 * @return asynError if @p acq_raw_image data parameters can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqRawImage(int * pImage, int &size)
{
	const char * functionName = "getAcqRawImage(int * pImage, int &size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_raw_image", 0, pImage, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the current step in a swept acquisition.
 *
 * @param [out] currentStep - A pointer to a 32-bit integer that will be set to the current step.
 * @return asynError if no acquisition has been performed, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqCurrentStep(int &currentStep)
{
	const char * functionName = "getAcqCurrentStep(int &currentStep)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int size = 0;
	int err = ses->getAcquiredData("acq_current_step", 0, &currentStep, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief get the time in milliseconds that have passed since the last call of startAcquisition().
 *
 * @param [out] elapsedTime - A pointer to a 32-bit unsigned integer that will be modified to the number of millisecondes elapsed
 *             	since the last call of startAcquisition().
 * @return always return asynSuccess
 */
asynStatus ElectronAnalyser::getAcqElapsedTime(double &elapsedTime)
{
	const char * functionName = "getAcqElapsedTime(double &elapsedTime)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int size = 0;
	ses->getAcquiredData("acq_elapsed_time", 0, &elapsedTime, size);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the number of ports available from External IO interface measurments.
 *
 * @param[out] ports - A pointer to a 32-bit integer that will be modified with the number of External I/O-ports.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOPorts(int &ports)
{
	const char * functionName = "getAcqIOPorts(int &ports)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int size = 0;
	int err = ses->getAcquiredData("acq_io_ports", 0, &ports, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the size of each vector from External IO data.
 *
 * This is used with the External I/O mechanizm, where the data is collected from a number of ports from e.g. a DAQ card.
 *
 * @param[out] dataSize - A pointer to a 32-bit integer that will be modified with the number of channels in one External I/O spectrum.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOSize(int &dataSize)
{
	const char * functionName = "getAcqIOSize(int &dataSize)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int size = 0;
	int err = ses->getAcquiredData("acq_io_size", 0, &dataSize, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the number of iterations that have elapsed since the last call of initAcquisition().
 *
 * This is used with the External I/O measurements.
 *
 * @param[out] iterations - A pointer to a 32-bit integer that will be modified with the number of iterations.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOIterations(int &iterations)
{
	const char * functionName = "getAcqIOIterations(int &iterations)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int size = 0;
	int err = ses->getAcquiredData("acq_io_iterations", 0, &iterations, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the unit of the External IO data vectors.
 *
 * The External I/O unit is a string of up to 32 characters (including the null termination)
 * defining the unit of the External I/O spectrum.
 *
 * @param[out] unit - A @c char* buffer to be filled with the External I/O unit. Can be 0 (NULL).
 * @param[in,out] size If @p unit is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOUnit(char * unit, int & size)
{
	const char * functionName = "getAcqIOUnit(char * unit, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_io_unit", 0, unit, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the scale of the External IO data.
 *
 * Defines an array of doubles where each element corresponds to an External I/O step.
 *
 * @param[out] scale - An array of doubles that will be filled with the External I/O scale. Can be 0 (NULL).
 * @param[in,out] size If @p scale is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOScale(double * scale, int & size)
{
	const char * functionName = "getAcqIOScale(double * scale, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_io_scale", 0, scale, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets data from the available port specified by @p index parameter in the External IO interface.
 *
 * The size of the data is acq_io_size.
 *
 * Defines an array of doubles where each element corresponds to an External I/O step.
 * @param[in] index - The index of the port to be queried. Must be between 0 and @c acq_io_ports - 1.
 * @param[out] pSpectrum - An array of doubles that will be filled with the data acquired from External I/O port @c index. Can be 0 (NULL).
 * @param[in,out] size - If @p pSpectrum is non-null, this parameter is assumed to contain the maximum number of
 * 				elements in the buffer. After completion, @p size is always modified to contain the length of the
 * 				resulting array.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, or
 * 			if @p index is out-of-bounds, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOSpectrum(int index, double * pSpectrum, int & size)
{
	const char * functionName = "getAcqIOSpectrum(int index, double * pSpectrum, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_io_spectrum", index, pSpectrum, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets a matrix of all data from all available ports in the External IO interface.
 * The size of the resulting vector is acq_io_ports * acq_io_size.
 *
 * @param[out] pData - An array of doubles that will be filled with the spectra acquired from the External I/O ports. Can be 0 (NULL).
 * @param[in,out] size - If @p pData is non-null, this parameter is assumed to contain the maximum number of
 * 				elements in the buffer. After completion, @p size is always modified to contain the length of the
 * 				resulting array.
 * @return asynError if External I/O is not available or if no acquisition has been initialized, or
 * 			if @p index is out-of-bounds, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOData(double * pData, int & size)
{
	const char * functionName = "getAcqIOData(double * pData, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_io_data", 0, pData, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * @brief gets the name of the IO port indicated by the @p index parameter.
 *
 * @param[in] index The index of the port that will be queried. Must be between 0 and @c acq_io_ports-1.
 * @param[out] pName - A @c char* buffer to be filled with the name of the External I/O port specified by @c index. Can be 0 (NULL).
 * @param[in,out] size If @p name is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 * @return asynError if External I/O is not available or if no acquisition has been initialized,
 * 			or if @p index is out-of-bounds, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getAcqIOPortName(int index, char * pName, int & size)
{
	const char * functionName = "getAcqIOPortName(int index, char * name, int & size)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getAcquiredData("acq_io_port_name", index, pName, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}



