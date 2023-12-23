#include <Fuzzy.h>
#include <DHT.h>

// Inisialisasi pin GPIO pada ESP32 untuk kontrol fan menggunakan L298N
const int ENA = 13; // Pin untuk kontrol kecepatan kipas pada L298N (ENA)
const int IN1 = 12; // Pin untuk arah putaran kipas pada L298N (IN1)
const int IN2 = 14; // Pin untuk arah putaran kipas pada L298N (IN2)
const int gasPin = 33; // Pin untuk sensor gas MQ-2 (Analog)
const int DHTPIN = 26; // Pin untuk sensor suhu DHT11

// Konfigurasi sensor suhu DHT11
DHT dht(DHTPIN, DHT11);

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput
FuzzySet *noLeak = new FuzzySet(0, 0, 100, 300);
FuzzySet *smallLeak = new FuzzySet(100, 300, 300, 800);
FuzzySet *bigLeak = new FuzzySet(300, 800, 1000, 1000);

FuzzySet *cold = new FuzzySet(20, 20, 22.5, 25);
FuzzySet *warm = new FuzzySet(22.75, 25, 25, 27.25);
FuzzySet *hot = new FuzzySet(25, 27.5, 30, 30);

// FuzzyOutput
FuzzySet *exhaustLow = new FuzzySet(0, 0, 55, 127.5);
FuzzySet *exhaustMedium = new FuzzySet(90, 127.5, 127.5, 170);
FuzzySet *exhaustHigh = new FuzzySet(127.5, 205, 255, 255);

void setup()
{
  // Set Serial output
  Serial.begin(9600);

  // Inisialisasi sensor DHT
  dht.begin();

  // FuzzyInput gasConcentration
  FuzzyInput *gasConcentration = new FuzzyInput(1);
  gasConcentration->addFuzzySet(noLeak);
  gasConcentration->addFuzzySet(smallLeak);
  gasConcentration->addFuzzySet(bigLeak);
  fuzzy->addFuzzyInput(gasConcentration);

  // FuzzyInput temperature
  FuzzyInput *temperature = new FuzzyInput(2);
  temperature->addFuzzySet(cold);
  temperature->addFuzzySet(warm);
  temperature->addFuzzySet(hot);
  fuzzy->addFuzzyInput(temperature);

  // FuzzyOutput exhaustSpeed
  FuzzyOutput *exhaustSpeed = new FuzzyOutput(1);
  exhaustSpeed->addFuzzySet(exhaustLow);
  exhaustSpeed->addFuzzySet(exhaustMedium);
  exhaustSpeed->addFuzzySet(exhaustHigh);
  fuzzy->addFuzzyOutput(exhaustSpeed);

  // Fuzzy Rules
  FuzzyRuleAntecedent *antecedent1 = new FuzzyRuleAntecedent();
  antecedent1->joinWithAND(noLeak, cold);
  FuzzyRuleConsequent *consequent1 = new FuzzyRuleConsequent();
  consequent1->addOutput(exhaustLow);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, antecedent1, consequent1);
  fuzzy->addFuzzyRule(fuzzyRule1);

  FuzzyRuleAntecedent *antecedent2 = new FuzzyRuleAntecedent();
  antecedent2->joinWithAND(noLeak, warm);
  FuzzyRuleConsequent *consequent2 = new FuzzyRuleConsequent();
  consequent2->addOutput(exhaustMedium);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, antecedent2, consequent2);
  fuzzy->addFuzzyRule(fuzzyRule2);


  FuzzyRuleAntecedent *antecedent3 = new FuzzyRuleAntecedent();
  antecedent3->joinWithAND(noLeak, hot);
  FuzzyRuleConsequent *consequent3 = new FuzzyRuleConsequent();
  consequent3->addOutput(exhaustHigh);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, antecedent3, consequent3);
  fuzzy->addFuzzyRule(fuzzyRule3);

  FuzzyRuleAntecedent *antecedent4 = new FuzzyRuleAntecedent();
  antecedent4->joinWithAND(smallLeak, cold);
  FuzzyRuleConsequent *consequent4 = new FuzzyRuleConsequent();
  consequent4->addOutput(exhaustMedium);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, antecedent4, consequent4);
  fuzzy->addFuzzyRule(fuzzyRule4);

  FuzzyRuleAntecedent *antecedent5 = new FuzzyRuleAntecedent();
  antecedent5->joinWithAND(smallLeak, warm);
  FuzzyRuleConsequent *consequent5 = new FuzzyRuleConsequent();
  consequent5->addOutput(exhaustMedium);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, antecedent5, consequent5);
  fuzzy->addFuzzyRule(fuzzyRule5);

  FuzzyRuleAntecedent *antecedent6 = new FuzzyRuleAntecedent();
  antecedent6->joinWithAND(smallLeak, hot);
  FuzzyRuleConsequent *consequent6 = new FuzzyRuleConsequent();
  consequent6->addOutput(exhaustHigh);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, antecedent6, consequent6);
  fuzzy->addFuzzyRule(fuzzyRule6);

  FuzzyRuleAntecedent *antecedent7 = new FuzzyRuleAntecedent();
  antecedent7->joinWithAND(bigLeak, cold);
  FuzzyRuleConsequent *consequent7 = new FuzzyRuleConsequent();
  consequent7->addOutput(exhaustHigh);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, antecedent7, consequent7);
  fuzzy->addFuzzyRule(fuzzyRule7);

  FuzzyRuleAntecedent *antecedent8 = new FuzzyRuleAntecedent();
  antecedent8->joinWithAND(bigLeak, warm);
  FuzzyRuleConsequent *consequent8 = new FuzzyRuleConsequent();
  consequent8->addOutput(exhaustHigh);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, antecedent8, consequent8);
  fuzzy->addFuzzyRule(fuzzyRule8);

  FuzzyRuleAntecedent *antecedent9 = new FuzzyRuleAntecedent();
  antecedent9->joinWithAND(bigLeak, hot);
  FuzzyRuleConsequent *consequent9 = new FuzzyRuleConsequent();
  consequent9->addOutput(exhaustHigh);
  FuzzyRule *fuzzyRule9 = new FuzzyRule(9, antecedent9, consequent9);
  fuzzy->addFuzzyRule(fuzzyRule9);

  // End of Fuzzy Rules

  // Setup pin mode untuk koneksi dengan L298N
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(ENA, LOW);
}

void loop()
{
  // Baca nilai dari sensor gas MQ-2
  int gasValue = analogRead(gasPin);
  float gasConcentration = map(gasValue, 0, 1023, 0, 1000);

  Serial.print("gasValue: ");
  Serial.println(gasValue);

  Serial.print("GasCocentration: ");
  Serial.println(gasConcentration);


  // Baca nilai dari sensor suhu DHT11
  // float tempValue = dht.readTemperature();
    float tempValue = 21;

  Serial.print("tempValue: ");
  Serial.println(tempValue);

  // Set nilai input ke sistem fuzzy
  fuzzy->setInput(1, gasConcentration);
  fuzzy->setInput(2, tempValue);

  // Proses fuzzy logic
  fuzzy->fuzzify();

  // Defuzzifikasi dan dapatkan nilai kecepatan kipas
  float exhaustSpeedValue = fuzzy->defuzzify(1);
  Serial.print("exhaustSpeedValue: ");
  Serial.println(exhaustSpeedValue);

    int pwmSpeed = (exhaustSpeedValue/255)*100;
// Kontrol kipas menggunakan L298N berdasarkan nilai exhaustSpeedValue
  if (exhaustSpeedValue <= 85) { // Kecepatan rendah
  Serial.println("Sikon 1");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, pwmSpeed); // Misalnya, atur kecepatan menjadi 100 (nilai default rendah)
  } else if (exhaustSpeedValue > 85 && exhaustSpeedValue <= 170) { // Kecepatan sedang
  Serial.println("Sikon 2");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, pwmSpeed); // Misalnya, atur kecepatan menjadi 200 (nilai default sedang)
  } else { // Kecepatan tinggi
  Serial.println("Sikon 3");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, pwmSpeed); // Atur kecepatan maksimum (255) untuk kecepatan tinggi
  }

  // Tunggu sejenak sebelum membaca sensor kembali
  delay(1000); // Ubah delay sesuai kebutuhan
}
