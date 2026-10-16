#include "config.h"
#include "AppLight.h"

AppLight app;

void setup() {
  app.begin();
}

void loop() {
  app.tick();
}