#include <LiquidCrystal.h>
#include <dht.h>

#define CONFERMA 10
#define SET_BUTTON 8
#define CHANGE_MODE 9
#define BUZZER 7
#define DHT11_PIN 6
#define LED 13

dht DHT;

const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char timeline[16];
char timeline2[16];

//default type of Pomodoro timer
int work_time_min = 60; 
int break_time_min= 10;

//flag for increment time
bool study_time = true;
bool _running = false;


//variable for manage the time
unsigned short minutes, seconds;
unsigned short pomo_minutes,pomo_seconds;
long milliseconds;
int hours_spent = 0;
int minutes_spent = 0;
int seconds_spent =0;

//variable for manage the buzzer
bool _tone = false;
int tone_counter=0;

//mode allowed
enum mode {pomodoro,crono,temperature,setting};
mode type = pomodoro;

//setting flag 
bool set_work_time  = false;
bool set_break_time = false;


/*----------------MANAGE BUTTON----------------------------*/
long time1=0;
long time2=0;
long time3=0;

int state1 = HIGH;
int state2 = HIGH;
int state3 = HIGH; 

int reading1;
int reading2;
int reading3;

int previous1 = LOW;
int previous2 = LOW;
int previous3 = LOW; 

bool _switch1=false;
bool _switch2=false;
bool _switch3=false;

int debounce=100;

void buttonPressed(){
  reading1=digitalRead(CONFERMA);
  reading2=digitalRead(CHANGE_MODE);
  reading3=digitalRead(SET_BUTTON);

  if(reading1 == HIGH  && previous1 == LOW && millis()- time1 > debounce){
    _switch1=true;
    if(state1==HIGH)
      state1= LOW;
    else
      state1 = HIGH;
    time1 = millis();
  }

  if(reading2 == HIGH  && previous2 == LOW && millis()- time2 > debounce){
    _switch2=true;
    if(state2==HIGH)
      state2= LOW;
    else
      state2 = HIGH;
    time2 = millis();
  }
   if(reading3 == HIGH  && previous3 == LOW && millis()- time3 > debounce){
    _switch3=true;
    if(state2==HIGH)
      state3= LOW;
    else
      state3 = HIGH;
    time3= millis();
  }
  
  previous1 = reading1;
  previous2 = reading2;
  previous3 = reading3;
}

/*-----------------------------SETUP-------------------------------*/

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Pomodoro");

  Serial.begin(9600);
  
  //setup LED for break
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  //setup button
  pinMode(CONFERMA,INPUT);
  pinMode(CHANGE_MODE,INPUT);
  
}


/*-----------------------------LOOP-----------------------------------*/
void loop() {
  
  /*------------------MANAGING TIME-----------------*/
  if(millis() - milliseconds > 990){
    if(_running){
      seconds++;
      if(study_time)
        seconds_spent++;
    }
    if(_tone)
      tone_counter++;
      
    milliseconds=millis();
  }
  
  if(study_time){
    pomo_minutes=work_time_min-1-minutes;
  }
  else
    pomo_minutes=break_time_min-1-minutes;
  
  pomo_seconds=60-1-seconds;
  buttonPressed();
  
  
  
  /*------------MANAGING LED---------------------------------------*/
  if(study_time && _running && !(minutes_spent==0 && seconds_spent==0))
    digitalWrite(LED,HIGH);
  else
    digitalWrite(LED,LOW);

/*-----------MANAGING BUZZER---------------------------------------*/
   if(!_running && tone_counter < 21){
     if(tone_counter % 2 == 0 &&  _tone){ 
       tone(BUZZER,200);
      }
      else{
        noTone(BUZZER);
      }
   }
   else{
     noTone(BUZZER);
     _tone=false;
     tone_counter=0;
   }

/*--------------------------SWITCH1--------------------*/
  if(_switch1 && type != temperature)
  {
    lcd.clear();
    
    if(type==setting){
      if(set_work_time)
        work_time_min += 5;
      else if (set_break_time)
        break_time_min ++;
    }
    
    if(_running)
    {
      _running=false;
      
      if(type==pomodoro){
        minutes=0;
        seconds=0;
          
        if(study_time){
          study_time=false;
          sprintf(timeline2,"gia` stanco?   ");
          lcd.setCursor(0,1);
          lcd.print(timeline2);
        }
        else{
        study_time=true;
        sprintf(timeline2,"vai de fretta? ");
        lcd.setCursor(0,1);
        lcd.print(timeline2);
        }
      }
      if(type==crono){
        if(study_time){
          study_time=false;
          sprintf(timeline2,"pausa");
          lcd.setCursor(0,1);
          lcd.print(timeline2);
        }
      }
      
    }
    else
    {
      _running=true;
      if(type==pomodoro){
        minutes=0;
        seconds=0;
      }
      if(type==crono){
        study_time=true;
        lcd.setCursor(0,1);
        lcd.print(timeline2);
      }
    }
  }
  _switch1=false;

/*----------------------SWITCH 2---------------------------------*/
  //change button pressed
  if(_switch2){
    minutes=0;
    seconds=0;
    _running=false;
    
    lcd.clear();
    if(type==pomodoro){
      lcd.setCursor(0,0);
      lcd.print("Cronometro");
      type=crono;
    }
    else if(type==crono){
      lcd.setCursor(0,0);
      lcd.print("Temperature");
      type = temperature;
      study_time = false;
    }
    else if(type==temperature){
       lcd.setCursor(0,0);
      lcd.print("Pomodoro");
      type = pomodoro;
      study_time = true;
    }

    if(type==setting){
      if(set_work_time){
        set_work_time=false;
        set_break_time=true;
      }
      else if(set_break_time){
        set_break_time=false;
        set_work_time=true;
      }
    }
  }
  _switch2=false;

/*-----------------------SWITCH 3--------------------------------*/
  if(_switch3){
    lcd.clear();
    if(type==pomodoro){
      _running=false;
      study_time=false;
      
      type=setting;
      set_work_time=true;
      set_break_time=false;
      work_time_min=10;
      break_time_min=1;
      
    }
    else{
        type = pomodoro;
        _running = true;
        study_time = true;
        minutes = 0;
        seconds = 0;
        set_work_time = true;
        set_break_time = false;
    }
  }
  
  _switch3=false;


/*----------------------MANAGING MODE-----------------------------*/


  //first line always of pomodoro 
  if(type==pomodoro){
    sprintf(timeline,"Pomo  %0.2dh %0.2d'",hours_spent,minutes_spent);
    lcd.setCursor(0,0);
    lcd.print(timeline);
  }
  else if(type==crono){
    sprintf(timeline,"Crono %0.2dh %0.2d'",hours_spent,minutes_spent);
      lcd.setCursor(0,0);
      lcd.print(timeline);
  }
  else if(type==setting){
    
    if(set_work_time){
      lcd.setCursor(6,0);
    }
    else if(set_break_time){
      lcd.setCursor(7,1);
    }
    
    sprintf(timeline,"work:  %2d",work_time_min);
    lcd.setCursor(0,0);
    lcd.print(timeline);
    if(set_work_time)
      lcd.print("*");
    sprintf(timeline2,"break: %2d",break_time_min);
    lcd.setCursor(0,1);
    lcd.print(timeline2);
    if(set_break_time)
      lcd.print("*");
    delay(50);
  }
  else if(type==temperature){
    int chk = DHT.read11(DHT11_PIN);
      lcd.setCursor(0,0);
      lcd.print("Temp: " );
      lcd.print(DHT.temperature);
      lcd.print((char)223);
      lcd.setCursor(0,1);
      lcd.print("Humidity: ");
      lcd.print(DHT.humidity);
      lcd.print("%");
      delay(50);
  }
  
  
  if(_running){
    if(seconds==60)
    {
      seconds=0;
      minutes++;
    }
    if(study_time){
      if(seconds_spent==60){
        seconds_spent=0;
        minutes_spent++;
      }
      if(minutes_spent==60){
        minutes_spent=0;
        hours_spent++;
      }
    }
    if(type == pomodoro){
      
      lcd.setCursor(0,1);
      
      if(study_time)
      {
         sprintf(timeline2, "Study %3d' %0.2d\" ", pomo_minutes, pomo_seconds);
         lcd.print(timeline2);
         
         //reached limit of pomodoro timer
         if(minutes==work_time_min){
          study_time=false;
          seconds=0;
          minutes=0;
          _running=false; 
          sprintf(timeline2,"te repusi?     ");
          lcd.setCursor(0,1);
          lcd.print(timeline2);
          _tone=true;
         }
      }
      else{
        sprintf(timeline2,"Break%3d' %0.2d\"",pomo_minutes,pomo_seconds);
        lcd.print(timeline2);
        
        if(minutes==break_time_min){
          study_time=true;
          seconds=0;
          minutes=0;
          sprintf(timeline2,"iamooo?        ");
          lcd.setCursor(0,1);
          lcd.print(timeline2);
          _running=false;
          _tone=true;
         }
      }
    }
    else if(type == crono){
      
      
      lcd.setCursor(0,1);
      sprintf(timeline2,"     %3d' %0.2d\"  ",minutes, seconds);
      lcd.print(timeline2);
    }
  }
  delay(10);


}
