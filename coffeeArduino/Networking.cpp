#include "Networking.h"

#define WIFI_CONNECT_RETRY_THR 100

//######################################################################################################################
bool Networking::isWifiConnected()
{
	return WiFi.status() == WL_CONNECTED;
}

//######################################################################################################################
bool Networking::connectWifi()
{
 // Setting up wireless connection:
	if( _debug_level ) Serial.println("Connecting WiFi network:");
	if( _debug_level ) Serial.print("   `" + _wifi_ssid + "` ... ");

	WiFiMode mode = WiFi.getMode();
	if(mode != WIFI_STA && mode != WIFI_AP_STA)
	{
		if( _debug_level ) Serial.println("FAILED");
		if( _debug_level ) Serial.println("WiFi mode: " + getWifiMode(mode) );

		return false;
	}

	if (WiFi.status() != WL_CONNECTED || WiFi.SSID() != _wifi_ssid)
	{
    WiFi.begin(_wifi_ssid.c_str(), _wifi_password.c_str());
	}
/*
	WiFi.persistent(false);
	WiFi.mode(WIFI_OFF);   // this is a temporary line, to be removed after SDK update to 1.5.4
	WiFi.mode(mode);
	WiFi.begin(_wifi_ssid.c_str(), _wifi_password.c_str());
*/

	int countdown = WIFI_CONNECT_RETRY_THR;
	while (WiFi.status() != WL_CONNECTED && (--countdown) > 0)
	{
		delay(200);
		if( _debug_level ) Serial.print(".");
	}

	if(WiFi.status() != WL_CONNECTED)
	{
		if( _debug_level ) Serial.println("FAILED");
		if( _debug_level ) Serial.println("WiFi mode: " + getWifiMode() );

		return false;
	}

	if( _debug_level ) Serial.println("OK");
	if( _debug_level ) Serial.print("   local IP: ");
	if( _debug_level ) Serial.println(WiFi.localIP());

	return true;
}

//######################################################################################################################
bool Networking::scanWifi(String & networks)
{
	networks = "";

// WiFi.scanNetworks will return the number of networks found
	if( _debug_level ) Serial.print("Scan WiFi network ... ");
	int n = WiFi.scanNetworks();
	if (n == 0)
	{
		if( _debug_level ) Serial.println("no networks found");
	}else{
		if( _debug_level ) Serial.println("(" + String(n) + ") OK");

		for(int i = 0; i < n; ++i)
		{
	// Print SSID and RSSI for each network found
		if( _debug_level ) Serial.print("   ");
		if( _debug_level ) Serial.print(i + 1);
		if( _debug_level ) Serial.print(": ");
		if( _debug_level ) Serial.print(WiFi.SSID(i));
		if( _debug_level ) Serial.print(" (");
		if( _debug_level ) Serial.print(WiFi.RSSI(i));
		if( _debug_level ) Serial.print(") ");

		String enc = getWifiEncryption(WiFi.encryptionType(i));
		if( _debug_level ) Serial.println(enc + " (" + String(WiFi.encryptionType(i), DEC) + ")");

		networks += String(i == 0 ? "" : ",") + "{\"ssid\":\"" + String(WiFi.SSID(i)) + "\",\"rsii\":\"" +
		String(WiFi.RSSI(i)) + "\",\"channel\":" + String(WiFi.channel(i)) + ",\"encryption\":\"" + enc + "\"}";

		delay(10);
	}

	networks = "[" + networks + "]";
  }
}

//######################################################################################################################
bool Networking::sendHttpRequest(String url, String contents, String &out, uint16_t * counter, uint16_t * ref_counter)
{
	if(WiFi.status() != WL_CONNECTED )
	{
		if( _debug_level ) Serial.println("Not connected to WiFi");
		return false;
	}

	_http_requests_count ++;
  
	if( _debug_level ) Serial.print("Free heap (" + String(_http_requests_count) + "):");
	if( _debug_level ) Serial.println(ESP.getFreeHeap(),DEC);

	if( _debug_level ) Serial.print("Connecting to `" + _host + ":" + String(_port) + "` ... ");

	if(! _client.connected())
	{
		if (! _client.connect(_host.c_str(), _port))
		{
			if( _debug_level ) Serial.println("FAILED");
			return false;
		}
	}

	int countdown = WIFI_CONNECT_RETRY_THR;
	while (! _client.connected() && (--countdown) > 0)
	{
		delay(200);
		if( _debug_level ) Serial.print(".");
	}
	if( _debug_level ) Serial.println("OK");
  
	if( _debug_level ) Serial.print("Request `" + String(url) + "` ... ");

	_client.println("POST " + url + " HTTP/1.1");
	_client.println("Host: " + _host);
	_client.println("User-Agent: ESP8266/1.0");
	_client.println("Connection: close");
	_client.println("Connection-ID: " + String(_http_requests_count));
	_client.println("Local-ID: " + String(_local_id));
	_client.println("Content-Type: application/x-www-form-urlencoded;");

    _client.println("x-ESP8266-STA-MAC: " + WiFi.macAddress());
    _client.println("x-ESP8266-AP-MAC: " + WiFi.softAPmacAddress());
    _client.println("x-ESP8266-free-space: " + String(ESP.getFreeSketchSpace()));
    _client.println("x-ESP8266-sketch-size: " + String(ESP.getSketchSize()));
    _client.println("x-ESP8266-chip-size: " + String(ESP.getFlashChipRealSize()));
    _client.println("x-ESP8266-sdk-version: " + String(ESP.getSdkVersion()));
    if(counter != NULL) _client.println("x-ESP8266-counter: " + String( *counter ));
    if(ref_counter != NULL) _client.println("x-ESP8266-ref-counter: " + String( *ref_counter ));
	
	_client.print("Content-Length: ");
	_client.println(contents.length());
	_client.println();
	_client.println(contents);

	int wait = WIFI_CONNECT_RETRY_THR;
	while(! _client.available() && --wait) delay(50);
  
	bool header = true;
	out = "";
  
	while(_client.available())
	{
		String line = _client.readStringUntil('\r');
		if(! header)
		{
			out += line;
			continue;
		}
    
		if(line == "\n")
		{
			line = _client.readStringUntil('\n');
			if(! line.length()) out += line;

			header = false;
		}
	}
  
	_client.stop();
	countdown = WIFI_CONNECT_RETRY_THR;
	while (_client.connected() && (--countdown) > 0) delay(50);

	if( _debug_level ) Serial.println("OK");
  
	return true;
}

//######################################################################################################################
bool Networking::stopWifiAp()
{
	WiFi.mode(WIFI_STA);

	if( _debug_level )
	{
		Serial.print("WiFi mode: ");
		Serial.println( getWifiMode() );
	}

	return false;
}

//######################################################################################################################
bool Networking::startWifiAp()
{
	WiFi.mode(WIFI_AP_STA);

	if( _debug_level )
	{
		Serial.print("WiFi mode: ");
		Serial.println( getWifiMode() );
	}

	return false;
}

//######################################################################################################################
String Networking::getWifiMode() { return getWifiMode(WiFi.getMode()); }
String Networking::getWifiMode(WiFiMode mode)
{
	switch(mode)
	{
		case WIFI_AP: return "WIFI_AP";
		case WIFI_STA: return "WIFI_STA";
		case WIFI_AP_STA: return "WIFI_AP_STA";
		default: return "Unknown";
	}
}

//######################################################################################################################
String Networking::getWifiStatus() { return getWifiStatus(WiFi.status()); }
String Networking::getWifiStatus(unsigned int status)
{
	switch(status)
	{
		case WL_CONNECTED: return "WL_CONNECTED";
		case WL_NO_SHIELD: return "WL_NO_SHIELD";
		case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
		case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
		case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
		case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
		case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
		case WL_DISCONNECTED: return "WL_DISCONNECTED";
		default: return "Unknown";
	}
}

//######################################################################################################################
String Networking::getWifiEncryption(unsigned int encryption)
{
	switch(encryption)
	{
		case ENC_TYPE_TKIP: return "TKIP (WPA)";
		case ENC_TYPE_WEP: return "WEP";
		case ENC_TYPE_CCMP: return "CCMP (WPA)";
		case ENC_TYPE_NONE: return "NONE";
		case ENC_TYPE_AUTO: return "AUTO";
		default: return "Unknown";
	}
}

//######################################################################################################################
String Networking::URLEncode(const char* msg)
{
	const char *hex = "0123456789abcdef";
	String encodedMsg = "";

	while (*msg!='\0')
	{
		if( ('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9') )
		{
			encodedMsg += *msg;
		} else {
			encodedMsg += '%';
			encodedMsg += hex[*msg >> 4];
			encodedMsg += hex[*msg & 15];
		}
		msg++;
	}

	return encodedMsg;
}
