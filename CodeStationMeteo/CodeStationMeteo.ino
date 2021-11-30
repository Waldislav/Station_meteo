#include <DHT.h>
#include <LiquidCrystal.h>

DHT dht(10, DHT11);
const int res=4, en=5, d4=6, d5=7, d6=8, d7=9;
LiquidCrystal lcd(res, en, d4, d5, d6, d7);
#define pluv A0
//#define pluv 2
#define anemo A1
//#define anemo 3
#define girouette A2
#define buzzer 11
#define suivant 13
#define immobile 12

int etat;   //détermine quelle information afficher sur l'écran
//int nbTourAnemo,nbPluv;   //compte le nombre de tour de l'anémomètre et de bascule du pluviomètre
//int tempsMesure=0;        //Permet de faire une mesure de l'anémomètre et du pluvimètre sur 5 secondes
//const float Pi=3.14159;
//const float RBras;        //Rayon des bras de l'anémomètre
//const float valBascule=0.2794;  //combien de mm de pluie sont tombés avec une bascule
int t,h;                  //Variables qui prennent la valeur de la température et de l'humidité 
int temps1,temps2;          //Pour déterminer le nombre de seconde passé entre deux actions
float pluie,vent,degvent;
boolean verif;              //Permet de vérifier si le bouton de maintient a été appuyé ou non

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(pluv,INPUT);
  pinMode(anemo,INPUT);
  pinMode(girouette, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(suivant, INPUT_PULLUP);   //Pour éviter l'effet rebond
  pinMode(immobile, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(pluv),incrementePluv, FALLING);
  //attachInterrupt(digitalPinToInterrupt(anemo),incrementeAnemo, FALLING);
  //nbTourAnemo=0;
  //nbPluv=0;
  
  delay(2000);
  //
  etat=1;
  verif=true;
}

void loop() {
  lcd.clear();
  temps1=millis();
  agir();         //Appel de la méthode qui affiche les informations sur l'écran
  verifieBout();  //Méthode qui nous permet de vérifier si on a fait un appuie sur un bouton et de simuler un delay de 2500 ms 
  /*
    if(temps1-tempsMesure>=5000){ //Méthode qui fait le calcul de la vitesse du vent et la hauteur des précipitations sur 5 secondes (Vrai anémomètre et pluviomètre)
      tempsMesure=millis();
      pluie=nbPluv*valBascule;
      vent=(2*Pi*RBras*nbTourAnemo*3.6)/5;
      nbPluv=0;
      nbTourAnemo=0;
    }
  */
  if(verif)       //Si le bouton de maintient n'est pas appuyé, alors on change les informations sur l'écran
    if(etat==3)
      etat=1;
    else
      etat+=1;
  temps2=0;  
}

void agir(){
  switch(etat){ //Selon la variable etat, on affiche différentes informations sur l'écran
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
        float hi = dht.computeHeatIndex(t, h, false);  //détermine en Fahrenheit la température ressentie
        lcd.setCursor(0,0);
        lcd.print("Temp r: "+String(hi)+"C");
        lcd.setCursor(0,1);
        pluie=analogRead(pluv);
        pluie=(pluie*5)/1023;     //détermine la hauteur des précipitations (simulation avec des variables allant de 0 à 5mm)
        lcd.print("Pluie : "+String(pluie)+"mm");
        break;
    }
    case 3 :{
        lcd.setCursor(0,0);
        vent=analogRead(anemo);
        vent=(vent*120)/1024;   //détermine la vitesse du vent (simulation avec des variables allant de 0 à 120km/h)
        degvent=analogRead(girouette);
        degvent=(degvent*360)/1023;   //détermine la direction de la girouette (simulation sur une plage allant de 0 à 360)
        lcd.print("V vent:"+String(vent)+"km/h");
        if(vent >= 80)                //Si la vitesse du vent dépasse les 80km/h on émet un son avec le buzzer
          digitalWrite(buzzer,HIGH);
        else
          digitalWrite(buzzer,LOW);
        lcd.setCursor(0,1);
        lcd.print("Dir vent :"+getDirectionVent(degvent));  //on fait appel à une fonction qui détermine la direction du vent avec la valeur mesurée
        
        break;
    }
  }
}

void verifieBout(){
  while(temps2<2500){       //La méthode s'arrête lorsque 2.5 secondes sont passées
    temps2=millis()-temps1; //Calcul du temps passé
    if(digitalRead(suivant)==LOW){    //Clic sur le bouton suivant
        temps2=2500;                  //Permet de nous sortir directement de la boucle et de changer d'état
        if(!verif)                    //Change l'état ici si le bouton maintient est appuyé (aucun changement d'état n'est fait en dehors)
          if(etat==3)
            etat=1;
          else
            etat+=1;
        while(digitalRead(suivant)==LOW)  //On s'assure que le bouton soit bien relaché
          delay(50);
    }

    if(digitalRead(immobile)==LOW){ //clic sur le bouton maintient
      verif=!verif;                 //on change la variable qui vérifie si le bouton a été appuyé ou non
      while(digitalRead(immobile)==LOW)   //On s'assure que le bouton soit bien relaché
          delay(50); 
    }
  }
}

String getDirectionVent(float degres){    //Méthode qui renvoi un String de notre direction (simulation)
  String dirVent;
  if ((degres>=0 && degres<22.5) || degres==360) dirVent="N";
  else if(degres>=22.5 && degres<45) dirVent="NNE";
  else if(degres>=45 && degres<67.5) dirVent="NE";
  else if(degres>=67.5 && degres<90) dirVent="ENE";
  else if(degres>=90 && degres<112.5) dirVent="E";
  else if(degres>=112.5 && degres<135) dirVent="ESE";
  else if(degres>=135 && degres<157.5) dirVent="SE";
  else if(degres>=157.5 && degres<180) dirVent="SSE";
  else if(degres>=180 && degres<202.5) dirVent="S";
  else if(degres>=202.5 && degres<225) dirVent="SSO";
  else if(degres>=255 && degres<247.5) dirVent="SO";
  else if(degres>=247.5 && degres<270) dirVent="OSO";
  else if(degres>=270 && degres<292.5) dirVent="O";
  else if(degres>=292.5 && degres<315) dirVent="ONO";
  else if(degres>=315 && degres<337.5) dirVent="NO";
  else if(degres>=337.5 && degres<360) dirVent="NNO"; 
  else dirVent="?";
  return dirVent;
}


/* //Méthodes qui incrémente le nombre de rotation et de bascules (pour le vrai anémomètre et pluviomètre)
 void incrementePluv(){
  nbPluv+=1;
 }
 void incrementeAnemo(){
  nbAnemo+=1;
 }
 */
