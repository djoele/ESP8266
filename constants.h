const int pinEnergie = 5;
const int pinGas = 0;
const int pinWater = 12;

const char* ssid = "Stonealone";
const char* password = "lievelian56";
const char* host = "localhost";
const int httpPort = 80;
const int httpPort2 = 3000;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4096/2;
char buf[EEPROM_MAX_ADDR];
char buf2[750];

const char * updateurl = "https://framboos-domotica.tk:443/update_esp8266";
String version;
char md5value[1024] = "empty";
const char * fingerprint = "25 20 A9 23 D7 21 A4 1D D2 A2 E1 E4 7E 56 0C BB FA 58 0E E0";

char uname[] = "djoele:@jP6Aq03";
char unameenc[40];

const char* updateString = "/json.htm?type=command&param=udevice&idx=";
const char* updateCounter = "&svalue=";
const char* updateElectricityOrText = "&nvalue=0&svalue=";
const char* updateGas = "&svalue=0&svalue=";

//Energie
int counter = -1;
const int ID = 11;
const char* type = "Energie";

const int ID4 = 14;
const char* type4 = "Huidig energieverbruik";

//Water
volatile int counter1 = -1;
const int ID1 = 12;
const char* type1 = "Water";
int waterpuls = 0;

//Gas
int counter2 = -1;
const int ID2 = 13;
const char* type2 = "Gas";
volatile int gaspuls = 0;

//Error
const int ID3 = 25;
const char* type3 = "Error";

//Huidig verbruik
int huidigverbruik = 0;
time_t pulsetijd = now();
time_t begintijd = now();
time_t tijdsduur = 0;
volatile int energiepuls = 0;

//Water variabelen
int waarde;
int waardenu;
time_t triggertijd = now();
time_t triggernu = now();
time_t tijdsduur2 = 0;

WiFiServer telnetServer(23);
WiFiClient serverClient;
unsigned long startTimeWifi = millis();
