class SoundFolder
{
  private:
    char prefix;
    int count;
    
  public:
    SoundFolder(char prefix)
    {
      this->prefix = prefix;
      count = 0;
    }    
  
    char* GetRandomSoundfile()
    {
      if(count == 0)
        return "";
      int selection = random(0, count);
      char res[8];
      Serial.println("GetRandomSoundfile: Third");
      res[0] = prefix;
      res[1] = 48 + selection / 100;
      res[2] = 48 + (selection % 100) / 10;
      res[3] = 48 + selection % 10;
      Serial.println("GetRandomSoundfile: Four");
      res[4] = '.';
      res[5] = 'w';
      res[6] = 'a';
      res[7] = 'v';
      return res;
    }

    void Add(char* file)
    {
      if(file[0] != prefix)
      {
        Serial.print("Added file ");
        Serial.print(file);
        Serial.print(" to the wrong folder: "); 
        Serial.println(prefix);
      }
      int number = 
        (file[1] - 48) * 100 + 
        (file[2] - 48) * 10 + 
        (file[3] - 48) * 1; 
      if(number + 1 > count)
        count = number + 1;
    }
};
