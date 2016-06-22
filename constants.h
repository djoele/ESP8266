//Meterkast ESP8266
//Zie D:\data\djoele\Documents\ESP8266-1.png
//pin 2: grijs = energie
//pin 8: groen = gas
//pin 9: blauw = water
const int pinEnergie = 5;
const int pinGas = 14;
const int pinWater = 12;

const String host = "framboos-domotica.tk";
const int httpsPort = 443;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4096;
char buf[EEPROM_MAX_ADDR];
char buf2[2000];
String stack;
String reset;

String version;
char md5value[32] = "empty";
String sha;
char shavalue[60] = "";
const char * fingerprint;

const char* updateurl = "https://framboos-domotica.tk:443/update_esp8266";
const char* trigger_stack = "https://framboos-domotica.tk:443/trigger_stack";
const char* ssid = "VGV7519B5207C";
const char* password = "Wachtwoord";
const char* www_username = "djoele";
const char* www_password = "@jP6Aq03";

char uname[] = "djoele:@jP6Aq03";
char unameenc[40];

const char* updateString = "/json.htm?type=command&param=udevice&idx=";
const char* updateCounter = "&svalue=";
const char* updateElectricityOrText = "&nvalue=0&svalue=";

//Energie
int counter = 0;
const int ID = 11;
const char* type = "Energie";

const int ID4 = 14;
const char* type4 = "Huidig energieverbruik";

//Water
volatile int counter1 = 0;
volatile int flips = 0;
const int ID1 = 12;
const char* type1 = "Water";
int waterpuls = 0;

//Gas
int counter2 = 0;
const int ID2 = 13;
const char* type2 = "Gas";
volatile int gaspuls = 0;

//Heap
const int ID5 = 28;
const uint32_t heap = ESP.getFreeHeap();

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

unsigned long startTimeWifi = millis();
#ifdef DEBUG
WiFiServer telnetServer(23);
WiFiClient serverClient;
#endif
ESP8266WebServer server(80);
IPAddress ip;
String ipadres;
