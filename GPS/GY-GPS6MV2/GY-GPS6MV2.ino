#include <TinyGPS++.h>
#include <WiFi.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial ss(1);

const char* ssid = "Donya";
const char* password = "1987donya";

float latitude, longitude;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str;
int pm;
WiFiServer server(80);

void setup() {
    Serial.begin(115200);
    ss.begin(9600, SERIAL_8N1, 16, 17);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.localIP());
}

void loop() {
    while (ss.available() > 0) {
        char c = ss.read();
        gps.encode(c);
        Serial.print(c); // چاپ داده‌های خام GPS برای دیباگ

        if (gps.location.isUpdated()) {
            latitude = gps.location.lat();
            lat_str = String(latitude, 6);
            longitude = gps.location.lng();
            lng_str = String(longitude, 6);
            Serial.print("Latitude: "); Serial.println(lat_str);
            Serial.print("Longitude: "); Serial.println(lng_str);
        }

        if (gps.date.isUpdated()) {
            date_str = "";
            date = gps.date.day();
            month = gps.date.month();
            year = gps.date.year();
            if (date < 10) date_str += '0';
            date_str += String(date);
            date_str += "/";
            if (month < 10) date_str += '0';
            date_str += String(month);
            date_str += "/";
            date_str += String(year);
            Serial.print("Date: "); Serial.println(date_str);
        }

        if (gps.time.isUpdated()) {
            time_str = "";
            hour = gps.time.hour();
            minute = gps.time.minute();
            second = gps.time.second();
            minute = (minute + 0);
            if (minute > 59) {
                minute = minute - 60;
                hour = hour + 1;
            }
            hour = (hour + 9) % 24;
            pm = hour >= 12 ? 1 : 0;
            hour = hour % 12;
            if (hour == 0) hour = 12;
            if (hour < 10) time_str += '0';
            time_str += String(hour);
            time_str += ":";
            if (minute < 10) time_str += '0';
            time_str += String(minute);
            time_str += ":";
            if (second < 10) time_str += '0';
            time_str += String(second);
            time_str += pm ? " PM" : " AM";
            Serial.print("Time: "); Serial.println(time_str);
        }
    }

    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += "<!DOCTYPE html><html><head><title>GPS DATA</title><style>";
    s += "table, th, td {border: 1px solid black; border-collapse: collapse;}";
    s += "th, td {padding: 10px; text-align: center;}";
    s += "</style></head><body>";
    s += "<h1 style=\"font-size:300%; text-align:center;\">GPS DATA</h1>";
    s += "<p style=\"font-size:150%; text-align:center;\"><b>Location Details</b></p>";
    s += "<table style=\"width:50%; margin-left:auto; margin-right:auto;\">";
    s += "<tr><th>Latitude</th><td>" + lat_str + "</td></tr>";
    s += "<tr><th>Longitude</th><td>" + lng_str + "</td></tr>";
    s += "<tr><th>Date</th><td>" + date_str + "</td></tr>";
    s += "<tr><th>Time</th><td>" + time_str + "</td></tr>";
    s += "</table></body></html>";

    client.print(s);
    delay(10000);
}
