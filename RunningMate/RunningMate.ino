#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define MAX_SIZE 82
#define RXD2 16
#define TXD2 17
#define WIDTH 128
#define HEIGHT 64

double coordinates[2] = {0, 0};
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);

void setup()  {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  initDisplay();

}

void loop() {
  char input[MAX_SIZE+1];
  const char subStr[7] = "$GPRMC";

  byte size = Serial2.readBytesUntil('\n', input, MAX_SIZE);
  input[size]=0;

  if(strstr(input, subStr))  {
    getCoords(input);
    if(coordinates[0] != 0 && coordinates[1] != 0)  displayCoords();
  }
}

void displayCoords(void) {
  display.println(coordinates[0], 5);
  display.println(coordinates[1], 5);

  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 10);
}

void initDisplay(void)  {
  while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))  {
    Serial.println(F("Waiting..."));
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
}

void getCoords(char in[MAX_SIZE+1])  {
  double latitude, longitude;
  char latBuf[11] = {0};
  char lngBuf[12] = {0};
  char ns, ew;
  char *tokens, *tempInPtr, *inPtr = strdup(in);

  for (int i = 0; i < 7; i++){
    tokens = strsep(&inPtr, ",");
    switch (i)  {
      case 3:
      strcpy(latBuf, tokens);
      break;
      case 4:
      ns = *tokens;
      break;
      case 5:
      strcpy(lngBuf, tokens);
      break;
      case 6:
      ew = *tokens;
      break;
      default:
      break;
    }
  }

  free(tempInPtr);

  if (latBuf[0] == '\0' || lngBuf[0] == '\0') {
    return;
  }

  latitude = strtod(latBuf, NULL);
  longitude = strtod(lngBuf, NULL); 

  latitude = convertCoords(latitude);
  longitude = convertCoords(longitude);

  if (ns == 'S')  {
    latitude *= -1.0;
  }
  if (ew == 'W')  {
    longitude *= -1.0;
  }
  
  coordinates[0] = latitude;
  coordinates[1] = longitude;
}

double convertCoords(double latLng)  {
  double minutesCorrected = ((int)(latLng*100000) % 10000000) / 6000000.0;
  int degrees = latLng / 100;
  
  return degrees + minutesCorrected;
}
