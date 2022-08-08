#include "Keyboard.h"

void setup() {
  Serial1.begin(115200);
  Keyboard.begin();
}


// the loop routine runs over and over again forever:
void loop() {
  String myString = Serial1.readString();
  if(myString !=""){
    Keyboard.write(KEY_KP_ENTER);
    delay(2000);
    keyboardString(myString,true);
  }
}

void keyboardString (String str, bool enterEnd){
  for(int i = 0; i<str.length(); i++){
    Keyboard.write(str.charAt(i));
    delay(50);
  }
  if(enterEnd){
    Keyboard.write(KEY_KP_ENTER);
  }
}
