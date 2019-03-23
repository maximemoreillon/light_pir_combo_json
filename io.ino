void IO_setup(){
  
  Serial.println(F("[IO] IO initialization"));
  
  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // Initial state
  if(strcmp(light_state,"ON") == 0) {
    digitalWrite(LIGHT_PIN, HIGH);
  }
  else {
    digitalWrite(LIGHT_PIN, LOW);
  }
}


void read_PIR(){
  static long last_PIR_reading;
  
  int PIR_reading = digitalRead(PIR_PIN);
  
  if(PIR_reading != last_PIR_reading) {

    // Motion sensor reading changed
    last_PIR_reading = PIR_reading;

    Serial.println(F("[IO] Motion detection state changed"));

    // Prerpare a JSON message
    StaticJsonDocument<200> outbound_JSON_message;

    // Add the motion detection state to the JSON message
    if(PIR_reading == HIGH){
      outbound_JSON_message["state"] = "motion";
      Serial.println(F("[IO] Motion detected"));
    }
    else {
      outbound_JSON_message["state"] = "idle";
      Serial.println(F("[IO] No motion detected"));
    }

    // Serialize JSON into a char array
    char JSONmessageBuffer[100];
    serializeJson(outbound_JSON_message, JSONmessageBuffer, sizeof(JSONmessageBuffer));

    // Send the char array
    Serial.println(F("[MQTT] publish of motion detector state"));
    MQTT_client.publish(MQTT_MOTION_STATUS_TOPIC, JSONmessageBuffer, MQTT_RETAIN);
    
  }
}
