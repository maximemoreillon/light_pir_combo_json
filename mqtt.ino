void MQTT_setup(){
  Serial.println(F("[MQTT] MQTT setup"));
  MQTT_client.setServer(MQTT_BROKER_ADDRESS, MQTT_PORT);
  MQTT_client.setCallback(MQTT_message_callback);
}

void MQTT_publish_light_state(){
  // Send the relay state
  StaticJsonDocument<200> outbound_JSON_message;
  outbound_JSON_message["state"] = light_state;
  char JSONmessageBuffer[100];
  serializeJson(outbound_JSON_message, JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("[MQTT] publish of relay state");
  MQTT_client.publish(MQTT_LIGHT_STATUS_TOPIC, JSONmessageBuffer, MQTT_RETAIN);
}

void MQTT_connection_manager(){

  static int MQTT_connected = -1; // 1: connected, 0: disconnected, -1: unknown
  static long last_MQTT_connection_attempt;
  
  if(!MQTT_client.connected()) {
    if(MQTT_connected != 0){
      // MQTT connection status changed to "disconnected"
      MQTT_connected = 0;

      Serial.println(F("[MQTT] Disconnected"));
    }
        
    if(millis() - last_MQTT_connection_attempt > 1000){
      last_MQTT_connection_attempt = millis();
      MQTT_client.connect(HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD, MQTT_LIGHT_STATUS_TOPIC, MQTT_QOS, MQTT_RETAIN, MQTT_LAST_WILL);
    }
        
  } else {
    if(MQTT_connected != 1){
      // MQTT connection status changed to "connected"
      MQTT_connected = 1;

      Serial.println(F("[MQTT] Connected"));
      Serial.println(F("[MQTT] Subscribing to topics"));
      MQTT_client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
      MQTT_publish_light_state();
    }
  }
}

void MQTT_message_callback(char* topic, byte* payload, unsigned int length) {

  Serial.print(F("[MQTTT] message received on "));
  Serial.print(topic);
  Serial.println(", payload: ");

  // Parsing payload as JSON
  StaticJsonDocument<200> inbound_JSON_message;
  deserializeJson(inbound_JSON_message, payload);

  const char* command_state = inbound_JSON_message["state"];

  if(strcmp(command_state, "ON") == 0 || strcmp(command_state, "on") == 0){   
    Serial.println(F("[IO] Turning light ON"));
    digitalWrite(LIGHT_PIN, HIGH);
    light_state = "ON";
  }
  else if(strcmp(command_state, "OFF") == 0 || strcmp(command_state, "off") == 0){
    Serial.println(F("[IO] Turning light OFF"));
    digitalWrite(LIGHT_PIN, LOW);
    light_state = "OFF";
  }
  else if(strcmp(command_state, "TOGGLE") == 0 || strcmp(command_state, "toggle") == 0){
    Serial.println(F("[IO] Toggling light state"));
    
    if(strcmp(light_state,"OFF") == 0){
      Serial.println(F("[IO] Turning light ON"));
      digitalWrite(LIGHT_PIN, HIGH);
      light_state = "ON";
    }
    else {
      Serial.println(F("[IO] Turning light OFF"));
      digitalWrite(LIGHT_PIN, LOW);
      light_state = "OFF";
    }
  }

  // Generating the response
  StaticJsonDocument<200> outbound_JSON_message;
  outbound_JSON_message["state"] = light_state;

  char JSONmessageBuffer[100];
  serializeJson(outbound_JSON_message, JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
  Serial.println(F("[MQTT] publish of light state"));
  MQTT_client.publish(MQTT_LIGHT_STATUS_TOPIC, JSONmessageBuffer, MQTT_RETAIN);
}
