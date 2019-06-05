class MaxFilter
{
  private:
    int max1;
    int max2;
  public:
    HighFilter()
    {
      max1 = 0;
      max2 = 0;
    }

    int Get()
    {
      int result = min(max1 + (long)max2, 32767); // Prevent overflow
      max1 = 0;
      max2 = 0;
      return result;
    }

    void Feed(int value)
    {
      if(value > max1)
      {
        max2 = max1;
        max1 = value;
        return;
      }
      if(value > max2)
      {
        max2 = value;
      }
    }
};

