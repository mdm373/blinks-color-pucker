#include "animate.h"
#include "timer.h"

#define STATE_PENDING 0
#define STATE_COLOR_SHOW_PUCK 1
#define STATE_COLOR_COMP_PUCK 2
#define STATE_COLOR_COMP_DISPLAY 3
#define STATE_COUNT 4


typedef void (*stateLoop)();

#define IR_PENDING_PUCK (IR_DATA_VALUE_MAX - 1)
#define IR_RED_COMP (IR_DATA_VALUE_MAX - 2)
#define IR_BLUE_COMP (IR_DATA_VALUE_MAX - 3)
#define IR_GREEN_COMP (IR_DATA_VALUE_MAX - 4)
#define IR_NO_COMP (IR_DATA_VALUE_MAX - 5)

#define IR_COLOR_BASE 1

#define SHOW_DURATION 800


byte _state;
byte _firstCompFace;
byte _compType;
byte _compFace;
byte _compValue;
bool _showSig0;
stateLoop states[STATE_COUNT];


bool isStartOfThreePendingPucks(const byte startFace) {
	bool isStartOfThreePending = true;
	FOREACH_FACE(f) {
		const byte offsetFace = (startFace + f) % FACE_COUNT;
		if(f > 2) {
			isStartOfThreePending = isStartOfThreePending && isValueReceivedOnFaceExpired(offsetFace);
			continue;
		}
		const byte value = isValueReceivedOnFaceExpired(offsetFace) ? 0 : getLastValueReceivedOnFace(offsetFace);
		isStartOfThreePending = isStartOfThreePending && value == IR_PENDING_PUCK;
	}
	return isStartOfThreePending;
}



bool isInShowPuckConfig() {
	_firstCompFace = FACE_COUNT;
	FOREACH_FACE(f) {
		bool areColorCandidates = isStartOfThreePendingPucks(f);
		if(areColorCandidates){
			_firstCompFace = f;
			setValueSentOnFace(IR_RED_COMP, f);
			setValueSentOnFace(IR_GREEN_COMP, (f+1) % FACE_COUNT);
			setValueSentOnFace(IR_BLUE_COMP, (f+2) % FACE_COUNT);
			break;
		}
	}
	return _firstCompFace < FACE_COUNT;
}

bool isInColorCompConfig(){
	_compType = IR_PENDING_PUCK;
	_compFace = FACE_COUNT;
	FOREACH_FACE(f) {
		if(isValueReceivedOnFaceExpired(f)) {
			continue;
		}
		const byte value = getLastValueReceivedOnFace(f);
		if(value > IR_NO_COMP && value < IR_PENDING_PUCK){
			_compType = value;
			_compFace = f;
			_compValue = MAX_BRIGHTNESS_5BIT;
			break;
		}
	}
	return _compFace < FACE_COUNT;
}

void loopStatePending() {
	if(buttonDoubleClicked() && isInShowPuckConfig()){
		_state = STATE_COLOR_SHOW_PUCK;
		return;
	}
	if(isInColorCompConfig()){
		_state = STATE_COLOR_COMP_PUCK;
		return;
	}
	animate::spin(WHITE, 4);
	setValueSentOnAllFaces(IR_PENDING_PUCK);
}

void loopStateColorComp() {
	if(isAlone()) {
		_state = STATE_PENDING;
		return;
	}
	if(buttonLongPressed()) {
		_state = STATE_COLOR_COMP_DISPLAY;
		return;
	}
	byte increment = 0;
	if(buttonSingleClicked()){
		increment = 1;
	}
	if(buttonDoubleClicked()){
		increment = 5;
	}
	if(buttonMultiClicked()){
		increment = 10;
	}
	_compValue = (_compValue + increment) % BRIGHTNESS_LEVELS_5BIT; 
	const byte r = _compType == IR_RED_COMP ? _compValue : 0; 
	const byte g = _compType == IR_GREEN_COMP ? _compValue : 0; 
	const byte b = _compType == IR_BLUE_COMP ? _compValue : 0; 
	setColor(MAKECOLOR_5BIT_RGB(r, g, b));
	setValueSentOnFace(_compValue + IR_COLOR_BASE, _compFace);
}

void loopStateColorShow() {
	buttonLongPressed();
	buttonSingleClicked();
	buttonMultiClicked();
	if(buttonDoubleClicked() && isAlone()) {
		_state = STATE_PENDING;
		return;
	}
	const byte r = getLastValueReceivedOnFace(_firstCompFace) - IR_COLOR_BASE;
	const byte g = getLastValueReceivedOnFace( (_firstCompFace+1) % FACE_COUNT) - IR_COLOR_BASE;
	const byte b = getLastValueReceivedOnFace( (_firstCompFace+2) % FACE_COUNT) - IR_COLOR_BASE;
	setColor(MAKECOLOR_5BIT_RGB(r, g, b));
}

void toggleShowOct() {
	_showSig0 = !_showSig0;
	timer::mark(SHOW_DURATION, toggleShowOct);
}

void loopStateColorDisplayValue() {
	if(isAlone()) {
		_state = STATE_PENDING;
		return;
	}
	if(buttonLongPressed()){
		_state = STATE_COLOR_COMP_PUCK;
		return;
	}
	byte value = _showSig0 ? _compValue / (FACE_COUNT) : _compValue % (FACE_COUNT); //base6
	byte bright = _showSig0 ? MAX_BRIGHTNESS_5BIT : MAX_BRIGHTNESS_5BIT/2;
	FOREACH_FACE(f) {
		if(f < value) {
			setColorOnFace(MAKECOLOR_5BIT_RGB(bright, bright, bright), f);
			continue;
		}
		setColorOnFace(OFF, f);
	}
}

void setup() {
	_showSig0 = false;
	timer::mark(SHOW_DURATION, toggleShowOct);
	_state = STATE_PENDING;
	states[STATE_PENDING] = loopStatePending;
	states[STATE_COLOR_COMP_PUCK] = loopStateColorComp;
	states[STATE_COLOR_SHOW_PUCK] = loopStateColorShow;
	states[STATE_COLOR_COMP_DISPLAY] = loopStateColorDisplayValue;
}

void loop() {
	timer::loop();
	states[_state]();
}
