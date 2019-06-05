class SoundFolder
{
  private:
    byte count;
    
  public:
    char prefix;
    SoundFolder(char prefix, byte count)
    {
      this->prefix = prefix;
      this->count = count;
    }    
  
    int GetRandomSoundfile()
    {
      if(count == 0)
        return "";
        Serial.print("count");
      Serial.println(count);
      int erg = random(0, count);
      return erg;
    }
};
