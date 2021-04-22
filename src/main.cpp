#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
#include <WebServer.h>
#include <EmonLib.h>
#define DEBUG_PORT Serial
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "Agentuino.h"
#include <Base64.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#ifdef DEBUG_PORT

  //#define DEBUG_MSG(...) DEBUG_PORT.print( __VA_ARGS__ )
  #define DEBUG_MSG(args...) DEBUG_PORT.print( args )
#else
  #define DEBUG_MSG(...)
#endif
#define Pinsonoro 6
static uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static uint8_t ip[] = {192,168,1,30};

///////////para uso da medicão da frequencia////////////////
long freq, tempo;
int pulsos;
boolean pulso;
/////////////////////////////////////////////////////////////
  //endereço para ip.
int addr = 0;
int addr1 = 1;
int addr2 = 2;
int addr3 = 3;

int valorrec;
int valorrec1;
int valorrec2;
int valorrec3;
//////////////////
//endereço para Gateway
int addr4 = 4;
int addr5 = 5;
int addr6 = 6;
int addr7 = 7;

int gat;
int gat1;
int gat2;
int gat3;
///////////////////

//endereço para mascara
int addr8 = 8;
int addr9 = 9;
int addr10 = 10;
int addr11 = 11;

int masc;
int masc1;
int masc2;
int masc3;
////////////////////
// endereço para IPTrap
int addr12 = 12;
int addr13 = 13;
int addr14 = 14;
int addr15 = 15;

int iptrap;
int iptrap1;
int iptrap2;
int iptrap3;
//static byte RemoteIP[4] = {iptrap, iptrap1, iptrap2, iptrap3};  // The IP address of the host that will receive the trap
/////////////////////
//////vars para guarda valores de alarmes//

int corrente;
int corrente1;
int corrente2;
int corrente3;

int addr16 =  16;
int addr17 =  17;
int addr18 =  18;
int addr19 =  19;

/////////////////
//para coloração das bolinhas//
String Colorcorrente;
String Colorcorrente1;
String Colorcorrente2;
String Colorcorrente3;
String Colortensao;
//para guarda tensao minima e tensão maxima
int addr20 =  20;
int addr21 =  21;

int tensaomin;
int tensaomax;
/////////////////////
//para guarda valor de temperatura//
int addr22 =  22;
int tempe;
String temperatura;
/////////////////////
//para guardar  valor de frequencia
int addr23 =  23;
int fren;
String Frequencia;
/////////////////////
//para alarme sonoro//
int addr24 = 24;
int alarmesonoro;

int addr25 = 25;
int addr26 = 34;
int addr27 = 44;
int addr28 = 54;
int addr29 = 64;
String value_comunidade;
String value_nome;
String value_user;
String value_senha;
int value_trap;
void (*resetmodo) () = 0; 

#if defined(ESP8266)
  // default for ESPressif
  #define WIZ_CS 15
#elif defined(ESP32)
  #define WIZ_CS 33
#elif defined(ARDUINO_STM32_FEATHER)
  // default for WICED
  #define WIZ_CS PB4
#elif defined(TEENSYDUINO)
  #define WIZ_CS 10
#elif defined(ARDUINO_FEATHER52)
  #define WIZ_CS 11
#else // default for 328p, 32u4 and m0
  #define WIZ_CS 10
#endif

#define WIZ_RESET 4
int SaveConfigsGlobal = 0;
#define PREFIX ""
WebServer webserver(PREFIX, 80);

EnergyMonitor emon[5];

OneWire  ourBus(3);  // Create a 1-wire object

DallasTemperature sensor(&ourBus);

struct sensordata{
  float temperatura;
  float vac;
  float hz;
  float c1;
  float c2;
  float c3;
  float c4;
} sensorData;


struct configData{
 uint8_t net[4][4];
  char comunity[9];
  char nome[9];
  char user[30];
  char senha[30];
  int alarmeCorrente[4];
  int alarmeTensao[2];
  int alarmeHz;
  int alarmeTemperatura;
  int trap;
  int alarmeSonoro;
} config;
const char inner_0[] PROGMEM = "C01,";
const char inner_1[] PROGMEM = "C02,";
const char inner_2[] PROGMEM = "C03,";
const char inner_3[] PROGMEM = "C04,";
const char inner_4[] PROGMEM = "Temp,";
const char inner_5[] PROGMEM = "VAC,";
const char inner_6[] PROGMEM = "Hz,";

const char color_0[] PROGMEM = "AC1,";
const char color_1[] PROGMEM = "AC2,";
const char color_2[] PROGMEM = "AC3,";
const char color_3[] PROGMEM = "AC4,";
const char color_4[] PROGMEM = "AS1,";
const char color_5[] PROGMEM = "AS2,";
const char color_6[] PROGMEM = "AS3,";

const char service_0[] PROGMEM = "IP";
const char service_1[] PROGMEM = "Máscara";
const char service_2[] PROGMEM = "Gateway";
const char service_3[] PROGMEM = "IP Trap";
const char service_4[] PROGMEM = "Comunity";
const char service_5[] PROGMEM = "Nome";
const char service_6[] PROGMEM = "Usuário";
const char service_7[] PROGMEM = "Senha";

const char alarme_0[] PROGMEM = "Corrente G1 (A)";
const char alarme_1[] PROGMEM = "Corrente G2 (A)";
const char alarme_2[] PROGMEM = "Corrente G3 (A)";
const char alarme_3[] PROGMEM = "Corrente G4 (A)";
const char alarme_4[] PROGMEM = "Tensão Min. (V)";
const char alarme_5[] PROGMEM = "Tensão Máx. (V)";
const char alarme_6[] PROGMEM = "Frequência (Hz)";
const char alarme_7[] PROGMEM = "Temperatura (ºC)";

const char *const innerValue[] PROGMEM = {inner_0, inner_1, inner_2, inner_3, inner_4, inner_5, inner_6};
const char *const colorValue[] PROGMEM = {color_0, color_1, color_2, color_3, color_4, color_5, color_6};
const char *const serviceValue[] PROGMEM = {service_0, service_1, service_2, service_3, service_4, service_5, service_6, service_7};
const char *const alarmeValue[] PROGMEM = {alarme_0, alarme_1, alarme_2, alarme_3, alarme_4, alarme_5, alarme_6, alarme_7};

P(head) ="<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>PDU - BRTM32AC12</title><style type=\"text/css\">";
P(css) = ".co{font-size:4em;line-height:1.5;margin-top:15px!important;font-weight:400;text-align:center}.ale{width:15px"
     ";height:15px;border-radius:50%;display:inline-block;float:right}td .ale{float: none;}.foo{position:fixed;left:0;bottom:0;width:100"
     "%;color:#fff}.iwi{width:20%}.car{position:relative;margin:.5rem 0 1rem 0;background-color:#fff;transition:box"
     "-shadow .25s;border-radius:2px}.car .car-ti{font-size:24px;font-weight:300}.car .car-con{padding:24px;border-"
     "radius:0 0 2px 2px}.car .car-con p{margin:0}.car .car-con .car-ti{display:block;line-height:32px;margin-botto"
     "m:8px}.blue{background-color:#0235c5c2!important}html{box-sizing:border-box;line-height:1.5;font-family:Ubuntu;"
     "font-weight:400;color:rgba(0,0,0,.87);font-size:15px}body{margin:0}nav{display:block}*,:after,:before{box-siz"
     "ing:inherit}ul:not(.browser-default){padding-left:0;list-style-type:none}ul:not(.browser-default)>li{list-sty"
     "le-type:none}a{text-decoration:none}.btn,.car{box-shadow:0 2px 2px 0 rgba(0,0,0,.14),0 3px 1px -2px rgba(0,0,"
     "0,.12),0 1px 5px 0 rgba(0,0,0,.2)}table,td,th{border:none}table{width:100%;border-collapse:collapse}table tbo"
     "dy tr td,table thead tr th{text-align:center}tr{border-bottom:1px solid rgba(0,0,0,.12)}td,th{padding:15px 5p"
     "x;display:table-cell;text-align:left;vertical-align:middle;border-radius:2px}.cen{text-align:center}.right{fl"
     "oat:right!important}.row{margin-bottom:60px}.row:after{content:\"\";display:table;clear:both}.row .col{float:"
     "left;box-sizing:border-box;padding:0 .75rem;min-height:1px;margin-left:auto;left:auto;right:auto}.row .col.l3"
     "{width:25%}.row .col.l10{width:83.333%}.row .col.l4{width:33.333%}.row .col.fs1{margin-left:8.333%}.row .col."
     "fs2{margin-left:16.666%}nav{color:#fff;width:100%;height:56px;line-height:56px}nav .nw{position:relative;heig"
     "ht:100%}nav .bl{position:absolute;color:#fff;display:inline-block;font-size:2rem;padding:0}nav ul{margin:0}na"
     "v ul li{transition:background-color .3s;float:left;padding:0}nav ul a{transition:background-color .3s;color:#"
     "fff;display:block;padding:0 15px;cursor:pointer}nav ul a:hover{background-color:rgba(0,0,0,.1)}.btn{font-size"
     ":100%;border:none;border-radius:2px;line-height:36px;padding:0 16px;color:#fff;cursor:pointer}";
P(nav)="</style></head><body><nav><div class=\"nw blue\"><a href=\"#\" class=\"bl\">Brisanet</a><ul class=\"right\"><li"
     "><a href=\"index.html\">Status</a></li><li><a href=\"conf.html\">Configurações</a></li><li><a href=\"log.html\""
     ">Log</a></li></ul></div></nav>";
P(index)="<div class=\"row\"><div class=\"fs1 col l10\">";
P(table)="<table><thead><tr><th>Serviço</th><th>Nome</th><th>Valor</th><th>Status</th></tr></thead><tbody>"
    "<tr><td>01</td><td>Temperatura</td><td id=\"s5\">0 ºC</td><td><div id=\"a5\" class=\"ale\"></div></td>"
    "</tr><tr><td>02</td><td>Tensão</td><td id=\"s6\">0.0V</td><td><div id=\"a6\" class=\"ale\"></div></td></tr>"
    "<tr><td>03</td><td>Frequência</td><td id=\"s7\">0.0 Hz</td><td><div id=\"a7\" class=\"ale\"></div></td></tr></tbody></table></div>";
P(footer) = "</div><div class=\"foo blue\"><p id=\"s15\">nome : comunidade</p></div>";
P(script) = "<script>function gD(){var e=new XMLHttpRequest;e.onreadystatechange=function(){if(4==this.readyState&&200==this.status){var e=JSON.parse(this.responseText);";
P(script_and) = "}},e.open(\"GET\",\"data.json\",!0),e.send()}window.onload=function(){gD(),setInterval(gD,1e4)};</script>";
P(getElementById) = "document.getElementById(\"";
P(innerHTML) = "\").innerHTML=e.";
P(innerFT) = "s15\").innerHTML=e.comu+\":\"+e.nome";
P(backgroundColor) = "\").style.backgroundColor=e.";
P(card) = "col l4\"><div class=\"car\"><div class=\"car-con\"><span class=\"car-ti\"><B>";
P(c_table) = "</B></span><table><thead><tr><th>Serviço</th><th>Valor</th></tr></thead><tbody><tr><td>";

P(form) = "<form  action=\"conf.html\" method=\"GET\"><input name=\"30\" style=\"display: none\" value=\"";

String ls;
String garaio() {
value_user.remove(value_user.length() - 1, 1);
value_senha.remove(value_senha.length() - 1, 1);
ls = ""+value_user+":"+value_senha+"";
char teste[ls.length()];

ls.toCharArray(teste, ls.length() + 1);
/////trasnformando em base64//////
int inputLen = sizeof(teste);
  
int encodedLen = base64_enc_len(inputLen);
char encoded[encodedLen];
  
// Serial.print(teste); Serial.print(" = ");
// note input is consumed in this step: it will be empty afterwards
base64_encode(encoded, teste, inputLen); 
  
//Serial.println(encoded);
return String(encoded);
}

void escreveString(int enderecoBase, String mensagem){ // Salva a string nos endereços de forma sequencial
  if (mensagem.length()>EEPROM.length() || (enderecoBase+mensagem.length()) >EEPROM.length() ){ // verificamos se a string cabe na memória a partir do endereço desejado
    Serial.println ("A sua String não cabe na EEPROM"); // Caso não caiba mensagem de erro é mostrada
    }
    else{ // Caso seja possível armazenar 
        for (unsigned int i = 0; i<mensagem.length(); i++){ 
            EEPROM.write(enderecoBase,mensagem[i]); // Escrevemos cada byte da string de forma sequencial na memória
            enderecoBase++; // Deslocamos endereço base em uma posição a cada byte salvo
            }
        EEPROM.write(enderecoBase,'\0'); // Salvamos marcador de fim da string 
  }
  }
String leString(int enderecoBase){
  String mensagem="";
  if (enderecoBase > EEPROM.length()){ // Se o endereço base for maior que o espaço de endereçamento da EEPROM retornamos uma string vazia
    return mensagem;
  }
  else { // Caso contrário, lemos byte a byte de cada endereço e montamos uma nova String
    char pos;
    do{
      pos = EEPROM.read(enderecoBase); // Leitura do byte com base na posição atual
      enderecoBase++; // A cada leitura incrementamos a posição a ser lida
      mensagem = mensagem + pos; // Montamos string de saídaa
    }
    while (pos != '\0'); // Fazemos isso até encontrar o marcador de fim de string
  }
  return mensagem; // Retorno da mensagem
}
String maq;
void pageStatus(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  int i, j;
  char buffer[6];

  DEBUG_MSG("[Request] Index page\n");

  if (server.checkCredentials(maq.c_str()) == false) {
    server.print(F("HTTP/1.1 401 Authorization Required\n"  
               "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"  
               "Content-Type: text/html\n"  
               "Connnection: close\n\n"  
               "<!DOCTYPE HTML>\n"  
               "<HTML><HEAD><TITLE>Error</TITLE>\n"  
               "</HEAD><BODY><H1>401 Unauthorized</H1></BODY></HTML>\n"));
    return;
  }

  server.httpSuccess();
  if (type != WebServer::HEAD)
  {
    server.printP(head);
    server.printP(css);
    server.printP(nav);
    server.printP(index);
    for(i=1; i<5; i++){
      server.print(F("<div class=\"col l3\"><div class=\"car\"><div class=\"cen\"><span class=\"co\" id=\"s"));
      server.print(i);
      server.print(F("\">0A</span></div><div class=\"car-con\"><span class=\"car-ti\">Grupo 0"));
      server.print(i);
      server.print(F("<div id=\"a"));
      server.print(i);
      server.print(F("\" class=\"ale\"></div></span></div></div></div>"));
    }
    server.printP(table);
    server.printP(footer);
    server.printP(script);
    for(i=1; i<8; i++){
      server.printP(getElementById);
      server.print("s");
      server.print(i);
      server.printP(innerHTML);
      strcpy_P(buffer, (char *)pgm_read_word(&(innerValue[i-1])));
      for(j=0; buffer[j]!='\0'; j++){
          server.print(buffer[j]);
      }

      server.printP(getElementById);
      server.print("a");
      server.print(i);    
      server.printP(backgroundColor);
      strcpy_P(buffer, (char *)pgm_read_word(&(colorValue[i-1])));
      for(j=0; buffer[j]!='\0'; j++){
        server.print(buffer[j]);
      }
    }
    server.printP(getElementById);
    server.printP(innerFT);
    server.printP(script_and);
  }
}

void pageConf(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  DEBUG_MSG("[Request] Conf page\n");

  if (server.checkCredentials(maq.c_str()) == false) {
    server.print(F("HTTP/1.1 401 Authorization Required\n"  
               "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"  
               "Content-Type: text/html\n"  
               "Connnection: close\n\n"  
               "<!DOCTYPE HTML>\n"  
               "<HTML><HEAD><TITLE>Error</TITLE>\n"  
               "</HEAD><BODY><H1>401 Unauthorized</H1></BODY></HTML>\n"));
    return;
  }

  char name[3];
  char value[16];

  server.httpSuccess();
  if(type == WebServer::HEAD){
    server.httpNoContent();
    return;
  }
  if (type == WebServer::POST)
  {
    //IP                i*10+j   00 - 03
    //Máscara           i*10+j   10 - 13
    //Gateway           i*10+j   20 - 23
    //IP Trap           i*10+j   30 - 33
    //Comunity          40+i     44
    //Nome              40+i     45
    //Usuário           40+i     46
    //Senha             40+i     47
    //en trap                    48
    //Corrente G1 (A)   50+i     50
    //Corrente G2 (A)   50+i     51
    //Corrente G3 (A)   50+i     52
    //Corrente G4 (A)   50+i     53
    //Tensão Min. (V)   50+i     54
    //Tensão Máx. (V)   50+i     55
    //Frequência (Hz)   50+i     56
    //Temperatura (ºC)  50+i     57
    //sonoro                     58
    config.trap = 0;
    config.alarmeSonoro= 0;
    while (server.readPOSTparam(name, 3, value, 16)){
      Serial.print(name);
      Serial.print(" = ");
      Serial.println(value);
      int num = atoi(name);
      if(num < 4) config.net[0][num]=atoi(value);
      else if(num < 14) config.net[1][num-10]=atoi(value);
      else if(num < 24) config.net[2][num-20]=atoi(value);
      else if(num<34)config.net[3][num-30] = atoi(value);
      else if(num==44)strcpy(config.comunity, value);
      else if(num==45) strcpy(config.nome, value);
      else if(num==46)strcpy(config.user, value);
      else if(num==47)strcpy(config.senha, value);
      else if(num>49 && num < 54)config.alarmeCorrente[num%50] = atoi(value);
      else if(num == 54)config.alarmeTensao[0]= atoi(value);
      else if(num == 55)config.alarmeTensao[1]= atoi(value);
      else if(num == 56)config.alarmeHz= atoi(value);
      else if(num == 57)config.alarmeTemperatura= atoi(value);
      else if(num==48 && strcmp(value, "ON") == 0)config.trap = 1;
      else if(num == 58 && strcmp(value, "ON") == 0)config.alarmeSonoro= 1;
    }
    SaveConfigsGlobal = 1;
    //para salvar ip
    EEPROM.write(addr, config.net[0][0]);
    EEPROM.write(addr1, config.net[0][1]);
    EEPROM.write(addr2, config.net[0][2]);
    EEPROM.write(addr3, config.net[0][3]);
    //para salvar a mascara
    EEPROM.write(addr8, config.net[1][0]);
    EEPROM.write(addr9, config.net[1][1]);
    EEPROM.write(addr10,config.net[1][2]);
    EEPROM.write(addr11, config.net[1][3]);
    ////////////////////////
    //para salvar gateway.
    EEPROM.write(addr4,config.net[2][0]);
    EEPROM.write(addr5,config.net[2][1]);
    EEPROM.write(addr6,config.net[2][2]);
    EEPROM.write(addr7,config.net[2][3]);
    ///////////////////////
    //para salvar IPTrap
    EEPROM.write(addr12,config.net[3][0]);
    EEPROM.write(addr13,config.net[3][1]);
    EEPROM.write(addr14,config.net[3][2]);
    EEPROM.write(addr15,config.net[3][3]);
    ///////////////////////
    ///para guardar valores da corentes//// 
    EEPROM.write(addr16, config.alarmeCorrente[0]);
    EEPROM.write(addr17, config.alarmeCorrente[1]);
    EEPROM.write(addr18, config.alarmeCorrente[2]);
    EEPROM.write(addr19, config.alarmeCorrente[3]);
    ///////////////////////
    //para guarda valores de tensao//
    EEPROM.write(addr20, config.alarmeTensao[0]);
    EEPROM.write(addr21, config.alarmeTensao[1]);
    ///////////////////////
    //para guarda valor da temperatura//
    EEPROM.write(addr22, config.alarmeTemperatura);
    ///////////////////////
    //para guarda valor de Hz//
    EEPROM.write(addr23, config.alarmeHz);
    ///////////////////////
    //para guarda valor de alarme sonoro//
    EEPROM.write(addr24, config.alarmeSonoro);
   
  escreveString(addr25, config.comunity);
  escreveString(addr25, config.comunity);
  escreveString(addr26, config.nome);
  escreveString(addr27,config.user);
  escreveString(addr28, config.senha);
  EEPROM.write(addr29, config.trap);
  SaveConfigsGlobal = 1;
resetmodo();
  } else {
    int i, j;
    char buffer[20];
    server.printP(head);
    server.printP(css);
    server.printP(nav);
    server.print(F("<div class=\"row\"><form action=\"conf.html\" id=\"tantofaz\" method=\"POST\"><div class=\"fs2 "));
    server.printP(card);
    server.print("Host");
    server.printP(c_table);
    for(i=0; i<8; i++){
      strcpy_P(buffer, (char *)pgm_read_word(&(serviceValue[i])));
      for(j=0; buffer[j]!='\0'; j++){
        server.print(buffer[j]);
      }
      server.print("</td><td>");
      if(i<4){
       for(int j=0; j<4; j++){
          server.print("<input class=\"iwi\" value=\"");
          server.print("255");
          server.print("\" name=\"");
          server.print((i*10)+j);
          server.print("\" type=\"text\">");
        }
      } else {
        server.print("<input placeholder=\"");
        server.print("no maximo oito caracteres");
        server.print("\" name=\"");
        server.print(40+i);
        server.print("\" type=\"text\">");
      }
        server.print("</td></tr><tr><td>");
    }
    server.print(F("Trap enable</td><td><input name=\"48\" value=\"ON\" type=\"checkbox\"></td></tr></tbody></table></div></div></div><div class=\""));
    server.printP(card);
    server.print("Alarmes");
    server.printP(c_table);
    for(i=0; i<8; i++){
      strcpy_P(buffer, (char *)pgm_read_word(&(alarmeValue[i])));
      for(j=0; buffer[j]!='\0'; j++){
        server.print(buffer[j]);
      }
      server.print("</td><td><input class=\"iwi cen\" value=\"");
      server.print("10");
      server.print("\" name=\"");
      server.print(50+i);
      server.print("\" type=\"text\"></td></tr><tr><td>");
    }
    server.print(F("Alarme<br>Sonoro</td><td><input name=\"58\" value=\"ON\" type=\"checkbox\"></td></tr></tbody></table></div></div></div>"));
    server.print(F("<div class=\"col\" style=\"margin-top: 45%\"><button type=\"button\" onclick=\"result = window.confirm('voce realmente deseja aplicar as alteraçoes'); if(result == true) {document.getElementById('tantofaz').submit();}\" class=\"btn blue\">APLICAR</button></div></form>"));
    server.printP(footer);
    server.printP(script);
    server.printP(getElementById);
    server.printP(innerFT);
    server.print("}},e.open(\"GET\",\"data.json\",!0),e.send()}window.onload=function(){gD()};</script>");
  }
}

void pageLog(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  DEBUG_MSG("[Request] Log page\n");

  if (server.checkCredentials(maq.c_str()) == false) {
    server.print(F("HTTP/1.1 401 Authorization Required\n"  
               "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"  
               "Content-Type: text/html\n"  
               "Connnection: close\n\n"  
               "<!DOCTYPE HTML>\n"  
               "<HTML><HEAD><TITLE>Error</TITLE>\n"  
               "</HEAD><BODY><H1>401 Unauthorized</H1></BODY></HTML>\n"));
    return;
  }

  server.httpSuccess();
  if (type != WebServer::HEAD)
  {
    server.printP(head);
    server.printP(css);
    server.printP(nav);
    server.printP(footer);
  }
}
String Color = "green";
void datajson(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  DEBUG_MSG("[Request] Json\n");
  if (server.checkCredentials(maq.c_str()) == false) {
    server.print(F("HTTP/1.1 401 Authorization Required\n"  
               "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"  
               "Content-Type: text/html\n"  
               "Connnection: close\n\n"  
               "<!DOCTYPE HTML>\n"  
               "<HTML><HEAD><TITLE>Error</TITLE>\n"  
               "</HEAD><BODY><H1>401 Unauthorized</H1></BODY></HTML>\n"));
    return;
  }
  
 String myjson;

  server.httpSuccess("application/json");
  if (type != WebServer::HEAD)
  {
    DynamicJsonDocument doc(400);
     doc["C01"] = String(sensorData.c1) +"A";
     doc["C02"] = String(sensorData.c2) +"A";
     doc["C03"] = String(sensorData.c3) +"A";
     doc["C04"] = String(sensorData.c4) +"A";
     doc["AC1"] = Colorcorrente;
     doc["AC2"] = Colorcorrente1;
     doc["AC3"] = Colorcorrente2;
     doc["AC4"] = Colorcorrente3;
     doc["AS1"] = temperatura;
     doc["AS2"] = Colortensao;
     doc["AS3"] = Frequencia;
     doc["Temp"] = String(sensorData.temperatura) + "C";
     doc["VAC"] = String (sensorData.vac) + "V";
     doc["Hz"] = String (sensorData.hz) + "Hz";
     doc["nome"] = value_nome; 
     doc["comu"] = value_comunidade;
 
     serializeJson(doc, myjson);

     server.print(myjson);
  } 
}

void setup()
{ Serial.begin(9600);
pinMode(Pinsonoro, OUTPUT);
digitalWrite(Pinsonoro, LOW);
//var para retirar valor da eeprom//
valorrec = EEPROM.read(addr);
valorrec1 = EEPROM.read(addr1);
valorrec2 = EEPROM.read(addr2);
valorrec3 = EEPROM.read(addr3);

masc = EEPROM.read(addr8);
masc1 = EEPROM.read(addr9);
masc2 = EEPROM.read(addr10);
masc3 = EEPROM.read(addr11);

gat = EEPROM.read(addr4);
gat1 = EEPROM.read(addr5);
gat2 = EEPROM.read(addr6);
gat3 = EEPROM.read(addr7);

iptrap = EEPROM.read(addr12);
iptrap1 = EEPROM.read(addr13);
iptrap2 = EEPROM.read(addr14);
iptrap3 = EEPROM.read(addr15);

corrente = EEPROM.read(addr16);
corrente1 = EEPROM.read(addr17);
corrente2 = EEPROM.read(addr18);
corrente3 = EEPROM.read(addr19);

tensaomin = EEPROM.read(addr20);
tensaomax = EEPROM.read(addr21);
 
tempe = EEPROM.read(addr22);

fren = EEPROM.read(addr23);

alarmesonoro = EEPROM.read(addr24);

value_comunidade = leString(addr25);
value_nome = leString(addr26);
value_user = leString(addr27);
value_senha = leString(addr28);
value_trap = EEPROM.read(addr29);
///////////////FIM///////////////
//garaio();
maq = garaio();
if(SaveConfigsGlobal == 0 && valorrec != 0) {
IPAddress myip(valorrec, valorrec1, valorrec2, valorrec3);
IPAddress mascara(masc, masc1, masc2, masc3);
IPAddress mygateway(gat,gat1,gat2,gat3);
Ethernet.begin(mac, myip, mygateway, mascara);
Serial.println("peguei o ip  modificado"); 
Serial.println(Ethernet.localIP());
}

if(SaveConfigsGlobal == 0 && valorrec == 000 and valorrec1 == 000) {
Ethernet.begin(mac, ip);
//Serial.println("peguei ip padrao");
EEPROM.write(addr, 0);
EEPROM.write(addr1, 0);
EEPROM.write(addr2, 0);
EEPROM.write(addr3, 0);
} 

  Serial.println(Ethernet.localIP());
  Serial.begin(9600);
  sensor.setResolution(0, 12); 

  emon[0].voltage(A0,210,1.7);
  emon[0].current(A1, 111.1);
  emon[1].current(A2, 111.1);
  emon[2].current(A3, 111.1);
  emon[3].current(A4, 111.1);

  pulso=HIGH;                 // Quando o Arduino e ligando pulso recebe nivel logico alto
  pinMode(5,INPUT);           // Configura o pino D5 como entrada

  sensor.begin();

  webserver.setDefaultCommand(&pageStatus);
  webserver.addCommand("index.html", &pageStatus);
  webserver.addCommand("conf.html", &pageConf);
  webserver.addCommand("log.html", &pageLog);
  webserver.addCommand("data.json", &datajson);
  webserver.begin();

 // start the OTEthernet library with internal (flash) based storage
  ArduinoOTA.begin(Ethernet.localIP(), "Arduino", "password", InternalStorage);
}

void loop() {
  ArduinoOTA.poll();
  static uint32_t time = 0;
  int i;
  static int c = 0;
  unsigned long f = 0;
  char buff[64];
  int len = 64;
//////////
  tempo = millis();           // Configura a variavel tempo para milisegundos
  if(digitalRead(5)==LOW)    // Se a entrada D5 estiver alto 
  {
    if(pulso==HIGH)           // Se pulso estiver alto
    {
      pulsos = pulsos++;    // Some os pulsos 
    }
    pulso=LOW;                // A veriavel pulso é configurada para nivel baixo
  }
  else{                       // Senão a variavel pulso continua em nivel alto
    pulso=HIGH;
  }
  if(tempo%2000==0){            // Se o modulo do tempo for igual a zero execute:
    sensorData.hz = pulsos/2;             // a Frequencia recebe a somatoria dos pulsos e divide por 2
    pulsos=0;                           // Reinicia a contagem dos pulsos
  }
  /////////////
  webserver.processConnection(buff, &len);
  if( time+10000 < millis()){
    time=millis();
    Serial.println(value_nome);
    Serial.println(value_comunidade);
   // if(value_trap == 1) {
   // static uint32_t locUpTime = 0;
   //Agentuino.Trampa("mandando um teste",RemoteIP, locUpTime);     // You need to specify a message, the remote host and the locUpTime
    //delay(1000);
    //locUpTime = locUpTime + 100;
 //}
  }

  switch (c) {
    case 0:
      for(i=0; i<10; i++){
        f += pulseIn(2, HIGH, 35);
      }
      c++;
      break;
      case 1:
      emon[0].calcVI(20,350);
      c++;
      break;
    case 2:
      emon[1].calcVI(20,350);
      c++;
      break;
    case 3:
      emon[2].calcVI(20,350);
      c++;
      break;
    case 4:
      emon[3].calcVI(20,350);
      c++;
      break;
      default:
      sensor.requestTemperatures();
      c=0;
  }

  //emon[0].serialprint();

  sensorData.vac = emon[0].Vrms;
  sensorData.c1 = emon[1].Irms;
  sensorData.c2 = emon[2].Irms;
  sensorData.c3 = emon[3].Irms;
  sensorData.c4 = emon[4].Irms;
  
  if (sensorData.c1 >= corrente){ Colorcorrente =  "red"; 
  } else {Colorcorrente = "green";}
   
  if (sensorData.c2 >= corrente1){ Colorcorrente1 =  "red"; 
  } else {Colorcorrente1 = "green";}

  if (sensorData.c2 >= corrente2){ Colorcorrente2 =  "red"; 
  } else {Colorcorrente2 = "green";}

  if (sensorData.c4 >= corrente3){ Colorcorrente3 =  "red"; 
  } else {Colorcorrente3 = "green";}
  if( sensorData.vac >=  tensaomax ){
      Colortensao = "red";
  }
  if(sensorData.vac <= tensaomin) {
    Colortensao = "orange";
  }
  if(sensorData.vac > tensaomin && sensorData.vac < tensaomax){
    Colortensao = "green";
  }
  
  sensorData.temperatura = sensor.getTempCByIndex(0);
  if (sensorData.temperatura >= tempe)
  {
    temperatura = "red";
  }else
  {
    temperatura = "green";
  }
  
if (sensorData.hz  >= fren)
  {
    Frequencia = "red";
  }else
  {
    Frequencia = "green";
  }
if(alarmesonoro == 1) {
 if(sensorData.vac > tensaomax + 10){
   digitalWrite(Pinsonoro, HIGH);
 }
}
}
//////////pinos de ligação de  de sensor de corrente///////////
                      //A0,A1,A2,A3,D3, D5, pino D2 para medição da frequencia //
//sendo que nao pode usar  o pino d5 pois ele foi destinado a ler a frequencia///                      