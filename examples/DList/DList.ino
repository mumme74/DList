/*
*	Licence: public domain
*/

#include <DList.h>
#include <stddef.h>


bool setuped = false;

bool demoIntArray(){
	bool dontUse;
	//Example to demonstrate DList usage
	// internally it uses a doubly linked list 
	// you can create a DList of any type 
	DList<int> intArr; // NOTE that we say which type our list is going to hold, <int> means it stores integers
	DListNode<int> i0(0), i1(1), i2(2), i3(3);// = 0, i1 = 1, i2 = 2, i3 = 3; // we need to create the storage nodes here (its called: on stack)
	
	dontUse = intArr.length() == 0;  // array is empty 
	intArr.push(i2);       // insert last at array, currently at pos[0] as arr is empty
	intArr.push_first(i0); // insert first at array, to pos[0], i2 is now at pos[1]
	intArr.insert(1, i1);  // insert at pos[1], i2 is now at pos[2]
	intArr.push(i3);       // insert last at array (to pos[3])
	dontUse = intArr.length() == 4;  // array now has 4 elements, NOTE that last element is at pos[3] as it starts from pos[0] not pos[1] 
	
	// add to each value
	for(uint8_t i = 0; i < intArr.length(); i++){
		intArr[i]++;  // increment value
	}
	// intArr is now [1, 2, 3, 4] as we have added 1 to each value
	
	// a safer (and faster) loop using iterator style
	for(auto vlu = intArr.first(); intArr.canMove(); vlu = intArr.next()) {
		vlu *= vlu;  // square it by multiply vlu by itself
		// intArr.at() tells the current internal position of the iterator, its a const variable (read only)
	}
	// intArr is now [1, 4, 8, 16]
	
	intArr.remove(2); // removes pos[2] (the one with 8 as value) 
	dontUse = intArr.length() == 3; 
	dontUse = intArr[2] == 16; 
	
	// empty array from the last to first
	while(auto vlu = intArr.pop()){
	 Serial.println(String("value removed from array: ") + vlu); // do something with vlu
	}

	dontUse = intArr.length() == 0; // array is now empty again
	return dontUse;
}






// example with user types
class myInPin {
public:
	uint8_t pinNr;
	const char *description;
	myInPin(): description("") { pinNr = 0; }  // NOTE!  custom type must have a constructor that takes 0 arguments
	myInPin(uint8_t pin, const char name[]) { pinNr = pin;  description = name; }
	bool isHigh() { 
		pinMode(pinNr, INPUT_PULLUP); 
		return digitalRead(pinNr) == 1; 
	}
};

class myOutPin {
	bool _toggle;
public:
	uint8_t pinNr;	
	const char *description;
	myOutPin(){}
	myOutPin(uint8_t pin, const char name[]) {
		pinNr = pin;  description = name; 
		pinMode(pinNr, OUTPUT);
		_toggle = false; 
	}
	void toggle() { 
		digitalWrite(pinNr, _toggle);
		_toggle = !_toggle;
	}
};

// create these here as we must store a persistent _toggle variable
myOutPin greenLed(6, "g");
myOutPin redLed(7, "r");
myOutPin teensyLed(13, "teensy");



bool demoCustomClasses(){
	bool dontUse; // quiet compiler warnings 
	myInPin downButton(15, "down"), upButton(16, "up"), okButton(18, "ok"), lastButton(17, "p");
	
	DList<myInPin> inputs; // create a global list object
	DListNode<myInPin>   inDownNode(downButton),
											 inUpNode(upButton), 
											 inOkNode(okButton),
											 inLastNode(lastButton);// create listnodes to hold our myIoPins (needed as all storage must be on stack)
	
	inputs.push(inDownNode);
	inputs.push(inUpNode);
	inputs.push(inOkNode);
	inputs.push(inLastNode);
	
	dontUse = inputs.length() == 3; // we have set 3 myInPin classes
	

  DList<myOutPin*> outputs;
	DListNode<myOutPin*> greenLedNode(&greenLed),
                      redLedNode(&redLed),
                      teensyLedNode(&teensyLed); // create myOutPins directly when creating the listnodes

	outputs.push(greenLedNode);
	outputs.push(redLedNode);
	outputs.push(teensyLedNode);
	dontUse = outputs.length() == 3; // we have set 3 output classes
	

	// iterate using iteration methods ascending order
	for(auto button = inputs.first(); inputs.canMove() == true; button = inputs.next()){
		if (button.isHigh())
			Serial.print(String("You pressed: ") + button.description + " \n");
	}

	// iterate in descending order
	for(auto led = outputs.last(); outputs.canMove(); led = outputs.previous()){
		if (okButton.isHigh()){
			Serial.print(String("Toggle led") +led->description + '\n');
			led->toggle();
		}
	}
	
	return dontUse;
}

void setup(){
	Serial.begin(115200); // 9600);
	greenLed.toggle();
}


void loop(){
	demoIntArray();
	demoCustomClasses();
	delay(100);
}