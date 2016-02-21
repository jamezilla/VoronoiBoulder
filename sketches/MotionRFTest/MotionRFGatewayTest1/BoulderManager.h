/*
void printStatus(MySensor &gw, int nodeID, sensor_status state) {

  if (state == SENSOR_MOTION_ACTIVE) {
    Serial.print("Boulder ");
    Serial.print(nodeID);
    Serial.print(" Sensor:");
    Serial.println(" Active");
  }

  if (state == SENSOR_MOTION_DETECTED) {
    Serial.print("Boulder ");
    Serial.print(nodeID);
    Serial.print(" Sensor:");
    Serial.println(" Detected");
    
    if(nodeID == 1) {
      MyMessage msg(nodeID,V_DIMMER);
      gw.send(msg.set(100));
    } else {
      MyMessage msg1(2,V_DIMMER);
      gw.send(msg1.set(0));
      //MyMessage msg2(3,V_DIMMER);
      //gw.send(msg2.set(50));
    }
  }

  if (state == SENSOR_MOTION_ENDED) {
    Serial.print("Boulder ");
    Serial.print(nodeID);
    Serial.print(" Sensor:");
    Serial.println(" Ended");

    if(nodeID == 1) {
      MyMessage msg(nodeID,V_DIMMER);
      gw.send(msg.set(0));
    } else {
      MyMessage msg1(2,V_DIMMER);
      gw.send(msg1.set(100));
      //MyMessage msg2(3,V_DIMMER);
      //gw.send(msg2.set(100));
    }
  }

}
*/
