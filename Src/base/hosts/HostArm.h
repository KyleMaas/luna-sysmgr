/**
 * @file
 * 
 * Common functionality for all ARM-based devices.
 *
 * @author Hewlett-Packard Development Company, L.P.
 * @author tyrok1
 *
 * @section LICENSE
 *
 *      Copyright (c) 2008-2012 Hewlett-Packard Development Company, L.P.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */




#ifndef HOSTARM_H
#define HOSTARM_H

#include <nyx/nyx_client.h>

#include "Common.h"

#include "HostBase.h"
#include "Event.h"
#include "CustomEvents.h"
#include "NyxSensorConnector.h"
#include "NyxInputControl.h"
#include "NyxLedControl.h"

#if defined(TARGET_DEVICE)
#include "HidLib.h"
#endif
#include "lunaservice.h"

#include <qsocketnotifier.h>
#include <QObject>

#define KEYBOARD_TOKEN		"com.palm.properties.KEYoBRD"
#define KEYBOARD_QWERTY		"z"
#define KEYBOARD_AZERTY		"w"
#define KEYBOARD_QWERTZ		"y"

#define HIDD_LS_KEYPAD_URI		"palm://com.palm.hidd/HidKeypad/"
#define HIDD_RINGER_URI			HIDD_LS_KEYPAD_URI"RingerState"
#define HIDD_SLIDER_URI			HIDD_LS_KEYPAD_URI"SliderState"
#define HIDD_HEADSET_URI		HIDD_LS_KEYPAD_URI"HeadsetState"
#define HIDD_HEADSET_MIC_URI		HIDD_LS_KEYPAD_URI"HeadsetMicState"
#define HIDD_GET_STATE			"{\"mode\":\"get\"}"

#define HIDD_LS_ACCELEROMETER_URI	"palm://com.palm.hidd/HidAccelerometer/"
#define HIDD_ACCELEROMETER_RANGE_URI	HIDD_LS_ACCELEROMETER_URI"Range"

#define HIDD_ACCELEROMETER_MODE			HIDD_LS_ACCELEROMETER_URI"Mode"
#define HIDD_ACCELEROMETER_INTERRUPT_MODE	HIDD_LS_ACCELEROMETER_URI"InterruptMode"
#define HIDD_ACCELEROMETER_POLL_INTERVAL	HIDD_LS_ACCELEROMETER_URI"PollInterval"
#define HIDD_ACCELEROMETER_TILT_TIMER		HIDD_LS_ACCELEROMETER_URI"TiltTimer"
#define HIDD_ACCELEROMETER_SET_DEFAULT_TILT_TIMER	"{\"mode\":\"set\",\"value\":6}"
#define HIDD_ACCELEROMETER_SET_DEFAULT_INTERVAL		"{\"mode\":\"set\",\"value\":2000}"
#define HIDD_ACCELEROMETER_SET_DEFAULT_MODE		"{\"mode\":\"set\",\"value\":\"all\"}"
#define HIDD_ACCELEROMETER_SET_POLL			"{\"mode\":\"set\",\"value\":\"poll\"}"

// Same as default kernel values
#define REPEAT_DELAY	250
#define REPEAT_PERIOD	33

#if 0
// TODO: These don't get included from <linux/input.h> because the OE build
// picks up the include files from the codesourcery toolchain, not our modified headers
#define SW_RINGER		0x5
#define SW_SLIDER		0x6
#define SW_OPTICAL		0x7
#define ABS_ORIENTATION		0x30

// TODO: these should come from hidd headers
#define EV_GESTURE      0x06
#define EV_FINGERID	0x07
#endif

typedef enum
{
    BACK = 0,
    MENU,
    QUICK_LAUNCH,
    LAUNCHER,
    NEXT,
    PREV,
    FLICK,
    DOWN,
    HOME,
    HOME_LEFT,
    HOME_RIGHT,
    NUM_GESTURES
} GestureType_t;

/**
 * Base class for all ARM-based host device classes to derive from
 */
class HostArm : public HostBase, public NYXConnectorObserver
{
    Q_OBJECT
public:
	/**
	 * Constructs an ARM-based host device
	 * 
	 * Initializes common ARM hardware.
	 */
	HostArm();
	
	/**
	 * Shuts down ARM-specific hardware
	 */
	virtual ~HostArm();
	
	//Documented in parent
	virtual void init(int w, int h);
	virtual void show();
	
	//Documented in parent
	virtual int getNumberOfSwitches() const;
#if defined(TARGET_DEVICE)
	
	/**
	 * Asks the HIDD service for the state of each of the switches
	 * 
	 * Posts KeyPress/KeyRelease events to the
	 * event queue of the active window for each
	 * of the hardware switches of this device.
	 * 
	 * Possible keys include:
	 * - Qt::Key_unknown
	 * - Qt::Key_Ringer
	 * - Qt::Key_Slider
	 * - Qt::Key_HeadsetMic
	 * - Qt::Key_Headset
	 */
	virtual void getInitialSwitchStates(void);
#endif
	int readHidEvents(int fd, struct input_event* eventBuf, int bufSize);
	
	/**
	 * @copybrief HostBase::hardwareName()
	 * 
	 * Since this class is a generic base for
	 * ARM-based devices, this always returns
	 * "ARM Device".
	 * 
	 * @return				Returns the string "ARM Device".
	 */
	virtual const char* hardwareName() const;
	
	//Documented in parent
	virtual InputControl* getInputControlALS();
	
	//Documented in parent
	virtual InputControl* getInputControlBluetoothInputDetect();
	
	//Documented in parent
	virtual InputControl* getInputControlProximity();
	
	//Documented in parent
	virtual InputControl* getInputControlTouchpanel();
	
	//Documented in parent
	virtual InputControl* getInputControlKeys();

	virtual LedControl* getLedControlKeypadAndDisplay();
	
	/**
	 * Function enables/disables the orientation sensor
	 * 
	 * @param	enable			true to enable the orientation sensor, false to disable it.
	 */
	virtual void OrientationSensorOn(bool enable);

	virtual void setBluetoothKeyboardActive(bool active);
	
	//Documented in parent
	virtual bool bluetoothKeyboardActive() const;

protected:

	QSocketNotifier* m_nyxLightNotifier;
	QSocketNotifier* m_nyxProxNotifier;

	virtual void wakeUpLcd();

	virtual int screenX(int rawX, Event::Type type) { return rawX; }
	virtual int screenY(int rawY, Event::Type type) { return rawY; }

	virtual void setCentralWidget(QWidget* view);

#if defined(TARGET_DEVICE)
	HidHardwareRevision_t m_hwRev;
	HidHardwarePlatform_t m_hwPlatform;
#endif
	int m_fb0Fd;
	int m_fb1Fd;
	void* m_fb0Buffer;
	int m_fb0NumBuffers;
	void* m_fb1Buffer;
	int m_fb1NumBuffers;

	LSHandle* m_service;

	InputControl* m_nyxInputControlALS;
	InputControl* m_nyxInputControlBluetoothInputDetect;
	InputControl* m_nyxInputControlProx;
	InputControl* m_nyxInputControlKeys;
	InputControl* m_nyxInputControlTouchpanel;
	LedControl* m_nyxLedControlKeypadAndDisplay;

    bool m_bluetoothKeyboardActive;

    NYXOrientationSensorConnector* m_OrientationSensor;

protected:
	void setupInput(void);
	void shutdownInput(void);

	void startService(void);
	void stopService(void);
	
	/**
	 * Disable screen blanking and puts the screen into graphics mode
	 * 
	 * Enabled screen blanking to restore the screen
	 * from sleep, then immediately disables it.
	 * 
	 * After that, it puts the screen into grahics
	 * mode to get it ready to draw to (and hide
	 * the cursor).
	 * 
	 * When deciphering the implementation of this,
	 * see the manpage for console_ioctl,
	 * specifically TIOCLINUX subcodes 0 and 10.
	 */
	void disableScreenBlanking();
	
	//Documented in parent
	virtual void flip();

	virtual QImage takeScreenShot() const;
	virtual QImage takeAppDirectRenderingScreenShot() const;
	virtual void setAppDirectRenderingLayerEnabled(bool enable);
	virtual void setRenderingLayerEnabled(bool enable);
	
	/**
	 * Given a return value from an IPC call with a "value" element, returns the integer value of it
	 * 
	 * When an IPC call passes back a JSON structure
	 * in a format such as:
	 * 
	 * <code>
	 * {
	 *   value : "127"
	 * }
	 * </code>
	 * 
	 * It returns the integer value of the "value"
	 * element.
	 * 
	 * @param	msg			Return value message to parse from IPC call.
	 * @param	value			Where to store the integer version of the value.
	 * @return				true if parsing was successful, false if parsing failed.
	 */
	static bool getMsgValueInt(LSMessage* msg, int& value);
#if defined(TARGET_DEVICE)
	static bool switchStateCallback(LSHandle* handle, LSMessage* msg, void* data);
#endif
    /**
     * Function gets called whenever there is some data
     * available from NYX.
     *
     * @param[in]   - aSensorType - Sensor which has got some data to report
     */
    virtual void NYXDataAvailable (NYXConnectorBase::Sensor aSensorType);

protected Q_SLOTS:
	void readALSData();
	void readProxData();
};

#endif /* HOSTARM_H */
