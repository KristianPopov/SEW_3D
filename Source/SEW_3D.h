#include "engine.h"
#include <string>
#include <sstream>

template <typename T>
std::string to_string(const T& value)
{
    std::ostringstream s;
    s << value;
    return s.str();
} 

int BuildCircle(float diameter,float step,float* &points){
     points = (float*)malloc(sizeof(float));
     
     int counter=0;
     
     for(float i=0; i<360;i+=step){
             points=(float*)realloc(points,(counter+2)*sizeof(float));
             points[counter]=cos(i*PI/180)*diameter*0.5*0.001;
             points[counter+1]=sin(i*PI/180)*diameter*0.5*0.001;
             counter+=2;
     }
     return counter;
}

int BuildRing(float inner_d,float outer_d,float step,float* &points){
     int counter = BuildCircle(outer_d,step,points);
     float* temp_points;
     int counter2 = BuildCircle(inner_d,step,temp_points);
     
     int temp;
     
     for(temp=0; temp<counter2; temp++){
          points=(float*)realloc(points,(temp+counter+1)*sizeof(float));   
          points[counter+temp]=temp_points[temp];       
     }
     
     return temp+counter;
}

struct point_3d{
    float x, y, z;
};

struct data{
    int num;
    char* name_branch;
    char* name_start_p;
    char* name_end_p;
    float hor_length;
    char* cond_d;
    float slope;
    float start_z_terrain;
    float end_z_terrain;
    float start_z;
    float end_z;
    float start_x;
    float start_y;
    float end_x;
    float end_y;
    float tube_inner_d;
    float tube_outer_d;
    float tube_thickness;
};

class pipe: public model{
    char* clone_name;
    float inner_d,outer_d;
    float thickness;
    float abs_length, hor_length;
    float slope;
    float top_angle;
    float start_x,start_y,start_z;
    float end_x,end_y,end_z;
    float shaft_d;
    
public:
    float* start_shaft_points;
    float* end_shaft_points;
    int num_v_sp;
    
    pipe(){}
       
    char* get_clone_name(){
          return clone_name;      
    }   
       
    void set_clone_name(char* new_clone_name){
         clone_name = new_clone_name;
    }
    
    float get_inner_d(){
          return inner_d;      
    }
    
    void set_inner_d(float new_inner_d){
          inner_d = new_inner_d;     
    }
    
    float get_outer_d(){
          return inner_d;      
    }
    
    void set_outer_d(float new_outer_d){
          outer_d = new_outer_d;     
    }    
    
    float get_thickness(){
          return thickness;      
    }
    
    float get_abs_length(){
          return abs_length;      
    }
    
    float get_hor_length(){
          return abs_length;      
    }   
    
    float get_slope(){
          return slope;      
    }
    
    float get_top_angle(){
          return top_angle;      
    }
    
    void set_start_x(float new_start_x){
          start_x = new_start_x;     
    }
    
    float get_start_x(){
          return start_x;      
    }
    
    void set_start_y(float new_start_y){
          start_y = new_start_y;     
    }
    
    float get_start_y(){
          return start_y;      
    }
    
    void set_start_z(float new_start_z){
          start_z = new_start_z;     
    }
    
    float get_start_z(){
          return start_z;      
    }
    
    float get_end_x(){
          return end_x;      
    }
    
    void set_end_y(float new_end_y){
          end_y = new_end_y;     
    }
    
    float get_end_y(){
          return end_y;      
    }
    
    void set_end_z(float new_end_z){
          end_z = new_end_z;     
    }
    
    float get_end_z(){
          return end_z;      
    }           
    
    void build_pipe(
         char* new_clone_name, 
         float new_inner_d, 
         float new_outer_d,
         float new_hor_length,
         float new_slope,  
         float new_start_x,
         float new_start_y,
         float new_start_z,
         float new_end_x,
         float new_end_y,
         float new_end_z,
         float new_shaft_d)     
    {
         clone_name = new_clone_name;
         inner_d = new_inner_d;
         outer_d = new_outer_d;
         thickness = outer_d - inner_d;
         hor_length = new_hor_length;
         slope = new_slope;
         start_x = new_start_x;
         start_y = new_start_y;
         start_z = new_start_z;
         end_x = new_end_x;
         end_y = new_end_y;
         end_z = new_end_z;
         abs_length = sqrt(pow(start_x-end_x,2)+pow(start_y-end_y,2)+pow(start_z-end_z,2));
         top_angle = acos((end_x - start_x)/hor_length) * 180.0/PI;
         if(new_shaft_d>=500) shaft_d=new_shaft_d;
         else shaft_d=new_shaft_d;
         
         cout<<"Top angle"<<top_angle<<endl;
         cout<<"Absolute/Horisontal: "<<abs_length<<' '<<hor_length<<endl;
         
         //cout<<"Leght: "<<sqrt(pow(start_x-end_x,2)+pow(start_y-end_y,2))<<endl;
         
         float* temp_points;
         float* temp_points2;
         int num_v_temp;
         
         int step = (int)(360/outer_d*15);
         if (step==0) step=1;
         
         cout<<"Step:"<<step<<endl;
         
         num_v=BuildRing(inner_d,outer_d,(float)step,temp_points);
         num_v_temp=BuildRing(inner_d,outer_d,(float)step,temp_points2);
         for(int i=0; i<num_v_temp; i++){
                  temp_points=(float*)realloc(temp_points,(i+num_v+1)*sizeof(float));   
                  temp_points[num_v+i]=temp_points2[i];       
         }
         
         num_v+=num_v_temp;

         arr_v = (float*)malloc(num_v/2*3*sizeof(float));
         arr_f = (int*)malloc(sizeof(int));
         f = (face*)malloc(sizeof(face));
         start_shaft_points = (float*)malloc(num_v/8*3*sizeof(float));
         end_shaft_points = (float*)malloc(num_v/8*3*sizeof(float));
         
         float slope_deg = asin((start_z-end_z)/abs_length)* 180.0/PI;
         float slope_rad = asin((start_z-end_z)/abs_length);
         float distance;
         cout<<"Slope deg: "<<slope_deg<<endl;
         
         float rot_angle;
         if(end_y<start_y) rot_angle = -top_angle;
         else rot_angle = top_angle;
         
          
         num_v/=2; 
         
         arr_vn = (float*)malloc((num_v/2+2)*3*sizeof(float));
         num_vn = num_v/2+2;
          
         for(int i=0; i<num_v; i++){
                 if(i<num_v/2){
                     arr_v[i*3+1]=shaft_d*0.0005-0.2;
                     //arr_vn
                     if(i<num_v/4){
                         
                         arr_vn[i*3]=cos((i*step)*(PI/180))*-sin(rot_angle*PI/180);
                         if(start_y>end_y) arr_vn[i*3+1]= -sin((i*step)*(PI/180))*sin(slope_rad)+cos((i*step)*(PI/180))*cos(rot_angle*PI/180);
                         else arr_vn[i*3+1]= sin((i*step)*(PI/180))*sin(slope_rad)+cos((i*step)*(PI/180))*cos(rot_angle*PI/180);
                         arr_vn[i*3+2]=sin((i*step)*(PI/180))*cos(slope_rad);
                               
                                                 
                         start_shaft_points[i*3]=temp_points[i*2];
                         start_shaft_points[i*3+1]=cos(asin(start_shaft_points[i*3]/(float)(shaft_d*0.005)))*shaft_d*0.005;
                         start_shaft_points[i*3+2]=temp_points[i*2+1]-slope_rad*start_shaft_points[i*3+1]+start_z;
                         
                         distance = sqrt(pow(start_shaft_points[i*3],2)+pow(start_shaft_points[i*3+1],2));
                         
                         start_shaft_points[i*3]=sin(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance + start_x; //x
                         start_shaft_points[i*3+1]=cos(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance +start_y; //y*/
                         
                         end_shaft_points[i*3]=temp_points[i*2];
                         end_shaft_points[i*3+1]=cos(-asin(end_shaft_points[i*3]/(float)(shaft_d*0.005)))*shaft_d*0.005-hor_length;
                         end_shaft_points[i*3+2]=temp_points[i*2+1]-slope_rad*-end_shaft_points[i*3+1]+start_z;
                         
                         distance = sqrt(pow(end_shaft_points[i*3],2)+pow(end_shaft_points[i*3+1],2));
                         
                         end_shaft_points[i*3]=sin(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance + start_x; //x
                         end_shaft_points[i*3+1]=cos(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance +start_y;
                     }
                     else{
                         arr_vn[i*3]=-cos((i*step)*(PI/180))*-sin(rot_angle*PI/180);
                         if(start_y>end_y) arr_vn[i*3+1]= sin((i*step)*(PI/180))*sin(slope_rad)+cos((i*step)*(PI/180))*cos(rot_angle*PI/180);
                         else arr_vn[i*3+1]= -sin((i*step)*(PI/180))*sin(slope_rad)+cos((i*step)*(PI/180))*cos(rot_angle*PI/180);
                         arr_vn[i*3+2]=-sin((i*step)*(PI/180))*cos(slope_rad);
                     }
                 }
                 else arr_v[i*3+1]=abs_length-shaft_d*0.005+0.2;
                 arr_v[i*3]=temp_points[i*2];
                 arr_v[i*3+2]=temp_points[i*2+1];
                 
                 distance=sqrt(pow(temp_points[i*2+1],2) + pow(arr_v[i*3+1],2));
                 if(distance==0) arr_v[i*3+2] = arr_v[i*3+1] = 0;
                 else{ 
                       arr_v[i*3+1]=cos(asin((arr_v[i*3+2])/distance)-slope_rad)*distance;
                       arr_v[i*3+2]=sin(asin((arr_v[i*3+2])/distance)-slope_rad)*distance + start_z;
                 }
                 
                 distance = sqrt(pow(temp_points[i*2],2)+pow(arr_v[i*3+1],2));
                 
                 arr_v[i*3]=sin(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance + start_x; //x
                 arr_v[i*3+1]=cos(acos((temp_points[i*2])/distance)-rot_angle*PI/180) * distance +start_y; //y*/    
         }
         
         arr_vn[(num_vn-2)*3]=-cos(slope_rad)*cos(rot_angle*PI/180); 
         arr_vn[(num_vn-2)*3+1]=-cos(slope_rad)*sin(rot_angle*PI/180);
         arr_vn[(num_vn-2)*3+2]=sin(slope_rad);
         arr_vn[(num_vn-1)*3]=0; 
         arr_vn[(num_vn-1)*3+1]=cos(slope_rad);
         arr_vn[(num_vn-1)*3+2]=-sin(slope_rad);         
         
         for(int i=0; i<num_v; i++){
                 //arr_f = (int*)realloc(arr_f,(i+1)*27*sizeof(int));
                 f = (face*)realloc(f,(i+1)*sizeof(face));   
                 
                         /*f[i*2].a.t = f[i*2+1].a.t = 0;
                         f[i*2].b.t = f[i*2+1].b.t = 0;
                         f[i*2].c.t = f[i*2+1].c.t = 0;*/
                         
                         f[i].a.n = f[i].b.n = f[i].c.n = 0;
                         
                  
                 if(i<num_v/4){
                      
                      if(i==num_v/4-1){
                          f[i].a.v = i;
                          f[i].b.v = 0;
                          f[i].c.v = num_v/2-1;
                          f[i].a.n = f[i].b.n = f[i].c.n = num_vn-2;
                      }
                      else{                          
                          f[i].a.v = i;
                          f[i].b.v = i+1;
                          f[i].c.v = num_v/4+i;
                          f[i].a.n = f[i].b.n = f[i].c.n = num_vn-2;                          
                      }
                        
                 }
                 else if((i>=num_v/4)&&(i<num_v/2)){
                          if(i==num_v/2-1){
                              f[i].a.v = num_v/4;
                              f[i].b.v = i;
                              f[i].c.v = 0;   
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-2;                   
                          }
                          else{
                              f[i].a.v = i+1;
                              f[i].b.v = i;
                              f[i].c.v = i-num_v/4+1;
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-2;
                          }
                          
                          //num_f=(i+1);
                 }
                 
                 else if((i>=num_v/2)&&(i<num_v*3/4)){
                          if(i==num_v*3/4-1){
                              f[i].b.v = i;
                              f[i].a.v = num_v/2;
                              f[i].c.v = num_v-1;
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-1;                             
                          }
                          else{
                              f[i].b.v = i;
                              f[i].a.v = i+1;
                              f[i].c.v = num_v/4+i;
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-1;
                          }
                          
                          //num_f=(i+1);                          
                 }
                 else if(i>=num_v*3/4){
                          
                          if(i==num_v-1){
                              f[i].a.v = i;
                              f[i].b.v = num_v*3/4;
                              f[i].c.v = num_v/2; 
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-1;                             
                          }
                          else{
                              f[i].a.v = i;
                              f[i].b.v = i+1;
                              f[i].c.v = i-num_v/4+1;
                              f[i].a.n = f[i].b.n = f[i].c.n = num_vn-1;
                              
                          }
                          //num_f=(i+1);                      
                 }
                 
                 num_f=(i+1);
         }
         int nf = num_f;
         for(int i=0; i<num_v/4; i++){
                f = (face*)realloc(f,(i+num_f+2)*sizeof(face));
                
                f[i+nf].a.n = f[i+nf].b.n = f[i+nf].c.n = 0;
                                 
                if(i==num_v/4-1){
                     f[i+nf].a.v = i+num_v/4;
                     f[i+nf].b.v = num_v/4;
                     f[i+nf].c.v = num_v-1;
                     f[i+nf].b.n = num_vn/2-1;
                     f[i+nf].a.n = f[i+nf].c.n = num_vn/2+i-1;                     
                }
                else{
                     f[i+nf].a.v = i+num_v/4;
                     f[i+nf].b.v = i+1+num_v/4;
                     f[i+nf].c.v = num_v/2+i+num_v/4;
                     f[i+nf].b.n = num_vn/2+i;
                     f[i+nf].a.n = f[i+nf].c.n = num_vn/2-1+i;
                     
                }
                num_f++;
         }
         nf = num_f;
         for(int i=0; i<num_v/4; i++){
                f = (face*)realloc(f,(i+num_f+2)*sizeof(face)); 
                
                f[i+nf].a.n = f[i+nf].b.n = f[i+nf].c.n = 0;
                
                if(i==num_v/4-1){
                     f[i+nf].a.v = num_v*3/4;
                     f[i+nf].b.v = i+num_v*3/4;
                     f[i+nf].c.v =num_v/4;
                     f[i+nf].b.n = num_vn/2-1+i;
                     f[i+nf].a.n = f[i+nf].c.n = num_vn/2-1;                             
                }
                else{
                     f[i+nf].a.v = i+1+num_v*3/4;
                     f[i+nf].b.v = i+num_v*3/4;
                     f[i+nf].c.v =i-num_v/2+1+num_v*3/4;
                     f[i+nf].b.n = num_vn/2-1+i;
                     f[i+nf].a.n = f[i+nf].c.n = num_vn/2+i;
                } 
                num_f++;                
         }
         nf = num_f;
         for(int i=0; i<num_v/4; i++){
                                 //f[i+nf].a.n = f[i+nf].b.n = f[i+nf].c.n = 0;
                f = (face*)realloc(f,(i+num_f+2)*sizeof(face)); 
                if(i==num_v/4-1){                 
                     f[i+nf].b.v = i;
                     f[i+nf].a.v = 0;
                     f[i+nf].c.v = num_v/2;
                     f[i+nf].b.n = i;
                     f[i+nf].a.n = f[i+nf].c.n = 0; 
                }
                else{                          
                     f[i+nf].b.v = i;
                     f[i+nf].a.v = i+1;
                     f[i+nf].c.v = num_v/2+i+1;
                     f[i+nf].b.n = i;
                     f[i+nf].a.n = f[i+nf].c.n = i+1;                 
                }
                num_f++;                
         }
         nf = num_f;
         for(int i=0; i<num_v/4; i++){
                                 //f[i+nf].a.n = f[i+nf].b.n = f[i+nf].c.n = 0;
                f = (face*)realloc(f,(i+num_f+2)*sizeof(face)); 
                if(i==num_v/4-1){                 
                     f[i+nf].a.v = i+num_v/2;
                     f[i+nf].b.v = num_v/2;
                     f[i+nf].c.v = num_v/4-1;
                     f[i+nf].a.n = f[i+nf].c.n = i; 
                     f[i+nf].b.n = 0; 
                }
                else{                          
                     f[i+nf].a.v = i+num_v/2;
                     f[i+nf].b.v = i+1+num_v/2;
                     f[i+nf].c.v = i;
                     f[i+nf].a.n = f[i+nf].c.n = i; 
                     f[i+nf].b.n = i+1;                     
                }
                num_f++;                
         }
         
         cout<<"nf "<<num_f<<endl;
    }  
};

class shaft: public model{
    float x,y;
    float z_terrain,z_bottom;
    char* name;
    float diameter;
public:
    char* get_name(){
          return name;      
    }   
    
    void set_name(char* new_name){
         name=new_name;
    }   
       
    float get_x(){
          return x;      
    }   
    
    void set_x(float new_x){
         x=new_x;
    }
    
    float get_y(){
          return y;      
    }   
    
    void set_y(float new_y){
         y=new_y;
    } 
    
    float get_z_terrain(){
          return z_terrain;      
    }   
    
    void set_z_terrain(float new_z_terrain){
         z_terrain=new_z_terrain;
    }
    
    float get_z_bottom(){
          return z_bottom;      
    }   
    
    void set_z_bottom(float new_z_bottom){
         z_bottom=new_z_bottom;
    }
    
    float get_diameter(){
          return diameter;      
    }   
    
    void set_diameter(float new_diameter){
         diameter=new_diameter;
    }            

    bool operator==(const shaft& sh){
         return ((this->x==sh.x)&&(this->y==sh.y)&&(this->z_terrain==sh.z_terrain));
    }
    
    void set_params(char* new_name, float new_x, float new_y, float new_z_terrain, float new_diameter){
         name=new_name;
         x=new_x;
         y=new_y;
         z_terrain=new_z_terrain;
         diameter=new_diameter;
    }
};

struct coords_fix{
    long int a;
    double b;  
};

void SepFloat(char* field,coords_fix &cf){
         char* temp_str=(char*)malloc(sizeof(char));
         int k=0;
         while(field[k]!='.'){
             temp_str=(char*)realloc(temp_str,(k+2)*sizeof(char));
             temp_str[k]=field[k];
             k++;                 
         }
         temp_str[k]=0;
         cf.a = atoi(temp_str);
         int temp=k-1;
         k=0;
         free(temp_str);
         temp_str=(char*)malloc(sizeof(char));
         while(field[temp+k]!=0){
             temp_str=(char*)realloc(temp_str,(k+1)*sizeof(char));
             temp_str[k]=field[k+temp+1];
             k++;                 
         }
         cf.b = atof(temp_str);
         free(temp_str);         
} 


int ReadingData(char* file_name,data* &lines,int &progress, float &offset_z){
    ifstream input(file_name);
    string f_line;
    int num_lines=0;
    char* cstr;
    int tabs=0;
    char* field = (char*)malloc(255*sizeof(char));
    lines = (data*)malloc(sizeof(data));
    coords_fix* s_x;
    coords_fix* s_y;
    coords_fix* e_x;
    coords_fix* e_y;
    s_x = (coords_fix*)malloc(sizeof(coords_fix));
    s_y = (coords_fix*)malloc(sizeof(coords_fix));
    e_x = (coords_fix*)malloc(sizeof(coords_fix));
    e_y = (coords_fix*)malloc(sizeof(coords_fix));
    
    long int max_x=0,min_x=2147483647;
    long int max_y=0,min_y=2147483647;
    float max_z=0,min_z=3.40282e+38;
    

    while(getline( input, f_line ) ) {
        cout<<f_line<<'\n';
        cstr = new char [f_line.length()+1];
        strcpy(cstr, f_line.c_str());
        tabs=0;
        
        int j=0;
        lines = (data*)realloc(lines,(num_lines+1)*sizeof(data));

        lines[num_lines].name_branch = (char*)malloc(sizeof(char));
        lines[num_lines].name_start_p = (char*)malloc(sizeof(char));
        lines[num_lines].name_end_p = (char*)malloc(sizeof(char));
        lines[num_lines].cond_d = (char*)malloc(sizeof(char));        

        s_x = (coords_fix*)realloc(s_x,(num_lines+1)*sizeof(coords_fix));
        s_y = (coords_fix*)realloc(s_y,(num_lines+1)*sizeof(coords_fix));
        e_x = (coords_fix*)realloc(e_x,(num_lines+1)*sizeof(coords_fix));
        e_y = (coords_fix*)realloc(e_y,(num_lines+1)*sizeof(coords_fix));    
        for(int i=0; i<f_line.length()+1; i++){
            if(cstr[i]==9){
                field[j]=0;
                
                switch(tabs){
                    case 0: lines[num_lines].num = atoi(field);
                            break;                             
                    case 1: //lines[num_lines].name_branch = (char*)realloc(lines[num_lines].name_branch,strlen(field)*sizeof(char));
                            strcpy(lines[num_lines].name_branch,field);
                            break;
                    case 2: //lines[num_lines].name_start_p = (char*)realloc(lines[num_lines].name_start_p,1000);
                            strcpy(lines[num_lines].name_start_p,field);
                            break;
                    case 3: //lines[num_lines].name_end_p = (char*)malloc((j+1)*sizeof(char));
                            strcpy(lines[num_lines].name_end_p,field);
                            break;
                    case 4: lines[num_lines].hor_length = atof(field);
                            break;
                    case 5: //lines[num_lines].cond_d = (char*)malloc((j+1)*sizeof(char));
                            strcpy(lines[num_lines].cond_d,field);
                            break;
                    case 6: lines[num_lines].slope = atof(field);
                            break;
                    case 7: lines[num_lines].start_z_terrain = atof(field);
                            break;
                    case 8: lines[num_lines].end_z_terrain = atof(field);
                            break;
                    case 9: lines[num_lines].start_z = atof(field);
                            break;
                    case 10: lines[num_lines].end_z = atof(field);
                            break;
                    case 11:SepFloat(field,s_x[num_lines]);
                            //cout<<endl<<s_x[num_lines].a<<'.'<<s_x[num_lines].b<<endl; 
                            break; 
                    case 12:SepFloat(field,s_y[num_lines]);
                            //cout<<s_y[num_lines].a<<'.'<<s_y[num_lines].b<<endl; 
                            break;
                    case 13:SepFloat(field,e_x[num_lines]);
                            //cout<<e_x[num_lines].a<<'.'<<e_x[num_lines].b<<endl; 
                            break; 
                    case 14: SepFloat(field,e_y[num_lines]);
                            //cout<<e_y[num_lines].a<<'.'<<e_y[num_lines].b<<endl<<endl; 
                            break;
                    case 15: lines[num_lines].tube_inner_d = atof(field);
                            break; 
                    case 16: lines[num_lines].tube_outer_d = atof(field);
                            break; 
                    case 17: lines[num_lines].tube_thickness = atof(field);
                            break;                                                                                                                                                                                                                                                                                            
                }
                //free(field);               
                j=0;
                //if(tabs==17) break;
                tabs++;
                //field = (char*)malloc(sizeof(char));
            }
            else{
                 //field = (char*)realloc(field,(j+1)*sizeof(char));
                 if(cstr[i]==',') field[j]='.';
                 else field[j]=cstr[i];
                 j++;
            }
            
        }
        free(cstr);
        
        if(max_x<s_x[num_lines].a) max_x=s_x[num_lines].a; 
        if(max_x<e_x[num_lines].a) max_x=e_x[num_lines].a;
        if(min_x>s_x[num_lines].a) min_x=s_x[num_lines].a;
        if(min_x>e_x[num_lines].a) min_x=e_x[num_lines].a;

        if(max_y<s_y[num_lines].a) max_y=s_y[num_lines].a; 
        if(max_y<e_y[num_lines].a) max_y=e_y[num_lines].a;
        if(min_y>s_y[num_lines].a) min_y=s_y[num_lines].a;
        if(min_y>e_y[num_lines].a) min_y=e_y[num_lines].a;

        if(max_z<lines[num_lines].start_z) max_z=lines[num_lines].start_z; 
        if(max_z<lines[num_lines].end_z) max_z=lines[num_lines].end_z;
        if(min_z>lines[num_lines].start_z) min_z=lines[num_lines].start_z;
        if(min_z>lines[num_lines].end_z) min_z=lines[num_lines].end_z;
        
        num_lines++;
        progress=num_lines;
    } 
    
    cout<<max_x<<' '<<min_x<<endl;
    
    long int offset_x=(min_x+max_x)/2;
    long int offset_y=(min_y+max_y)/2;
    offset_z=(min_z+max_z)/2;
    
    cout<<offset_x<<' '<<offset_y<<endl;
    
    for(int i=0; i<num_lines; i++){
       lines[i].start_x = s_x[i].a - offset_x + s_x[i].b;
       lines[i].end_x = e_x[i].a - offset_x + e_x[i].b;
       lines[i].start_y = s_y[i].a - offset_y + s_y[i].b;
       lines[i].end_y = e_y[i].a - offset_y + e_y[i].b;
       lines[i].start_z -= offset_z;
       lines[i].end_z -= offset_z;
       lines[i].start_z_terrain -= offset_z;
       lines[i].end_z_terrain -= offset_z;
    }
    
    free(s_x);
    free(e_x);
    free(s_y);
    free(e_y);
    input.close();
    return num_lines;
}

model create_SD_shaft(){
      model sdsh;
      sdsh.num_v=0;
      sdsh.arr_v = (float*)malloc(sizeof(float));
      sdsh.arr_vn = (float*)malloc(sizeof(float));
      sdsh.arr_vt = (float*)malloc(sizeof(float));
      sdsh.f = (face*)malloc(sizeof(face));
      
      //int counter=0;
      
      for(float i=0; i<720;i+=15){
             sdsh.arr_v=(float*)realloc(sdsh.arr_v,(sdsh.num_v+3)*sizeof(float));
             sdsh.arr_vn=(float*)realloc(sdsh.arr_vn,(sdsh.num_v+3)*sizeof(float));
             sdsh.arr_v[sdsh.num_v]=cos(i*PI/180)*0.5;
             sdsh.arr_vn[sdsh.num_v]=cos(i*PI/180);
             sdsh.arr_v[sdsh.num_v+1]=sin(i*PI/180)*0.5;
             sdsh.arr_vn[sdsh.num_v+1]=sin(i*PI/180);
             if(i<360)sdsh.arr_v[sdsh.num_v+2]=0;
             else sdsh.arr_v[sdsh.num_v+2]=1;
             sdsh.arr_vn[sdsh.num_v+2]=0;
             sdsh.num_v+=3;
      }
      
      sdsh.arr_vn=(float*)realloc(sdsh.arr_vn,(sdsh.num_v+6)*sizeof(float));
      sdsh.arr_v=(float*)realloc(sdsh.arr_v,(sdsh.num_v+6)*sizeof(float));
      for(int i=1; i<7; i++){
              if(i%3){
                 sdsh.arr_vn[sdsh.num_v+i-1]=0;
              }
              else{
                 sdsh.arr_vn[sdsh.num_v+i-1]=1;
              }
      }
      
      for(int i=0; i<5; i++){
              sdsh.arr_v[sdsh.num_v+i]=sdsh.arr_vn[sdsh.num_v+i]=0;    
      }
      sdsh.arr_v[sdsh.num_v+5]=sdsh.arr_vn[sdsh.num_v+5]=-1;
      sdsh.arr_vn[sdsh.num_v+2]=1;
      sdsh.arr_vt[0]=0;
      sdsh.num_vt=1;
      sdsh.num_v+=6;
       
      sdsh.num_v/=3;
      sdsh.num_vn=sdsh.num_v;
      
      for(int i=0; i<sdsh.num_v/2-1; i++){
           sdsh.f=(face*)realloc(sdsh.f,(i+1)*sizeof(face));
           if(i<sdsh.num_v/2-2){
                sdsh.f[i].a.v=i;
                sdsh.f[i].b.v=i+1;
                sdsh.f[i].c.v=sdsh.num_v-2;
                sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;
           }
           else{
                sdsh.f[i].a.v=i;
                sdsh.f[i].b.v=0;
                sdsh.f[i].c.v=sdsh.num_v-2;   
                sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;            
           }
           sdsh.num_f++; 
      }
      for(int i=sdsh.num_v/2-1; i<sdsh.num_v-2; i++){
           sdsh.f=(face*)realloc(sdsh.f,(i+1)*sizeof(face));

           if(i<sdsh.num_v-3){
                sdsh.f[i].a.v=sdsh.f[i].a.n=i;
                sdsh.f[i].b.v=sdsh.f[i].b.n=i+1;
                sdsh.f[i].c.v=sdsh.f[i].c.n=i-sdsh.num_v/2+1;
                //sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;
           }
           else{
                sdsh.f[i].a.v=sdsh.f[i].a.n=i;
                sdsh.f[i].b.v=sdsh.f[i].b.n=sdsh.num_v/2-1;
                sdsh.f[i].c.v=sdsh.f[i].c.n=sdsh.num_v/2-2; 
                //sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;            
           }
           sdsh.num_f++;              
      }
      int nf=sdsh.num_f;
      for(int i=0; i<sdsh.num_v/2-1; i++){
           sdsh.f=(face*)realloc(sdsh.f,(i+1+nf)*sizeof(face));

           if(i<sdsh.num_v/2-2){
                sdsh.f[i+nf].a.v=sdsh.f[i+nf].a.n=i;
                sdsh.f[i+nf].b.v=sdsh.f[i+nf].b.n=i+1;
                sdsh.f[i+nf].c.v=sdsh.f[i+nf].c.n=i+sdsh.num_v/2;
                //sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;
           }
           else{
                sdsh.f[i+nf].a.v=sdsh.f[i+nf].a.n=i;
                sdsh.f[i+nf].b.v=sdsh.f[i+nf].b.n=0;
                sdsh.f[i+nf].c.v=sdsh.f[i+nf].c.n=sdsh.num_v/2-1; 
                //sdsh.f[i].a.n = sdsh.f[i].b.n = sdsh.f[i].c.n = sdsh.num_vn-1;            
           }
           sdsh.num_f++;              
      }      
      
      cout<<sdsh.num_f<<endl;
      //sdsh.num_f--;
      
      return sdsh;
};
