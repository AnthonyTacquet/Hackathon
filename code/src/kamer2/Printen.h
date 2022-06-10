const char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
LiquidCrystal_I2C lcd(0x27, 20, 4);
unsigned long startTijd = 0;
unsigned long huidigeTijd = 0;
const int periodeLang = 500;
const int periodeKort = 200;
int teller = 0;
int tijd = 250;
int stap = 0;
boolean wacht = false;

void pulse() {
  while (tijd > 50) {
    if (timerSos.fire() && stap == 0) {
      digitalWrite(blauw, LOW);
      stap = 1;
      timerSos2.begin(tijd);
    }
    if (timerSos2.fire() && stap == 1) {
      digitalWrite(blauw, HIGH);
      stap = 0;
      tijd -= 25;
      timerSos.begin(tijd);
    }
  }
  stap = 2;
}

void glitch() {
  Serial.println(stap);
  if (timerSos.fire() && stap == 2) {
    digitalWrite(groen, HIGH);
    stap++;
    hint.begin(200);
  }
  if (hint.fire() && stap == 3) {
    digitalWrite(groen, LOW);
    stap++;
    timerSos2.begin(400);
  }
  if (timerSos2.fire() && stap == 4) {
    digitalWrite(groen, HIGH);
    stap++;
    timerSos.begin(200);
  }
  if (timerSos.fire() && stap == 5) {
    digitalWrite(groen, LOW);
    stap = 2;
    timerSos.begin(3000);
  }
}

void printe()
{
  digitalWrite(groen, HIGH);
  digitalWrite(rood, HIGH);
  lcd.setCursor(teller, 0);
  Serial.println(analogRead(1));
  lcd.print(chars[map(analogRead(1), 0, 1023, 0, 26)]);
  if (digitalRead(select) == HIGH) {
    teller++;
    woord = woord + chars[map(analogRead(1), 0, 1023, 0, 26)];
  }
  //Wacht tot knop is losgelaten;
  while (digitalRead(select) == HIGH) {
  }
  if (digitalRead(enter) == HIGH) {
    woord = woord + chars[map(analogRead(1), 0, 1023, 0, 26)];
    //Wacht tot knop is losgelaten;
    while (digitalRead(enter) == HIGH) {}
    //Check of het woord correct is
    stap = 0;
    tel = 0;
    Serial.println(woord);
    Serial.println(stap);
    while (woord == programma[randomNr]) {
      Serial.println("In loop");
      digitalWrite(blauw, HIGH);
      digitalWrite(groen, HIGH);
      while (true) {
        if (stap < 2) {
          pulse();
        }
        if (tel < 10 && stap == 2 && timerSos.fire()) {
          digitalWrite(blauw, LOW);
          stap++;
          timerSos2.begin(tijd);
        } else if (timerSos2.fire() && stap == 3 && tel < 10) {
          digitalWrite(blauw, HIGH);
          stap--;
          tel++;
          timerSos.begin(tijd);
        } else if (tel == 10 && stap != 5) {
          stap = 5;
          Serial.println("TESTTINGGGGG");
          digitalWrite(groen, LOW);
          digitalWrite(blauw, HIGH);
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("CORRECT");
          lcd.setCursor(0, 1);
          lcd.print("HAPPY HUNTING :D");
          timerSos.begin(4000);
        }
        else if (stap == 5) {
          if (timerSos.fire()) {
            timerSos.begin(3000);
            stap = 2;
            lcd.clear();
            while (true) {
              glitch();
            }
          }
        }
      }
    }
    //FAIL
    while (woord != programma[randomNr] && tel < 10) {
      Serial.println("Foute code");
      Serial.println(stap);
      Serial.println(tel);
      if (stap != 2) {
        digitalWrite(blauw, HIGH);
        digitalWrite(groen, HIGH);
        digitalWrite(blauw, HIGH);
        Serial.println("pulse");
        while (tel < 10) {
          if (stap < 2) {
          pulse();
        }
        if (tel < 10 && stap == 2 && timerSos.fire()) {
          digitalWrite(blauw, LOW);
          stap++;
          timerSos2.begin(tijd);
        } else if (timerSos2.fire() && stap == 3 && tel < 10) {
          digitalWrite(blauw, HIGH);
          stap--;
          tel++;
          timerSos.begin(tijd);
        }
        }
        digitalWrite(blauw, HIGH);
        digitalWrite(groen, HIGH);
        digitalWrite(rood, LOW);
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("WRONG!");
        stap = 1;
        wrong.begin(2500);
        while (stap != 0) {
          if (wrong.fire() && stap == 1) {
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print("THE GHOST IS");
            lcd.setCursor(1, 1);
            lcd.print("GETTING CLOSER!");
            stap++;
            wrong.update(2000);
          }
          else if (wrong.fire() && stap == 2) {
            lcd.clear();
            teller = 0;
            stap = 0;
            woord = "";
            Serial.println(stap);
          }
        }
      }
    }
  }
}