//Sad engine. Part of basic classes and functions.
#include <iostream>
#include <fstream>
#include <math.h>
#include "menu.h"
#define PI 3.14159265

using namespace std;

//--------------------------------------------------------------------------
//timer class
class timer{
       unsigned int t_counter; //timer counter
       
public:
       void reset(){ //reset timer
            t_counter=0;
       }
       void set(unsigned int new_time){ //set timer
            t_counter = new_time;
       }
       
       void inc(){ //increment timer
            t_counter++;
       }
       
       unsigned int get(){ //get current time
           return t_counter;
       }
};
//--------------------------------------------------------------------------
class self_timing: public timer{
       unsigned int max_itt;
       int old_time;
       int step;
public:
       self_timing():max_itt(10),old_time(0),step(0){
           reset();
       }
       
       void miliseconds(int time_seconds){
           if((abs((int)(get()-1000))<10000)&&(get()-1000>1000)) step=1000;
           else if((abs((int)(get()-1000))<1000)&&(get()-1000>100)) step=100;
           else if((abs((int)(get()-1000))<100)&&(get()-1000>10)) step=10;
           else if(abs((int)(get()-1000))<10) step=1;
      
           //step=(get()-1000)/10;
      
           if(time_seconds!=old_time){
              cout<<get()<<' '<<max_itt<<' '<<step<<' '<<abs((int)(get()-1000))<<endl;
              if(get()>1000) max_itt+=step; 
              else if((get()<999) && (max_itt>=step/10)) max_itt-=step/10;
              old_time=time_seconds;
              reset();
           }
      
           for(int i=0;i<max_itt; i++){
                  Sleep(0);
           } 
       }      
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//main object class
class main_object{ // main object in SAD engine. I has properties like objects in real life.
      float x,y,z; //coordinates of the object in the game
      float angle_x,angle_y,angle_z; //angles by all axis
      float mass; //mass of the object
      float lenght, width, height; //dimensions
      
public:
       // all constructors
      main_object() 
      : x(0), y(0), z(0), angle_x(0), angle_y(0), angle_z(0), mass(0)  //Initialise object in center of coord system
      {}
      
      main_object(float init_x, float init_y, float init_z) //Initialise object with coords , without any rotation
      : x(init_x), y(init_y), z(init_z) 
      {}
      
      main_object( //Init object with every property , without dimensions and mass
          float init_x, 
          float init_y, 
          float init_z,
          float init_angle_x,
          float init_angle_y,
          float init_angle_z)
      : x(init_x),
        y(init_y), 
        z(init_z),
        angle_x(init_angle_x),
        angle_y(init_angle_y), 
        angle_z(init_angle_z)
      {}     
      
      main_object( //init object with everything
          float init_x, 
          float init_y, 
          float init_z,
          float init_angle_x,
          float init_angle_y,
          float init_angle_z,
          float init_mass) 
      : x(init_x),
        y(init_y), 
        z(init_z),
        angle_x(init_angle_x),
        angle_y(init_angle_y), 
        angle_z(init_angle_z),
        mass(init_mass)
      {}
      
      //getters and setters

      float get_x(){
           return x;
      }
      
      float get_y(){
           return y;
      }

      float get_z(){
           return z;
      }
      
      float get_angle_x(){
           return angle_x;
      }
      
      float get_angle_y(){
           return angle_y;
      }

      float get_angle_z(){
           return angle_z;
      }
      
      float get_mass(){
           return mass;
      }      
      
      void set_x(float new_x){
           x=new_x;
      }
      
      void set_y(float new_y){
           y=new_y;
      }
      
      void set_z(float new_z){
           z=new_z;
      }
      
      void set_angle_x(float new_angle_x){
           angle_x=new_angle_x;
      }
      
      void set_angle_y(float new_angle_y){
           angle_y=new_angle_y;
      }
      
      void set_angle_z(float new_angle_z){
           angle_z=new_angle_z;
      }
      
      void set_mass(float new_mass){
           mass=new_mass;
      }
      
       float get_lenght(){
           return lenght;
      }
      
      void set_lenght(float new_lenght){
           lenght=new_lenght;
      }
      
       float get_width(){
           return width;
      }
      
      void set_width(float new_width){
           width=new_width;
      }
      
       float get_height(){
           return height;
      }
      
      void set_height(float new_height){
           height=new_height;
      }
      
      //physics of the object
      
      void phys_fall(float Vo,float G,int timer){ //Free falling physics
           z+=Vo/1000 - (G*0.000001*timer)/2;  
      }
};

class camera: public main_object{ //The camera in SAD engine world
      float elev;
public:
       float get_elevation(){ //Gets the current elevation of the camera.
             return elev;
       }
       
      void set_angles(float azimuth,float elevation){ //set angles of the camera
           set_angle_z(azimuth);
           set_angle_y(elevation*sin(azimuth*PI/180));
           set_angle_x(elevation*cos(azimuth*PI/180));
           elev = elevation;
      }
      void set_coords(float new_x,float new_y, float new_z){ // set coords of the camera
           set_x(new_x);
           set_y(new_y);
           set_z(new_z);              
      }
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//spectator class

class spectator: public camera{
public:
       void move_forward(float speed){
            set_y(get_y()+(speed/1000)*cos(get_angle_z()*PI/180)); 
            set_x(get_x()+(speed/1000)*sin(get_angle_z()*PI/180));
       }
       
       void move_back(float speed){
            set_y(get_y()-(speed/1000)*cos(get_angle_z()*PI/180)); 
            set_x(get_x()-(speed/1000)*sin(get_angle_z()*PI/180));            
       }
       
       void move_left(float speed){
            set_y(get_y()+(speed/1000)*sin(get_angle_z()*PI/180)); 
            set_x(get_x()-(speed/1000)*cos(get_angle_z()*PI/180));            
       }
       
       void move_right(float speed){
            set_y(get_y()-(speed/1000)*sin(get_angle_z()*PI/180)); 
            set_x(get_x()+(speed/1000)*cos(get_angle_z()*PI/180));            
       }
       
       void move_up(float speed){
            set_z(get_z()+(speed/1000));           
       }
       
       void move_down(float speed){
            set_z(get_z()-(speed/1000));           
       }
       
};

struct f_point{
    int v, t, n;   
};

struct face{
    f_point a,b,c;      
};

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//model class
class model{ //Model with coords and algorithm to draw it with render.
//All values are public , because It's good to have direct access
public:
    face* f;
    float* arr_v; //array with vertices (coords) of the model
    float* arr_vt; //array with texture coords
    float* arr_vn; //array with normals
    int* arr_f; //array with algorithm to match the vertices
    int num_f, num_v, num_vt, num_vn; //number of polygons , number of vertices , number of texture coords, number of normals
    
    model():num_f(0), num_v(0), num_vt(0), num_vn(0){} //Initialise empty model
};
