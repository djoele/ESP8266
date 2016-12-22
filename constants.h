//Meterkast ESP8266
//Zie D:\data\djoele\Documents\ESP8266-1.png
//pin 8: blauw = water
const int pinWater = 14;


const String host = "framboos-master.nl";
const int httpsPort = 443;

char buf[2000];
char buf2[2000];
int error_sent = 0;

String version;
char md5value[32] = "empty";
String sha;
char shavalue[60] = "";
const char * fingerprint;

const char* updateurl = "https://framboos-master.nl:443/update_esp8266";
const char* trigger_stack = "https://framboos-master.nl:443/trigger_stack";
const char* ssid = "VGV7519B5207C";
const char* password = "YmgH334fLyNB";
const char* www_username = "djoele";
const char* www_password = "@jP6Aq03";

char uname[] = "djoele:@jP6Aq03";
char unameenc[40];

const char* updateString = "/json.htm?type=command&param=udevice&idx=";
const char* updateCounter = "&svalue=";

//Water
volatile int counter = 0;
const int ID1 = 12;
const char* type1 = "Water";
int waterpuls = 0;
time_t trigger_begin = now();
time_t trigger_nu;
time_t tijdsduur;

//Water variabelen
int waarde;
int waardenu;

unsigned long startTimeWifi = millis();
#ifdef DEBUG
WiFiServer telnetServer(23);
WiFiClient serverClient;
#endif
ESP8266WebServer server(80);
IPAddress ip;
String ipadres;
