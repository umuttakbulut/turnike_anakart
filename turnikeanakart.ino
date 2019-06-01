#include <SoftwareSerial.h>

#include <SPI.h>
#include <seri.hpp>
#include <tdef.hpp>
 #include <Ethernet.h>
#include <EEPROM.h>;
//#include <SD.h>
 
 #include "SdFat.h"
 
SdFat SD;

#include <Wire.h>

#define BAUDRATE 9600
#define LISTSIZE 250
//#define debug 
int readyn=0;
int numcards=-1;
int established=0;
 File bck;
 File webFile;
 File logfile;
File myFile;
IPAddress ip(10, 0, 0, 95);


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
EthernetServer server(80);
MFRC522::Uid cards[LISTSIZE];
MFRC522::Uid lastread;
MFRC522::Uid masterlastread;
 void logtofile(String data);
HSeri * seri1=new HSeri(&Serial1);
HSeri * seri2=new HSeri(&Serial2);
void setup() {
  // put your setup code here, to run once:
   //delay(5000);
  
   while(!Serial);
  //delay(57600);
pinMode(10,OUTPUT);
//digitalWrite(10,HIGH);
//SPI.begin();  
    
  
  Serial.begin(BAUDRATE);
Serial1.begin(BAUDRATE);
Serial2.begin(BAUDRATE);

#ifdef debug 

Serial.println("Debug : on");
 MFRC522::Uid o;
 o.size=5;
 char b[20];
 o.uidByte[0]='1';
 Serial.println(BUFSIZ);
 sprintf(b,"__%02X__",o.uidByte);
 
  Serial.println(sizeof(o.uidByte));
 Serial.println(b);
 Serial.println(o.size);
#endif

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
   // return;
  }else{
    syslogtofile("system startup");
     Serial.println(" log began!");
    }
delay(100);
//    
//pinMode(LED_BUILTIN, OUTPUT);
//establishContact(); 

//readlogfile();
   
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
 delay(1000);
   seri1->writeInt(TREADY);
     seri1->writeInt(0);
    EEPROM.get(0,cards[0]);
   for(int i=0; EEPROM.get(i*sizeof(cards[i]), cards[i]).size!=0;i++){
   dump_byte_array(cards[i].uidByte,cards[i].size);
    numcards++;
    Serial.println("-------------------------");
}

}
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
COMMANDS k;
void loop() {
  
//seri1->writeInt(NOOP);
   //Serial.println("lop");
//    seri1.listen();//
//// write commands must loop in here write them
/*
if(readyn==0){

    // delay(1000);
}
*/
if(seri2->available()){
 // Serial.println("jumbo");
  k =seri2->readInt();
  switch(k){
    case READCLOG:
    {
      masterlastread = seri2->readUid();
      int rid = seri2->readInt();
      int rd = seri2->readInt();
      //Serial.println(rid);
      //Serial.println(rd);
      //Serial.println("--");
      dump_byte_array(masterlastread.uidByte, 10);
      //Serial.println("---------------------");
     // Zaman z = getTime();
     // char logs[100];
     // lastread = masterlastread;
    //  sprintf(logs,"\"%d\",\"%d\",\"%02X%02X%02X%02X\",\"%d-%02d-%02d %02d:%02d:%02d\"\r\n",rid,rd,lastread.uidByte[0],lastread.uidByte[1],lastread.uidByte[2],lastread.uidByte[3],z.dd,z.mm,z.yyyy,z.h,z.m,z.s);
    //  Serial.print(logs);
    }
    break;
    case NOOP: break;
    default:
      Serial.println(".");
      Serial.println(k);
      Serial2.end();
      Serial2.begin(BAUDRATE);
      readyn=0;
      delay(1000);
      break;
  }
}
if(seri1->available()){
  
 k =seri1->readInt();
 
  switch(k){
  
  case READCLOG:
  {

//    Serial.println("clogg");
         lastread=seri1->readUid();
        
     int rid=seri1->readInt();
    
       int rd=seri1->readInt();

       Serial.println(rid);
         Serial.println(rd);
         Serial.println("--");
         Serial.println(lastread.uidByte[0],HEX);
         Serial.println(lastread.uidByte[1],HEX);
         Serial.println(lastread.uidByte[2],HEX);
         Serial.println(lastread.uidByte[3],HEX);
         Serial.println(lastread.uidByte[4],HEX);
         Serial.println(lastread.uidByte[5],HEX);
         Serial.println(lastread.uidByte[6],HEX);
         Serial.println(lastread.uidByte[7],HEX);
         Serial.println(lastread.uidByte[8],HEX);
         Serial.println(lastread.uidByte[9],HEX);
         
         Serial.println("---------------------");

         if(rid==-1||rid>50){
          readyn=0;
          
          }
       /*   RTC.yyyy=2017;
          RTC.mm=5;
          RTC.dd=54;
          RTC.h=8;
     RTC.m=33;
         RTC.writeTime();
         */
       //  delay(1000);
       if(cexists(lastread)>-1){
          seri1->writeInt(OPENDOOR);
          seri1->writeInt(rid);
           seri1->writeInt(rd);

             Zaman z;
    z=getTime();
      //Serial.println("RCLOG");
  //  "1","24234","23-11-2017 13:44:00"
    char logs[100];
  sprintf(logs,"\"%d\",\"%d\",\"%02X%02X%02X%02X\",\"%d-%02d-%02d %02d:%02d:%02d\"\r\n",rid,rd,lastread.uidByte[0],lastread.uidByte[1],lastread.uidByte[2],lastread.uidByte[3],z.dd,z.mm,z.yyyy,z.h,z.m,z.s);
   logtofile(logs);
   Serial.println(logs);
       }
         // delay(1000);
         // return;
  }
  break;
  case TREADY:
 {
  unsigned int readerid;
  readerid=seri1->readInt();
  if(readerid<1||readerid>50){
    seri1->writeInt(TREADY);
     seri1->writeInt(0);
    
    }else{
  Serial.print("READY:");
  Serial.println(readerid);
    
  readyn++;
  }
   return;
  }
 
  break;
  case NOOP:
 // digitalWrite(LED_BUILTIN,LOW);
  //  delay(1000);
  // digitalWrite(LED_BUILTIN,HIGH);
   
  // Serial.println("oke");
  // return;
  break;
  
    default:
  //  while(seri1->available()){
   //   seri1->read();
    //  }
    Serial.print(".");
    Serial.println(k);
    Serial1.end();
    Serial1.begin(BAUDRATE);
    readyn=0;
    delay(1000);
  //  Serial.end();
//  delay(1);
    break;
    }

}


if(readyn==0){
    delay(10);
     seri1->writeInt(TREADY);
     seri1->writeInt(0);
     seri1->flush();
    delay(1000);
    }





   int index = 0;
    char clientline[BUFSIZ];
    EthernetClient client = server.available();
    if (client) {
        boolean currentLineIsBlank = true;
        index = 0;
        while (client.connected()) {
            if (client.available()) { 
                char c = client.read();
                if (c != '\n' && c != '\r') {
                  clientline[index] = c;
                  index++;
                  if (index >= BUFSIZ) 
                    index = BUFSIZ -1;
 
                  continue;
                }
                clientline[index] = 0;
                char* filename = processFile(clientline);
               // Serial.print("Requested: ");
               // Serial.println(filename);
                /* Setup AJAX Requests Here:
                 * These are just direct output values that are requested 
                 * with AJAX.
                 */
                if (strstr(clientline, "GET /?data=val1") != 0) {
                  code200(client);
                  client.println(1337);
                  break;
                }
                if (strstr(clientline, "GET /lastread") != 0) {
                  code200(client);
                  for(int i=0;i<masterlastread.size;i++){
                   client.write(masterlastread.uidByte[i]);
                    }
//                  dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
                  
                  //client.println("AABBCCDD");
                  break;
                }

  if (strstr(clientline, "GET /time?") != 0) {
                  Zaman z;
                //  z.dd,z.mm,z.yyyy,z.h,z.m,z.s
                  code200(client);
                   char* t=strtok(clientline,"=");
                    t=strtok(NULL,"=");
            if(t){
              
              z.yyyy=atoi(strtok(t,","));
              z.mm=atoi(strtok(NULL,","));
              z.dd=atoi(strtok(NULL,","));
              z.h=atoi(strtok(NULL,","));
              z.m=atoi(strtok(NULL,","));
              z.s=atoi(strtok(NULL,","));
                RTC.yyyy=z.yyyy;
          RTC.mm=z.mm;
          RTC.dd=z.dd;
          RTC.h=z.h;
        RTC.m=z.m;
         RTC.s=z.s;
        
         RTC.writeTime();
          Serial.println(z.s);
           client.println("ok");
            }
           
                    
                
                  
                  //client.println("AABBCCDD");
                  break;
                }
    if (strstr(clientline, "GET /del?") != 0) {
                  
                  code200(client);
                   char* t=strtok(clientline,"=");
                    t=strtok(NULL,"=");
            if(t){
              MFRC522::Uid u;
              u.size=strlen(t)/2;
              for(int a=0;a<u.size;a++){
                int b=0;
                sscanf(t+(a*2),"%2x",&b);
                u.uidByte[a]=(char)b;

               
                // Serial.println((char)b,HEX);
              }
            /*  int yer = cexists(u)*sizeof(MFRC522::Uid);
              char val = EEPROM.read(yer);
              Serial.println(yer);
              Serial.println(val, HEX);
              */
             // dump_byte_array(u.uidByte, u.size);
               if(cexists(u)>-1){
                delUid(u);
                  client.println("Silindi");
                  }else{
                   
                  client.println("Bulunamadi");
                    }
            }
           
                    
                
                  
                  //client.println("AABBCCDD");
                  break;
                }
                 if (strstr(clientline, "GET /add?") != 0) {
                  
                  code200(client);
                   char* t=strtok(clientline,"=");
                    t=strtok(NULL,"=");
            if(t){
              MFRC522::Uid u;
              u.size=strlen(t)/2;
              for(int a=0;a<u.size;a++){
                int b=0;
                sscanf(t+(a*2),"%2x",&b);
                u.uidByte[a]=(char)b;

               
                // Serial.println((char)b,HEX);
              }
             // dump_byte_array(u.uidByte, u.size);
               if(cexists(u)>-1){
                  client.println("Kayitli");
                  }else{
                    addUid(u);
                     
                  client.println("Eklendi");
                    }
            }
           
                    
                
                  
                  //client.println("AABBCCDD");
                  break;
                }
                if (SD.exists(filename)) {
                   code200(client);
                   webFile = SD.open(filename);
                   if (webFile) {
                       while(webFile.available()) {
                           client.write(webFile.read());
                       }
                       webFile.close();
                   }
                   break;
                } else {
                    if (strlen(filename) < 2) {
                      webFile = SD.open("index.html");
                       if (webFile) {
                           while(webFile.available()) {
                               client.write(webFile.read());
                           }
                           webFile.close();
                       }
                    } else {
                      client.println("HTTP/1.1 404 Not Found");
                      client.println("Content-Type: text/html");
                      client.println("Connection: close");
                      client.println();
                      client.println("<html><head><title>404 - Not Found</title></head><body><h1>404 - Not Found</h1></body></html>");
                      break;
                    }
                }
                break;
            }
        } 
      //  delay(1);
        client.stop();



   
}
 //seri1->writeInt(NOOP); 
  //delay(1000);
//delay(1000);
//seri1->flush();
}

 void logtofile(String data){
  //Serial.print("LOG:");
  //seri1.print(data);
   logfile = SD.open("LOG.TXT", FILE_WRITE);
    if (logfile) {
     // Serial.print("Writing to test.txt...");
     // myFile.print("kayit");
      logfile.print(data);
      logfile.close();
     // Serial.println("done.");
    }else {
      //file didn't open.
      Serial.println("error opening test.txt");
    }
  }
  void readlogfile(){
    //read from file..
     // read from the file until there's nothing else in it:
  
    logfile = SD.open("SYSLOG.TXT");
    if (logfile) {
 
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        Serial.write(myFile.read());
      }
      // close the file:
      myFile.close();
      // SD.remove("syslog.txt");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening SYSLOG.txt");
    }


    logfile = SD.open("LOG.TXT");
    if (logfile) {
 
      // read from the file until there's nothing else in it:
      while (logfile.available()) {
        Serial.write(logfile.read());
      }
      // close the file:
      logfile.close();
     //  SD.remove("log.txt");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening LOG.TXT");
    }
    
  }

  void syslogtofile(String data){
 // Serial.print("LOG:");
  //seri1.println(data);
   logfile = SD.open("syslog.txt");
    if (logfile) {
     // Serial.print("Writing to test.txt...");
     // myFile.print("kayit");
      logfile.println(data);
      logfile.close();
     // Serial.println("done.");
    }else {
      // file didn't open.
      Serial.println("error opening test.txt to write");
    }
  }
  
char* processFile(char clientline[255]) {
   char *filename;
   filename = clientline + 5;
  (strstr(clientline, " HTTP"))[0] = 0;
  return filename;
}
void code200(EthernetClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
}
int delUid(MFRC522::Uid a){
  int index=cexists(a);
  if(index==-1){
    return -1;
    }
    MFRC522::Uid empty;
    empty.size=0;
    cards[index]=cards[numcards];
    EEPROM.put(sizeof(MFRC522::Uid)*index, cards[numcards]);
    EEPROM.put(sizeof(MFRC522::Uid)*numcards,empty);
    numcards--;
    return -1;
  }
  int addUid(MFRC522::Uid a){
  if(cexists(a)==-1){
        numcards++;
        cards[numcards] = a;
        EEPROM.put(sizeof(MFRC522::Uid)*numcards,a);
        return numcards;
  }
  
  }
  
int cexists(MFRC522::Uid a){
  for(int i=0;i<LISTSIZE;i++){
    if(cequal(a,cards[i])) return i;
  }
  return -1;
  }
  bool cequal(MFRC522::Uid a,MFRC522::Uid b){
  if(a.size==0||b.size==0){
    return false;
    }
     if(a.size==b.size){
      for(int i=0;i<a.size;i++){
        if(a.uidByte[i]!=b.uidByte[i]){
          return false;
          }
      }
      return true;
      }
      return false;
  }


void establishContact() {
  while (Serial1.available() <= 0) {
  //  seri1->writeInt(NOOP);   // send a capital A
    delay(1000);
  }
  Serial.println("established");
}

