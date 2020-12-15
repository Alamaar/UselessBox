#define debug 1
#include <Servo.h>
/*Nuottien taajuuksien määrittely*/
#define c  261
#define d  294
#define e  329
#define f  349
#define g  391
#define gS  415
#define a  440
#define aS  455
#define b  466
#define cH  523
#define cSH  554
#define dH  587
#define dSH  622
#define eH  659
#define fH  698
#define fSH  740
#define gH  784
#define gSH  830
#define aH  880

//Funktioiden esittely
void servonliikutus(int y,int z); //Liikutaa servoja ja ajastaa viiveen, y uusi asento, z=1 kansi z=2 tassu
void toiminto1(void);             //Eri toiminnot
void toiminto2(void); 
void toiminto3(void);
void toiminto4(void);
void toiminto5(void); 
void toiminto6(void);            
void musiikkitoiminto(void);      
void satunnaistoiminto(void);     
void null(void);                  // Tyhjä funktio
void alustaHCsr04(void);          //alustaa etaisyysanturin
int etaisyysHCsr04(void);         //palautaa etäisyyden senttimetreinä 
bool kytkintila(void);            //tutkii kytkimen tilaa palautaa boolean arvon true=päällä false =pois päältä
void toimintovalitsin(int);
void kurkkaa(void);
//Globaaleja muuttujia kurkaustoimintoon
int aktiivisuus = 6;      
unsigned long edellinenaika = 0;

bool asentotunnistin(void);
void tiltsound(void);
unsigned long aloitusaika=0;
int xpysty,ypysty,zpysty;

#define Toimintoja 6  /*toimintojen määrä*/
/*Toimintotaulukko, tätä käytetään funktioiden kutsumiseen satunnaisesti 
 * ja tätä muokkaamalla voi muutta tuotteen toimintaa.
 * 2 rivillä etaisyysanturia käyttävia toimintoja johon mennään satunnaisesti, todennäiköisyys riippuu
 * toimintofunktioiden ja null funktioiden suhteesta.
  */
void *(*Toiminnot[2][Toimintoja])(void) =
{{&toiminto1, &toiminto2, &toiminto3, &toiminto4, &musiikkitoiminto, &satunnaistoiminto},       
{&toiminto5,  &toiminto6, &null     , &null     , &null            ,&null             }};  

//Nuotit - Star Wars Imperial March
int nuotit[73] = {a,a,a,f,cH,a,f,cH,a,0,eH,eH,eH,fH,cH,gS,f,cH,a,0,aH,a,a,aH,gSH,gH,fSH,fH,fSH,0,aS,dSH,dH,cSH,cH,b,cH,0,f,gS,f,a,cH,a,cH,eH,0,aH,a,a,aH,gSH,gH,fSH,fH,fSH,0,aS,dSH,dH,cSH,cH,b,cH,0,f,gS,f,cH,a,f,cH,a};
int nuottikesto[] = {500,500,500,350,150,500,350,150,650,500,500,500,500,350,150,500,350,150,650,500,500,300,150,500,325,175,125,125,250,325,250,500,325,175,125,125,250,350,250,500,350,125,500,375,125,650,500,500,300,150,500,325,175,125,125,250,325,250,500,325,175,125,125,250,350,250,500,375,125,500,375,125,650,650};                                  

//Arduinon pinnien määrittely
const int servo2 = 10;    //tassu
const int servo1 = 9;     //kansi
const int trigPin = 3;    //etäisyysanturi pinnit
const int echoPin = 2;
const int buzz = 8;       //kaiutin
const int kytkin = 5;     //kytkin
Servo tassu;              //servot
Servo kansi;
const int xakseli = A1;   //kiihtyvyysensorinakselit
const int yakseli = A2;
const int zakseli = A3;


void setup() {
 //Alustetaan pinnit, servot, anturit, randomnumerogenerointi

  alustaHCsr04();   
  tassu.attach(servo2);
  kansi.attach(servo1);
  randomSeed(analogRead(0));
  pinMode(kytkin, INPUT_PULLUP);      //Kytketään pinni high tilaan
  tassu.write(10);                    //asetetaan servot ala asentoon
  kansi.write(15);
  xpysty = analogRead(xakseli);         //kalibroidaan pystyasento
  ypysty = analogRead(yakseli);
  zpysty = analogRead(zakseli);
  if(debug){
    Serial.begin(9600);
    Serial.println(xpysty);
    Serial.println(ypysty);
    Serial.println(zpysty);
    Serial.println("alustusvalmis");
      
  }
}

void loop() {
  /* Pääfunktio. Tutkitaan kytkimen tilaa kutsumalla kytkintila funktio ja siirrytään toimintovalintafunktion*/
 
  if(asentotunnistin()){
    if(kytkintila()){
      toimintovalitsin(0);
      aktiivisuus = 0;
    }
    kurkkaa();
  }
}

int etaisyysHCsr04(void){      
  //liipasee etaisyysanturin ja laskee echo pulssin kestosta etaisyyden ja palauttaa sen senttimetreinä. 
  long pulssikesto=0;
  int etaisyys=0,x=0,vanhaetaisyys=0;
 
    digitalWrite(trigPin, HIGH);    //lähetää 10microsekunnin pituisen pulssin trigpin
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);

    pulssikesto = pulseIn(echoPin, HIGH); //mittaa echopin pulssin keston
  
    etaisyys= pulssikesto*0.034/2;        //lasketaan etaisyys äänennopeus ja jaetaan kahdella
  if(debug){
    Serial.println(etaisyys);
  }
  return etaisyys;
}

void alustaHCsr04(){ 
  //alustetaan etaisyysanturi asettamalla pinnit ja laittamalla trigpinni tilaan low
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

bool kytkintila(void){
  //Kun kytkin on päällä palautta arvon true ja kun kytkin pois päältä false
  if(debug){
    Serial.print("Kytkinasento - ");
    Serial.println(!digitalRead(kytkin));
  }
  return !digitalRead(kytkin);
}
void toiminto1 (void){
  if(debug){
    Serial.println("toiminto1"); //Laatikon kansi avautuu, käsi tulee ulos sulkien vivun ja vetäytyy takaisin kannen sulkeutuessa
  }
  servonliikutus(175,1);  
  servonliikutus(175,2);  
  servonliikutus(10,2);   
  servonliikutus(15,1);   
}
void toiminto2 (void){
  if(debug){
    Serial.println("Toiminto2");  //Kansi avautuu, käsi liikkuu hitaasti vivulle sekä takaisin laatikkoon jonka jälkeen kansi sulkeutuu
  }
  servonliikutus(175,1);              
  for (int x=15; x < 175; x=x+2){    //Määrittää asteet sekä kuinka nopeasti asteita lisätään
  tassu.write(x);                   
  delay(50);
}
  for (int x=175; x > 15; x=x-2){   //Sama kuin ylempi komento mutta laatikkoon palautumiseen
    tassu.write(x);
    delay(50);
  }
  servonliikutus(15,1);             
}
void toiminto3 (void){
  if (debug){
    Serial.println("Toiminto3");    //Käsi tulee ulos laatikosta, liikkuu kuin vilkuttaen, laittaa vivun kiinni ja palautuu takaisin
  }
  servonliikutus(175,1);
  delay(500);
  servonliikutus(140,2);
  delay(500);
  servonliikutus(155,2);
  delay(500);
  servonliikutus(140,2);
  delay(500);
  servonliikutus(155,2);
  delay(500);
  servonliikutus(140,2);
  delay(500);
  servonliikutus(175,2);
  delay(500);
  servonliikutus(10,2);
  delay(500);  
  servonliikutus(15,1);
}
void toiminto4 (void){
  if(debug){
    Serial.println("Toiminto4");  //Kansi avautuu hetkeksi, sulkeutuu ja avautuu, käsi sulkee vivun, kansi sulkeutuu ja avautuu hetkeksi ja sulkeutuu sitten taas
  }
  servonliikutus(100,1);
  delay(1000);
  servonliikutus(15,1);
  delay(250);  
  servonliikutus(175,1);
  delay(250);
  servonliikutus(175,2);
  servonliikutus(10,2);
  delay(250);
  servonliikutus(15,1);
  delay(250);
  servonliikutus(100,1);
  delay(1000);
  servonliikutus(15,1);
  
  
}
void toiminto5 (void){
  if(debug){
    Serial.println("Toiminto5");    //Toiminnon ollessa päällä se odottaa että kytkin on laitettu päälle sekä että se havaitsee yli 25cm etäisyydellä jotain jonka jälkeen toiminto 1 tapahtuu
  }
  int etaisyys=0;
  while(etaisyys < 25 || !kytkintila()){  //etäisyyden ollessa alle 25cm tai kytkintilan ollessa käänteisesti päällä laite ei tee mitään
     etaisyys = etaisyysHCsr04();
  }
  toiminto1();    
}
void toiminto6(void){
  /*Tämä toiminto 6 ohjaa kanta ja tassua riippuen etaisyysanturin lukemista. arvot 25-50 sentimetriä
   * ohjataan kantta siten että lähempänä kansi on täysin auki ja kaempana kansi on kiinni. 
   * 2-25 välillä ohjataan tassua samalla periaatteella. tassun kulma voi olla vain 140 astetta enneinkuin se tulee sensorin eteen.
   * funktiossa pysytään aika vakion määrämän ajan verran.
   */
  if(debug){
  Serial.println("Toiminto6");
   }
  int x = 15; //servon kontrollointi muuttuja
  int etaisyys=51; //asetetaan etaisyysmuuttuja toimintojen ulkopuolelle
  unsigned long aika  = 20000 + millis();   //Tallennetaan aika muuttujaan kulunut aika + 20 sekunttia
  unsigned long aikakulunut = 0;
  while(aikakulunut < aika){        //suoritetaan kunees aikakulunut saavuttaa aika arvon.
    aikakulunut = millis();
    etaisyys =  etaisyysHCsr04();         //haetaan etäisyys etäisyysanturi funktiolla.
    etaisyys = constrain(etaisyys, 2, 50); //rajoittaa etaisyys arvot välille 2 cm - 50cm
   
     if (etaisyys <= 4 || kytkintila()){    // kytkin ollessa kiini tai etäisyys ollessa 4cm tai alle käytetään tassua kytkimellä puoli sekunttia
        servonliikutus(175,2);
        delay(500);
        servonliikutus(140,2);        
      }
    if (etaisyys > 25 && etaisyys <= 50){   // ohjataan kantta, 
      x = map(etaisyys, 25 ,50, 175, 15);   //käytetään map funktiota muuttama arvot 20-50 cm servon astearvoiksi välillä 15-175
      x = constrain(x, 15, 175);            //rajoitetaan servon liike välille 15 ja 175 astetta
      servonliikutus(x,1);                       // kirjoitetaan arvo servolle      
    }
    if (etaisyys > 4 && etaisyys <= 25){    //ohjataan tassua 
      x = map(etaisyys, 4 ,25, 140, 10);   
      x = constrain(x, 10, 140);            
      servonliikutus(x,2);
    } 
  delay(100);
  } //end while
servonliikutus(10,2);    //laitetaan kansi ja tassu alas
servonliikutus(15,1);
}
void null(void){
  //Tyhjä funktio, toimintotalukkoa varten.
  if(debug){
   Serial.println("null");
  }
}
void servonliikutus(int Uasento,int z){
  /*(Uasento servon uusisento, int z ohjattava servo) (z=1 kansi) (z=2 tassu)
   * Servon liikutus funktio, servojen liikuttamiseen ja viiveen laskemiseen.
   *  Funktio myös varmistaa ettei servot liiku yli rakenteen rajojen tai ole toisen servon edessä.
   *  Vertailaan vanhaa asentoa uuden asennon arvoon ja lasketaan viive sen mukaan, mitä servoilla menee liikkua.
   */
  int vertailu=0;     //vertailuarvo muuttuja
  int aika=0;         //aika
  int Vasento=0;            //tähän muuttujaan tallennetaan servon vanha asento.
  Uasento = min(Uasento, 175);    //servojen yläraja
  if(z==1){                   //valitaan Vasento:n paikalla ohjattavan servon vanha asento
    Vasento = kansi.read();
    if (tassu.read() != 10){   //varmistetaan että tassu on alhaalla ennenkuin ohjataan kantta
        tassu.write(10);
        delay(300);                     
    }
    Uasento = max(Uasento, 15);   //rajoitetaan minimi 15 asteeseen
    kansi.write(Uasento);
  }
  if(z==2){
    Vasento = tassu.read();
    if (kansi.read() != 175){   //varmistetaan että kansi on auki ennenkuin ohjataan tassua
        kansi.write(175);
        delay(400);
    }
    Uasento = max(Uasento, 10);   //rajoitetaan minimi 10 asteeseen
    tassu.write(Uasento);
  }
  vertailu =  Uasento - Vasento;
  vertailu = abs(vertailu);       //vertailaan nykyisen asennon ja uuden asennon paikkaa, otetaan itseisarvo
  aika = vertailu * 3;            //servon nopeus 100ms per 60 astetta mutta testit osoitti että tarvitaan isompi kertoja.
  delay(aika);                      // viivytetään
}

void toimintovalitsin(int toiminnot){ 
/* Toimintovalitsin joka arpoo satunnailuvun ja suorittaa funktion toimintotaulusta. 
 *  Tulaan uudestaan funktioon ja arvotaan riviltä 1, kun toiminnot = 0, funktioon ja arvotaan 2 rivin vaihtoehdoista.
 *  Jos toiminnot(1) ei tulla uudestaan funktioon.
  */
  if(debug){ 
     Serial.println("toimintovalitsin");
  }
  int luku = random(0,Toimintoja);
  (*Toiminnot[toiminnot][luku])();
  if (toiminnot == 0){
    toimintovalitsin(1);
  }
  } 
void musiikkitoiminto(void){
  /* Musiikki toiminto soittaa nuotit järjestyksessä ja liikutta servoja 
   *  kunnes joko tassu servo laittaa kytkimen kiinni tai käyttäjä 
   */
  if(debug){
  Serial.println("Musiikkitoiminto");
   }
  int servoasento;
  int x=0;
  while(x < 73 && kytkintila()){        //Suoritetaan kun x < 73 tai kytkintila = false
    if (x <= 48){
    servoasento = map(x, 0, 48, 15, 175);               //liikutetaan servoja sen mukaan missä musiikki on menossa
    servonliikutus(servoasento, 1);
   }
   if (x > 48){
    servoasento = map(x, 49, 72, 10, 175);
    servonliikutus(servoasento, 2);
   }
   tone(buzz, nuotit[x], nuottikesto[x]);               //soitetaan musiikki
  
   
   int pauseBetweenNotes = nuottikesto[x] * 1.10;
   delay(pauseBetweenNotes);
   noTone(buzz);
   x++;
  }//endwhile
servonliikutus(10,2);   //laitetaan kansi ja tassu alas.
servonliikutus(15,1);  
}

void satunnaistoiminto(void){
  if(debug){
  Serial.println("Satunnaistoiminto");
   }
   /*Satunnaistoiminto ohjaa laitetta satunnaisesti ottamalla satunnaisia arvoja servojen liikutukseen, soittamalla satunnaisia nuotteja
    * ja viivyttelemällä satunnaisia aikoja.
    */
  int asentotaulukko[] = {15,25,35,175,55,65,75,85,175,105,115,125,135,145,155,175,175,175};
  unsigned long aika  = 10000 + millis();              //Tallennetaan aika muuttujaan kulunut aika + 10 sekunttia
  unsigned long aikakulunut = 0;
  while(aikakulunut < aika && kytkintila()){          //loop loppuu kun kytkin tai aikaraja tulee vastaan
    aikakulunut = millis();
    int sluku= random(1,5);

    switch (sluku){
      case 1:
        servonliikutus(asentotaulukko[random(0,11)],1);       //kannensatunnaiset asennot
        break;
      case 2:
        servonliikutus(asentotaulukko[random(0,11)],2);       //tassun satunnaiset asennot
        break;
      case 3: 
        tone(buzz, random(260,881));                          //satunnainen nuotti
        delay(500);
        noTone(buzz);
        break;
      case 4:
        delay(random(100,2000));                              //satunnainen viivytys
        break;   
    }
    delay(300); 
  } //end while
  
  if (kytkintila()){              //jos kytkinjää pääle aikarajan jälkeen laitetaan kiini
    servonliikutus(175,1);
    servonliikutus(175,2);
  }
  servonliikutus(10,2);    //laitetaan kansi ja tassu alas
  servonliikutus(15,1);
}

void kurkkaa(void){
/*Kurkaa toiminto kurkaa 4 kertaa kun aktiivisuus on nollattu(laitetta käytetty) 15 sekunnin välein.
 */
  if (aktiivisuus < 4){
    if (millis()- edellinenaika >= 15000){        //kun edellisestä ajasta on 15 sekunttia suoritetetaan.
      servonliikutus(60,1);                       //kurkkaus
      delay(500+200*aktiivisuus);                 //pidennetään viivettä joka kerralla
      servonliikutus(15,1);                       //kansi alas
      aktiivisuus++;
      edellinenaika = millis();                     //tallennetaan edelllinen aika
      if(debug){
        Serial.println("kurkkaa");
      }
    }      
  }
}

bool asentotunnistin(void){
  /* Tunnistaa asennon. Laatikon ollessa oikeinpäin palauttaa boolean true arvon.
   *  Laatikon ollessa muussa asennossa käytetään tiltsound() funktiota soittamaan ääni.
   *  Soitetaan ääntä vain 10 sekunttia viimeisestä oikeasta asennota(pysty). Palauetetaan false.
   */ 
  int xarvo = analogRead(xakseli);      //haetaan arvot
  int yarvo = analogRead(yakseli);
  int zarvo = analogRead(zakseli);

  xarvo = (xarvo - (xpysty - 10)) * ((xpysty + 10) - xarvo);          //lasketaan rajaarvoilla +-10. tulos 0 tai positiivinen kun rajojen sisällä (parabeliyhtälö)
  yarvo = (yarvo - (ypysty - 10)) * ((ypysty + 10) - yarvo);
  zarvo = (zarvo - (zpysty - 10)) * ((zpysty + 10) - zarvo);

  
  if (xarvo >= 0 && yarvo >= 0 && zarvo >= 0){          //laatikko oikeinpäin
        noTone(buzz);
        aloitusaika = millis();                         //haetaan aloitusaika
        return true;    
  } 
  else{
    if(aloitusaika + 10000 > millis()){                 // kun 10 sekunttia on kulunut ei soitetaan ääntä
      tiltsound();
    }
    else{
      noTone(buzz);
    }
    return false;
  }
}

void tiltsound(void){
  /* Soittaa kaiuttimesta taajuuden riippuen laatikon asennosta. Arvot kalibroitu testaamalla
   */  
  int nuotti; 
  int xarvo = analogRead(xakseli);    //haetaan arvot
  int yarvo = analogRead(yakseli);
  int zarvo = analogRead(zakseli);
  
  if (0){
    Serial.println("tiltsound");
    Serial.print(xarvo);
    Serial.print("--");
    Serial.print(yarvo);
    Serial.print("--");
    Serial.print(zarvo);
  }

  xarvo = map(xarvo, 270, 412, 1000,1500);          //säädetään arvot sopiviksi taajuuksiksi
  yarvo = map(yarvo, 270, 410, 500,1000);
  zarvo = map(zarvo, 300, 450, 500,0);

  nuotti = xarvo - yarvo - zarvo;                   //lasketaan yhteen taajuusväli 0-1500hz
  
  tone(buzz, nuotti);
  delay(50); 
}
