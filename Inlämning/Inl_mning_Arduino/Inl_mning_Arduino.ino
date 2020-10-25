String cardsWithAccess[10];
int numberOfCards = 0;
  
void setup() 
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop() 
{
  if (Serial.available() > 0)
  {
    String receivedOrder = Serial.readStringUntil('>');
    if (receivedOrder.startsWith("<REMOTEOPENDOOR"))
    {
      digitalWrite(3, HIGH);
      delay(3000);
      digitalWrite(3, LOW);
    }
    else if (receivedOrder.startsWith("<CLEARCARDLIST"))
    {
      for (int i = 0; i < numberOfCards; i++)
      {
        cardsWithAccess[i] = "";
      }
      numberOfCards = 0;
    }
    else if (receivedOrder.startsWith("<ADDCARD"))
    {
      String cardNumber = receivedOrder.substring(8);
      cardsWithAccess[numberOfCards] = cardNumber;
      numberOfCards += 1;
    }
    else if (receivedOrder.startsWith("<SCANCARD"))
    {
      String cardNumber  = receivedOrder.substring(9);
      bool access = false;
      for (int i = 0; i < numberOfCards; i++)
      {
        if (cardsWithAccess[i] == cardNumber)
        {
          digitalWrite(3, HIGH);
          delay(3000);
          digitalWrite(3, LOW);
          access = true;
        }
      }
      if (access == false)
      {
        digitalWrite(2, HIGH);
        delay(3000);
        digitalWrite(2, LOW);
      }
    }
  }
}
