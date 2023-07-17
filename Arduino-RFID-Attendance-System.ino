#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9           // Configurable, see typical pin layout above
#define SS_PIN 10          // Configurable, see typical pin layout above
const int LED_PIN = 6;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

const int numOfCards = 2;//the nuber of cards used. this can change as you want
byte cards[numOfCards][4] = {{0xA3, 0x04, 0x9E, 0x14},{0x93, 0xE6, 0xE5, 0x18}}; // array of UIDs of rfid cards
int readCard[numOfCards]; //this array content the details of cards that already detect or not .

//*******************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  pinMode(LED_PIN, OUTPUT);
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  // Serial.println(F("\nReady to Read"));    //shows in serial that it is ready to read
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void readRFID(){
  int j = -1;
  byte card_ID[4];//card UID size 4byte
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {//look for new card
    return;//got to start of loop if there is no card present
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {// Select one of the cards
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];
  }
  
  for (int i = 0; i < numOfCards; i++) {
    if (card_ID[0] == cards[i][0] && card_ID[1] == cards[i][1] && card_ID[2] == cards[i][2] && card_ID[3] == cards[i][3]) {
      j = i;
    }
  }
  
  if(j == -1) {//check the card validity
    Serial.println("Invalid Card.");
  }
  else if (readCard[j] == 1) { //to check if the card already detect
    readCard[j] = 0;
    Serial.print("\nOUT");
    ShowName();
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  else {
    readCard[j] = 1;
    Serial.print("\nIN");
    ShowName();
  }
  delay(1000);
  
}

void ShowName() {
  
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------
  digitalWrite(LED_PIN, HIGH);

  //-------------------------------------------

  // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  // Serial.print(F("\nName: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(LED_PIN, LOW);
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(LED_PIN, LOW);
    return;
  }
  
  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");

  //---------------------------------------- GET LAST NAME

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(LED_PIN, LOW);
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(LED_PIN, LOW);
    return;
  }

  //PRINT LAST NAME
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }
  //----------------------------------------

  delay(500); //change value if you want to read cards faster
  digitalWrite(LED_PIN, LOW);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

//*******************************//
void loop() {
  readRFID();
}