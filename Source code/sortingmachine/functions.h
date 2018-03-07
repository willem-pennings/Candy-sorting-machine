#ifndef FUNCTIONS_H
  #define FUNCTIONS_H
  
  void initializeTCS34725(void);
  void initializeDRV2605(void);
  void initializePins(void);
  double arrayMax(double array[], int sizeOf);
  double arrayMin(double array[], int sizeOf);
  int getStepDistance(ItemColor nextcolor, ItemColor prevcolor);
  void sendSyncVariables();
  void receive(int numBytes);
  int shortestPath();

#endif
