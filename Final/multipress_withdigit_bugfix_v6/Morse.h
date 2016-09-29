#ifndef MORSE_H_
#define MORSE_H_

#define UNIT_LENGTH 250

//Formula for calculating unit time is
// T = 1200 / W [Where T is the unit time (ms) and W is wpm
int calculateUnitTime(byte wpm)
{
  return 1200 / wpm;
}


static  const struct {const char letter, *code;} MorseMap[] = {
    { 'A', ".-" },
    { 'B', "-..." },
    { 'C', "-.-." },
    { 'D', "-.." },
    { 'E', "." },
    { 'F', "..-." },
    { 'G', "--." },
    { 'H', "...." },
    { 'I', ".." },
    { 'J', ".---" },
    { 'K', ".-.-" },
    { 'L', ".-.." },
    { 'M', "--" },
    { 'N', "-." },
    { 'O', "---" },
    { 'P', ".--." },
    { 'Q', "--.-" },
    { 'R', ".-." },
    { 'S', "..." },
    { 'T', "-" },
    { 'U', "..-" },
    { 'V', "...-" },
    { 'W', ".--" },
    { 'X', "-..-" },
    { 'Y', "-.--" },
    { 'Z', "--.." },
    { ' ', "/" }, //Gap between word, seven units 
      
    { '1', ".----" },
    { '2', "..---" },
    { '3', "...--" },
    { '4', "....-" },
    { '5', "....." },
    { '6', "-...." },
    { '7', "--..." },
    { '8', "---.." },
    { '9', "----." },
    { '0', "-----" },
    { '.', "·–·–·–" },
    { ',', "--..--" },
    { '?', "..--.." },
    { '!', "-.-.--" },
    { ':', "---..." },
    { ';', "-.-.-." },
    { '(', "-.--." },
    { ')', "-.--.-" },
    { '"', ".-..-." },
    { '@', ".--.-." },
    { '&', ".-..." },
};



class Morse {
  public:
    Morse(byte ledPin, byte buzzerPin, unsigned int buzzFreq) 
    {
      ledPin = ledPin;
      buzzerPin = buzzerPin;
      buzzFreq = buzzFreq;
    }

    void begin(void)
    {
      pinMode(ledPin, OUTPUT);
      pinMode(buzzerPin, OUTPUT);
      digitalWrite(ledPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }

    String encode(String str)
    {
      morse_word = "";
      size_t i;
      size_t j;

      for (i = 0; i < str.length(); ++i)
      {
        for (j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j)
        {
          if (toupper(str[i]) == MorseMap[j].letter)
          {
            morse_word += MorseMap[j].code;
            break;
          }
        }
        morse_word += " ";
      }
      return morse_word;
    }

    String decode(String morse)
    {
      text_word = "";
      
      int lastPos = 0;
      int pos = morse.indexOf(' ');
      while( lastPos <= morse.lastIndexOf(' ') )
      {    
        for( int i = 0; i < sizeof MorseMap / sizeof *MorseMap; ++i )
        {
          if( morse.substring(lastPos, pos) == MorseMap[i].code )
          {
            text_word += MorseMap[i].letter;
          }
        }
    
        lastPos = pos+1;
        pos = morse.indexOf(' ', lastPos);
        
        // Handle white-spaces between words (7 spaces)
        while( morse[lastPos] == ' ' && morse[pos+1] == ' ' )
        {
          pos ++;
        }
      }

       return text_word;
    }

    void playCode(String mw)
    {
      for(int i=0; i<=mw.length(); i++)
      {
        Serial.print("Dit Dah: " + String(mw[i]));
        switch( mw[i] )
        {
          case '.': //dit
            digitalWrite( ledPin, HIGH );
            delay( UNIT_LENGTH );
            digitalWrite( ledPin, LOW );
            delay( UNIT_LENGTH );
              
            break;
    
          case '-': //dah
            digitalWrite( ledPin, HIGH );
            delay( UNIT_LENGTH*3 );
            digitalWrite( ledPin, LOW );
            delay( UNIT_LENGTH );
              
            break;
    
          case ' ': //gap
            delay( UNIT_LENGTH );
        }
      }
    }

  
  private:
    byte ledPin;
    byte buzzerPin;
    unsigned int buzzFreq;
    String text_word;
    String morse_word;
    

};

#endif
