void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("NAVIGATION SYSTEM READY");
  }
  
  void loop() {
    if (Serial.available()) {
      String qr_data = Serial.readStringUntil('\n');
      qr_data.trim();
      
      // Обработка перекрёстков
      if (qr_data == "1") {
        Serial.println("Reached intersection 1 → Turning NORTH");
        simulateMovement("N", 2000);
      }
      else if (qr_data == "2") {
        Serial.println("Reached intersection 2 → Turning EAST");
        simulateMovement("E", 1500); 
      }
      else if (qr_data == "3") {
        Serial.println("Reached intersection 3 → Turning WEST");
        simulateMovement("W", 1500);
      }
      else if (qr_data == "4") {
        Serial.println("Reached intersection 4 → Turning SOUTH");
        simulateMovement("S", 2000);
      }
      else {
        Serial.print("UNKNOWN QR: ");
        Serial.println(qr_data);
      }
    }
  }
  
  void simulateMovement(String dir, int duration) {
    Serial.print("Moving ");
    Serial.print(dir);
    Serial.print(" for ");
    Serial.print(duration);
    Serial.println("ms");
    
    // Эмуляция движения (можно заменить на управление моторами)
    delay(duration); 
    Serial.println("Movement complete");
  }
