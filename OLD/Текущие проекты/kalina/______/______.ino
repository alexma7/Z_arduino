void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
//Термистор
double Thermister(int RawADC) {
  double Temp;
  Temp = log(((10240000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;   // Kelvin to Celcius
  //Temp = (Temp * 9.0)/ 5.0 + 32.0;    // 1 способ Convert Celcius to Fahrenheit 
  //Temp = (Temp * 1.8) + 32.0;   // 2 способ Convert Celcius to Fahrenheit
  return Temp;
}
