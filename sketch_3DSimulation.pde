/* Description:
  * The program recieve by serial communication xyz points.
  * The points are then plot on 3D simulation.
  * Written by Elad Soll, mail: elad.soll@s.afeka.ac.il
*/

import processing.serial.*;
import peasy.*;
//Objects:
Serial serial;
PeasyCam cam;
//Variables:
final float angleIncrement = 0.1f;
ArrayList<PVector> pointList;
final int baudRate = 115200;
void setup(){
  size(800,640,P3D);
  colorMode(RGB,255,255,255);
  noSmooth();
  pointList = new ArrayList<PVector>();//List which saves coordinates.
  //PeasyCam
  cam = new PeasyCam(this,800);
  cam.rotateZ(-3.1415/4);
  cam.rotateX(-3.1415/4);
  //Serial Port communication condition
  try{
    if(Serial.list().length == 0){
      println("No serial device connected");
    }
    else
      serial = new Serial(this,Serial.list()[0],baudRate);
    }
    catch(Exception error){
    println("Not able to connect to serialPort (error:"+error.getClass().getName()+" " + error.getMessage() + ")");
    exit();
    }
}
void draw(){
  if(mousePressed && (mouseButton == LEFT));
  perspective();
  background(33);
  stroke(255,255,255);
  fill(50);
  ellipse(0,0,10,10);
  String serialString = serial.readStringUntil('\n');
  if(serialString != null){
    String[] coordinates = split(serialString, ' ');
    if(coordinates.length == 3){
      pointList.add(new PVector(float(coordinates[0]),float(coordinates[1]),float(coordinates[2])));
    }
  }
  //Draw point cloud
  for(int idx = 0; idx < pointList.size(); idx++){
    PVector v = pointList.get(idx);
    if(idx == pointList.size() -1){
      stroke(255,15,15);
      line(0, 0, 0, v.x, -v.z, -v.y);//fov red line
    }
      stroke(255-v.z, 255-v.y, 255-v.x);
      point(v.x, -v.z, -v.y);
  }
}
void keyRealeased(){
  if(key == 'c'){
    pointList.clear();
  }
  else if(key == CODED){
    if(keyCode == UP){
      cam.rotateX(angleIncrement);
    }
    else if(key == DOWN){
      cam.rotateX(-angleIncrement);
    }
    else if(key == LEFT){
      cam.rotateY(angleIncrement);
    }
    else if(key == RIGHT){
      cam.rotateY(-angleIncrement);
    }
  }
}
