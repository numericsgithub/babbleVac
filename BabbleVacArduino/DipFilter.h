class DipFilter
{
  private:
    int pitchStartValue;
    int lastFeed;
    int direction;

    private void ChangeDirection(int value)
    {
      if(value < 0)
        direction = -1;
      else direction = 1;
    }
  public:
    DipFilter()
    {
      pitchStartValue = 0;
      lastFeed = 0;
      direction = 0;
      isReady = false;
    }

    int Empty()
    {
    }

    bool Feed(int value)
    {
      switch(direction)
      {
        case 0: 
        {
          if(value == 0)
            return false;
          pitchStartValue = 0;
          ChangeDirection(value);
        } break;
      }
    }
};

