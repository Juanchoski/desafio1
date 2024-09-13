int pinA = A0;
int valor = 0;
void setup (){
Serial.begin(9600);
} 
void loop(){
  valor = analogRead(pinA);
  Serial.println(valor);
}
