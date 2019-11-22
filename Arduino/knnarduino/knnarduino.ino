String tess;
String buf;
char buf_temp;
int leng;
int i,j;
int buf2;

int textdata[][2]=
{
  {'a',100000},
  {'b',110000},
  {'c',100100},
  {'d',100110},
  {'e',100010},
  {'f',110100},
  {'g',110110},
  {'h',110010},
  {'i',010100},
  {'j',010110},
  {'k',101000},
  {'l',111000},
  {'m',101100},
  {'n',101110},
  {'o',101010},
  {'p',111100},
  {'q',111110},
  {'r',111010},
  {'s',011100},
  {'t',011110},
  {'u',101001},
  {'v',111001},
  {'w',010111},
  {'x',101101},
  {'y',101111},
  {'z',101011},
};

void  setup()
{
  Serial.begin(9600);
  pinMode(3,OUTPUT); 
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
}

int first(int a)
{
  a = a/100000;
  a*=60;
  return a;
}

int second(int a)
{
  a = a % 100000;
  a = a/10000;
  a*=60;
  return a;
}

int third(int a)
{
  a = a % 10000;
  a = a/1000;
  a*=60;
  return a;
}

int fourth(int a)
{
  a = a % 1000;
  a = a/100;
  a*=60;
  return a;
}

int fifth(int a)
{
  a = a % 100;
  a = a/10;
  a*=60;
  return a;
}

int sixth(int a)
{
  a = a % 10;
  a = a/1;
  a*=60;
  return a;
}

void loop()
{
    while(Serial.available())
      {
        char wait = Serial.read();
        tess.concat(wait);
      }
    
    leng = tess.length();
    for(i = 0 ; i < leng ; i++)
      {
        buf = tess.substring(i,i+1);
        buf_temp = buf[0];

        if (isupper(buf_temp))
          {
          strlwr(buf_temp);
          analogWrite(3,0);
          analogWrite(4,0);
          analogWrite(5,0);
          analogWrite(6,0);
          analogWrite(7,0);
          analogWrite(8,60);
          delay(700);
          }
  
          for(j = 0;j < sizeof(textdata)/(sizeof(int)*2) ;j++)
         {
          
          if (buf_temp == textdata[j][0])
            {
             buf2 = textdata[j][1];    
             analogWrite(3,first(buf2));
             analogWrite(4,second(buf2));
             analogWrite(5,third(buf2));
             analogWrite(6,fourth(buf2));
             analogWrite(7,fifth(buf2));
             analogWrite(8,sixth(buf2));
             delay(200);

             analogWrite(3,0);
             analogWrite(4,0);
             analogWrite(5,0);
             analogWrite(6,0);
             analogWrite(7,0);
             analogWrite(8,0);
            }
        }
        delay(500);
      }tess = "";
}
