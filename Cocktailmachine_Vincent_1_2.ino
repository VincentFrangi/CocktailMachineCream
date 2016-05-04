
//TODO rampe d accélération
//TODO Meilleur gestion des cocktail
//TODO ajout initialisation
//TODO Allumer éteindre moteur
//TODO Gestion LCD + clavier
//TODO mode manuel
//TODO arrêt d'urgence
//TODO quand on repasse par le 0 refaire une ref pour éviter le décalage


// ---------PIN------------------------
// Motor 1 PIN
#define TURRET_STEP_PIN 24
#define TURRET_DIR_PIN 22
#define TURRET_ENABLE_PIN // not(ENABLE) the motor PIN
#define TURRET_REF_PIN 30 // Init Sensor

// Motor 2 PIN
#define PUSHER_STEP_PIN 28
#define PUSHER_DIR_PIN 26
#define TURRET_ENABLE_PIN // not(ENABLE) the motor PIN
#define PUSHER_REF_PIN 32 // Init Sensor

//------------TURRET SENS----------------
#define CCW LOW // Counter clock wise
#define CW HIGH // Clock wise

// ------Constantes--------------------
const int stepPerRevolution = 200; // Step per revolution
const int steppingFactor = 4; // For one step you have to make 4 pulses
const int bottleGearRatio = 40; // Ratio of the gear for the TURRET
const int numberBottle = 10; // Number of bottle of the turret
const int revolutionOpenBottle = 19;

// -----Globales variables-------------
uint16_t onePosition =  (stepPerRevolution * steppingFactor * bottleGearRatio) / numberBottle; //How many step to do one position
int choiseCocktail = 0; // What cocktail have you choose
int absolutPosition = 0;

// ----- Cocktails --------------------
typedef struct Cocktail Cocktail;
struct Cocktail {
  String name;  // Name of the cockatail
  int number; // Number of ingrediant
  int pos[]; // Array of ingrediant
};

// ------Receipt-----------------------
// Tequilla Sunrise
int Tequilla[] = {1, 2, 1, 9, 2, 9, 1, 2, 2}; // 6cl Tequila 2cl genadine 12 cl Orangensaft

// ------ Liste de cocktail -----------
Cocktail list[10];

//------- Initialisation --------------
void setup() {
  // Serial communication
  Serial.begin(9600);

  // -----init the cocktails -----------
  // TODO create a function
  list[0].name += "Tequilla Sunrise";
  list[0].number = 9;
  for (int i = 0; i < list[0].number; i++) {
    list[0].pos[i] = Tequilla[i];
    //Serial.print(list[0].pos[i]);
  }
  
  //Rotation Bottle PIN
  pinMode(TURRET_STEP_PIN, OUTPUT);
  pinMode(TURRET_DIR_PIN, OUTPUT);

  //Puscher PIN
  pinMode(PUSHER_STEP_PIN, OUTPUT);
  pinMode(PUSHER_DIR_PIN, OUTPUT);

  //Sensor
  pinMode(TURRET_REF_PIN, INPUT_PULLUP);

  pinMode(PUSHER_REF_PIN, INPUT_PULLUP);


}

//----------- Main ------------------
void loop() {
  
  Serial.println("Welcome");
  //Serial.println(list[choiseCocktail].pos[0]);
  
  makeCocktail(&list[choiseCocktail]); // make the choosen cocktail
  
  Serial.println("Return to Start");
  movePos(0);
  while (1); // wait
}

//----------Making a Cocktail-------------
void makeCocktail(Cocktail* choice) {
  
  Serial.println(choice->name);
  Serial.println(choice->pos[0]);
  
  for (int i = 0; i < choice->number; i++) { // Change the ingrediant
    Serial.println();
    Serial.println(i);
    
    if (absolutPosition != choice->pos[i]) { // move to the right position if need
      movePos(choice->pos[i]);
    }
    fillCup(); // fill the cup with alcool
  }
  Serial.println("Cocktail done");
}

// ---------- Turn the Bottles --------
void movePos(int pos) {
  
  Serial.print("Moving position, from: ");
  Serial.print(absolutPosition);
  Serial.print("  to: ");
  Serial.println(pos);

  // Finding what way is the best
  if (absolutPosition <= pos) {
    if ((pos - absolutPosition) < 5) { // Moving CCW
      digitalWrite(TURRET_DIR_PIN, CCW);

      Serial.print("Moving CCW  ");
      Serial.print("Bottle:  ");
      Serial.println((pos - absolutPosition));

      rotate(pos - absolutPosition);
    }
    else { // Moving CW
      digitalWrite(TURRET_DIR_PIN, CW);

      Serial.print("Moving CW  " );
      Serial.print("Bottle:  ");
      Serial.println((10 - (pos - absolutPosition)));

      rotate(10 - (pos - absolutPosition));
    }
  }
  else { //Moving CW
    if ((absolutPosition - pos) < 5) {
      digitalWrite(TURRET_DIR_PIN, CW);

      Serial.print("Moving CW  ");
      Serial.print("Bottle:  ");
      Serial.println((absolutPosition - pos));

      rotate(absolutPosition - pos);
    }
    else { // Moving CCW
      digitalWrite(TURRET_DIR_PIN, CCW);

      Serial.print("Moving CCW  ");
      Serial.print("Bottle:  ");
      Serial.println(10 - (absolutPosition - pos));

      rotate(10 - (absolutPosition - pos));
    }
  }
  absolutPosition = pos;
}
//--------------rotate---------------
void rotate(int pos){
  int j = 1000; // Start speed
  Serial.println("rotate");
  for (int i = 0; i < pos*onePosition; i++) {
        digitalWrite(TURRET_STEP_PIN, HIGH);
        if(j>400){ // acceleration
            j--;
          }
        delayMicroseconds(j);
        digitalWrite(TURRET_STEP_PIN, LOW);
  }
}


//----------Filling the Cup------------
void fillCup() {
  digitalWrite(PUSHER_DIR_PIN, HIGH);
  Serial.println("Open");
  for (int i = 0; i < revolutionOpenBottle * stepPerRevolution * steppingFactor; i++) {
    digitalWrite(PUSHER_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(PUSHER_STEP_PIN, LOW);
  }

  Serial.println("wait");
  delay(2000);

  digitalWrite(PUSHER_DIR_PIN, LOW);
  Serial.println("Close");
  for (int i = 0; i < revolutionOpenBottle * stepPerRevolution * steppingFactor; i++) {
    digitalWrite(PUSHER_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(PUSHER_STEP_PIN, LOW);
  }
}

//---------Initialise--------------
/*void init(){
  Serial.println("Initialisation");
  digitalWrite(PUSHER_DIR_PIN, LOW);
  Serial.println("Down");
  while(digitalRead(PUSHER_REF_PIN)){
      digitalWrite(PUSHER_STEP_PIN, HIGH);
      delayMicroseconds(200);
      digitalWrite(PUSHER_STEP_PIN, LOW);
      }

  digitalWrite(TURRET_DIR_PIN, HIGH);
  Serial.println("Moving CW");
  Serial.print("To Init  ");
  while(digitalRead(TURRET_REF_PIN)){
     for(int i = 0; i < 10; i++){
          digitalWrite(TURRET_STEP_PIN, HIGH);
          delayMicroseconds(400);
          digitalWrite(TURRET_STEP_PIN, LOW);
       }
    }
  absolutPosition = 0;
  Serial.println("Init done");
  }
*/

