#include <DHT.h>
#include <LiquidCrystal.h>

DHT dht(8, DHT11);
const int res=2, en=3, d4=4, d5=5, d6=6, d7=7;
LiquidCrystal lcd(res, en, d4, d5, d6, d7);
#define pluv A0
#define anemo A1
#define girouette A2
#define buzzer 9
#define suivant 11
#define immobile 10

int etat;
boolean verif;
void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(pluv,INPUT);
  pinMode(anemo,INPUT);
  pinMode(girouette, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(suivant, INPUT_PULLUP);
  pinMode(immobile, INPUT_PULLUP);
  etat=1;
  verif=true;
  delay(2000);
}

int t,h,f,temps1,temps2;
float pluie,vent,degvent;

void loop() {
  lcd.clear();
  temps1=millis();
  agir();
  verifieBout();
  if(verif)
    if(etat==3)
      etat=1;
    else
      etat+=1;
  temps2=0;
}

void agir(){
  switch(etat){
    case 1 :{
        h= dht.readHumidity();
        t= dht.readTemperature();
        lcd.setCursor(0,0);
        lcd.print("Temp : "+String(t)+"C");
        lcd.setCursor(0,1);
        lcd.print("Humi : "+String(h)+"%");
        break;
    }
    case 2 :{
        float hi = dht.computeHeatIndex(f, h);
        lcd.setCursor(0,0);
        f = dht.readTemperature(true);
        lcd.print("Temp r: "+String(dht.convertFtoC(hi))+"C");
        lcd.setCursor(0,1);
        pluie=analogRead(pluv);
        pluie=(pluie*5)/1024;
        lcd.print("Pluie : "+String(pluie)+"mm");
        break;
    }
    case 3 :{
        lcd.setCursor(0,0);
        vent=analogRead(anemo);
        vent=(vent*120)/1024;
        degvent=analogRead(girouette);
        degvent=(degvent*360)/1024;
        lcd.print("V vent:"+String(vent)+"km/h");
        Serial.println(getDirectionVent(359.30));
        Serial.print("deg : ");
        Serial.println(degvent);
        if(vent >= 80)
          digitalWrite(buzzer,HIGH);
        else
          digitalWrite(buzzer,LOW);
        lcd.setCursor(0,1);
        lcd.print("Dir vent :"+getDirectionVent(degvent));
        
        break;
    }
  }
}

void verifieBout(){
  while(temps2<2000){
    temps2=millis()-temps1;
    if(digitalRead(suivant)==LOW){
        temps2=2000;
        if(!verif)
          if(etat==3)
            etat=1;
          else
            etat+=1;
        while(digitalRead(suivant)==LOW)
          delay(50);
    }

    if(digitalRead(immobile)==LOW){
      verif=!verif;
      while(digitalRead(immobile)==LOW)
          delay(50); 
    }
  }
}

String getDirectionVent(float degres){
  if((degres>=0 && degres<22.5) ^ (degres<=359 && degres>=360)) return "N";
  else if(degres<=22.5 && degres<45) return "NNE";
  else if(degres<=45 && degres<67.5) return "NE";
  else if(degres<=67.5 && degres<90) return "ENE";
  else if(degres<=90 && degres<112.5) return "E";
  else if(degres<=112.5 && degres<135) return "ESE";
  else if(degres<=135 && degres<157.5) return "SE";
  else if(degres<=157.5 && degres<180) return "SSE";
  else if(degres<=180 && degres<202.5) return "S";
  else if(degres<=202.5 && degres<225) return "SSO";
  else if(degres<=255 && degres<247.5) return "SO";
  else if(degres<=247.5 && degres<270) return "OSO";
  else if(degres<=270 && degres<292.5) return "O";
  else if(degres<=292.5 && degres<315) return "ONO";
  else if(degres<=315 && degres<337.5) return "NO";
  else if(degres<=337.5 && degres<359) return "NNO"; 
  else return "?";
}
