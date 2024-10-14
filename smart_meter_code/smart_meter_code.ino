// Include library 
#include <LiquidCrystal_I2C.h>
#include <EmonLib.h>

// Sets the type of LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// Create an instance 
EnergyMonitor emon1;  

// Measuring Current Using ACS712
const int Sensor_Pin = A1; 
int sensitivity = 185; 
int offsetvoltage = 2542; 

// Bytes for smiley
byte smiley[8] = {
  B00000,
  B01010,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

void setup() {
  // Voltage: input pin, calibration, phase_shift
  emon1.voltage(A0, 187, 1.7); 

  // Initialize the LCD
  lcd.init();                       
  // Print a message to the LCD.
  lcd.backlight();
  lcd.createChar(0, smiley);
  
  lcd.setCursor(3, 0);
  lcd.print("Welcome ");
  lcd.setCursor(0, 1); // Use line 1 since the LCD is 16x2
  lcd.write(byte(0));

  delay(2000); // Pause to display welcome message
  lcd.clear();
}

void loop() {
  //************************* Measure Voltage ******************************************
  emon1.calcVI(20, 2000); // Calculate all. No. of half wavelengths (crossings), time-out
  int Voltage = emon1.Vrms;  // Extract Vrms into a variable

  //************************* Measure Current ******************************************
  unsigned int temp = 0;
  float maxpoint = 0;
  for (int i = 0; i < 500; i++) {
    temp = analogRead(Sensor_Pin); 
    if (temp > maxpoint) {
      maxpoint = temp;
    }
  }
  
  float ADCvalue = maxpoint; 
  double eVoltage = (ADCvalue / 1024.0) * 5000; // Gets you mV
  double Current = ((eVoltage - offsetvoltage) / sensitivity);
  double AC_Current = (Current) / (sqrt(2));  // Convert to AC current

  //************************* Measure Power ******************************************
  int Power = Voltage * AC_Current;

  //************************* Measure Energy ******************************************
  long milisec = millis();  // Calculate time in milliseconds 
  long time = milisec / 1000;  // Convert milliseconds to seconds
  
  float Energy = (Power * time) / 3600;  // Convert Watt-sec to Watt-hour
  
  //************************* Display Data ******************************************
  lcd.clear();  // Clear the LCD before displaying new data

  // Line 1: Voltage and Current
  lcd.setCursor(0, 0);  
  lcd.print("V=");
  lcd.print(Voltage);  // Display voltage
  lcd.print("V ");

  lcd.print("I=");
  lcd.print(AC_Current, 2);  // Display current
  lcd.print("A");

  // Line 2: Power and Energy
  lcd.setCursor(0, 1);
  lcd.print("P=");
  lcd.print(Power);  // Display power
  lcd.print("W ");

  lcd.print("E=");
  lcd.print(Energy, 1);  // Display energy in Wh
  lcd.print("Wh");

  delay(200);  // Update every second
}
