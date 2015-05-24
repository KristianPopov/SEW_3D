/**************************
 * Includes
 *
 **************************/

//#include <iostream>
#include <windows.h>
#include <process.h>
#include <gl/gl.h>
#include "GLINIT.h"
#include "dds.h"
#include <gl/glu.h>
#include "SEW_3D.h"
//#include <math.h>
//#include "engine.h"

using namespace std;

#define PI 3.14159265

/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);
void Phys_Thread(void* PARAMS);


float theta = 0.0f;
float colorBlue[] = { 0.0f, 0.0f, 1.0f, 0.5f };
float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float ambientLight0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float diffuseLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float specularLight0[] = { 2.0f, 2.0f, 2.0f, 1.0f };
float position0[] = { 10, 10, 0,  1 };


float test_x=0,test_y=0;
float test_width=0.1,test_height=0.1;

bool click=false, any_btn=false;
bool sim=0;
bool o_cmd=0, e_cmd=0;
int x=0,y=0;
//int fx=0,fy=0;
//float rx=0,ry=0;
bool move_forward=0, move_back=0, move_right=0, move_left=0, move_up=0, move_down=0, cam_ctrl=0, ctrl=0;
//float player_x=0 , player_y=0 , player_z=0;
float global_scale=1.0;
float offset_z=0;
float azth=0,elev=0;
POINT pt;

char* conv_c_str = (char*)malloc(8);
string conv_str;
string help_message;

spectator spec;

SYSTEMTIME phystime;
SYSTEMTIME FPStime;
int old_time=0;
self_timing self_t;
int FPS=0,FPS_=0;
char show_fps=0;
float cam_speed = 200;

    int num_pipes=0;// = ReadingData(FileName,lines);
    int num_sh=0;
    
    model shaft_sd = create_SD_shaft();
    bool opened_file=false;
    pipe* pipes;
    shaft* shafts;
    int progress=0;
    data* lines;// = OpenFile(num_pipes);

void BuildSewage(int num_pipes,int &num_sh,pipe* &pipes,shaft* &shafts,data* lines,int &progress){
    shaft temp_sh; 
    num_sh=0;
    float temp_d;
    
    pipes = new pipe[num_pipes];
    shafts = (shaft*)malloc(sizeof(shaft));
     
    for(int i=0; i<num_pipes; i++){
            pipes[i].build_pipe(lines[i].name_branch,lines[i].tube_inner_d,lines[i].tube_outer_d,lines[i].hor_length,lines[i].slope,lines[i].start_x,lines[i].start_y,lines[i].start_z,lines[i].end_x,lines[i].end_y,lines[i].end_z,100);
            if(lines[i].tube_inner_d>1500) temp_d=3000;
            else if(lines[i].tube_inner_d>1000) temp_d=2000;
            else if(lines[i].tube_inner_d>600) temp_d=1500;
            else temp_d=1000;
            
            if(num_sh>0){
                int k;        
                for(k=0; k<num_sh; k++){
                        temp_sh.set_params(lines[i].name_start_p, lines[i].start_x, lines[i].start_y, lines[i].start_z_terrain, temp_d);
                        temp_sh.set_z_bottom(lines[i].start_z);
                        if(shafts[k]==temp_sh){
                            if(shafts[k].get_z_bottom()>temp_sh.get_z_bottom()) shafts[k].set_z_bottom(temp_sh.get_z_bottom());
                            if(shafts[k].get_diameter()<temp_sh.get_diameter()) shafts[k].set_diameter(temp_sh.get_diameter());
                            break;
                        }
                }
                if(k==num_sh){
                      shafts = (shaft*)realloc(shafts,(num_sh+1)*sizeof(shaft));
                      shafts[num_sh].set_params(lines[i].name_start_p, lines[i].start_x, lines[i].start_y, lines[i].start_z_terrain, temp_d);
                      shafts[num_sh].set_z_bottom(lines[i].start_z);
                      num_sh++;                     
                }
                for(k=0; k<num_sh; k++){
                        
                        temp_sh.set_params(lines[i].name_end_p, lines[i].end_x, lines[i].end_y, lines[i].end_z_terrain, temp_d);
                        temp_sh.set_z_bottom(lines[i].end_z);
                        if(shafts[k]==temp_sh){
                            if(shafts[k].get_z_bottom()>temp_sh.get_z_bottom()) shafts[k].set_z_bottom(temp_sh.get_z_bottom());
                            if(shafts[k].get_diameter()<temp_sh.get_diameter()) shafts[k].set_diameter(temp_sh.get_diameter());
                            break;
                        }
                }
                if(k==num_sh){
                      shafts = (shaft*)realloc(shafts,(num_sh+1)*sizeof(shaft));
                      shafts[num_sh].set_params(lines[i].name_end_p, lines[i].end_x, lines[i].end_y, lines[i].end_z_terrain, temp_d);
                      shafts[num_sh].set_z_bottom(lines[i].end_z);
                      num_sh++;                     
                }
            }
            else{
                 shafts[0].set_params(lines[i].name_start_p, lines[i].start_x, lines[i].start_y, lines[i].start_z_terrain, temp_d);
                 shafts[0].set_z_bottom(lines[i].start_z);
                 num_sh++;
            }
       progress=i+1;
    }    
}


data* OpenFile(int &num_pipes,int &progress){
      num_pipes=0;
      data* lines;
      HWND hWnd;
      char FileName[512];
      OPENFILENAME ofn={0};    
      ofn.lStructSize=sizeof(OPENFILENAME);
      //ofn.Flags=OFN_ALLOWMULTISELECT|OFN_EXPLORER;
      ofn.lpstrFilter="Tab Delimated (*.txt) \0*.txt\0";
      ofn.lpstrFile=FileName;
      ofn.lpstrFile[0] = '\0';
      ofn.nMaxFile=MAX_PATH;
      ::GetOpenFileName(&ofn);
      MessageBox(hWnd, FileName, "Test", MB_OK);
      if(strlen(FileName)) num_pipes = ReadingData(FileName,lines,progress,offset_z);
      else{ 
            progress=0;
            num_pipes=0;
      }
      
      return lines;
}

void ExportFile(int num_pipes,int num_sh,pipe* pipes, shaft* shafts, model SD_shaft){
      HWND hWnd;
      char FileName[512];
      string FName;
      OPENFILENAME ofn={0};    
      ofn.lStructSize=sizeof(OPENFILENAME);
      //ofn.Flags=OFN_ALLOWMULTISELECT|OFN_EXPLORER;
      ofn.lpstrFilter="Wavefront .obj (*.obj) \0*.obj\0";
      ofn.lpstrFile=FileName;
      ofn.lpstrFile[0] = '\0';
      ofn.nMaxFile=MAX_PATH;
      ::GetSaveFileName(&ofn);
      MessageBox(hWnd, FileName, "Test", MB_OK);
      ofstream file;
      FName = to_string(FileName);
      //FName = FName + ".obj";
      file.open (FName.c_str());
      file << "#This file is generated by SEW-3D - Storm sewage 3D Vusualisation" <<endl;
      file << "#All rights reserved (c) 2015 Kristian Popov e_mail: kristian@popovbg.eu\n" <<endl;
      
      int curr_v=1;
      int curr_vn=1;
      
      for(int k=0; k<num_pipes; k++){
          for(int i=0; i<pipes[k].num_v; i++){
                  file<<"v "<<pipes[k].arr_v[i*3+1]<<' '<<pipes[k].arr_v[i*3+2]<<' '<<pipes[k].arr_v[i*3]<<endl;
          }
                 
      }
      for(int k=0; k<num_sh; k++){
          for(int i=0; i<SD_shaft.num_v; i++){
                  file<<"v "<<SD_shaft.arr_v[i*3+1]*shafts[k].get_diameter()*0.001+shafts[k].get_y()
                  <<' '<<SD_shaft.arr_v[i*3+2]*(shafts[k].get_z_terrain()-shafts[k].get_z_bottom()+0.5)+shafts[k].get_z_bottom()-0.5
                  <<' '<<SD_shaft.arr_v[i*3]*shafts[k].get_diameter()*0.001+shafts[k].get_x()<<endl;
          }
      }              
      
      file<<"\nvt 0 0\n\n";
      
      for(int k=0; k<num_pipes; k++){
          for(int i=0; i<pipes[k].num_vn; i++){
                  file<<"vn "<<pipes[k].arr_vn[i*3+1]<<' '<<pipes[k].arr_vn[i*3+2]<<' '<<pipes[k].arr_vn[i*3]<<endl;
          }
      }
      
          for(int i=0; i<SD_shaft.num_vn; i++){
                  file<<"vn "<<SD_shaft.arr_vn[i*3+1]<<' '<<SD_shaft.arr_vn[i*3+2]<<' '<<SD_shaft.arr_vn[i*3]<<endl;
          }      
      
      file<<endl;
      
      for(int k=0; k<num_pipes; k++){
          for(int i=0; i<pipes[k].num_f; i++){
                  file<<"f "<<pipes[k].f[i].a.v+curr_v<<'/'<<1<<'/'<<pipes[k].f[i].a.n+curr_vn;
                  file<<' '<<pipes[k].f[i].b.v+curr_v<<'/'<<1<<'/'<<pipes[k].f[i].b.n+curr_vn;
                  file<<' '<<pipes[k].f[i].c.v+curr_v<<'/'<<1<<'/'<<pipes[k].f[i].c.n+curr_vn<<endl;
          }
          curr_v+=pipes[k].num_v;
          curr_vn+=pipes[k].num_vn;
      }
      
      for(int k=0; k<num_sh; k++){
          for(int i=0; i<SD_shaft.num_f; i++){
                  file<<"f "<<SD_shaft.f[i].a.v+curr_v<<'/'<<1<<'/'<<SD_shaft.f[i].a.n+curr_vn;
                  file<<' '<<SD_shaft.f[i].b.v+curr_v<<'/'<<1<<'/'<<SD_shaft.f[i].b.n+curr_vn;
                  file<<' '<<SD_shaft.f[i].c.v+curr_v<<'/'<<1<<'/'<<SD_shaft.f[i].c.n+curr_vn<<endl;
          }
          curr_v+=SD_shaft.num_v;
      }
      
      file.close();
      /*if(strlen(FileName)) num_pipes = ReadingData(FileName,lines,progress);
      else{ 
            progress=0;
            num_pipes=0;
      }*/
}

void CameraUse(camera cam){
     glRotatef (cam.get_angle_z(), 0.0f, 1.0f, 0.0f);
     glRotatef (cam.get_angle_y(), 0.0f, 0.0f, 1.0f);
     glRotatef (cam.get_angle_x(), 1.0f, 0.0f, 0.0f);
     glTranslatef(-cam.get_x(),-cam.get_z(),cam.get_y());
}

void XY2Polar(RECT ds,int scr_x,int scr_y, float &azimuth,float &elevation,unsigned int res_x, unsigned int res_y, float sens){
     
           static int fx=0,fy=0;
           int x=scr_x-((ds.right/2)-res_x/2)+fx;
           int y=scr_y-((ds.bottom/2)-res_y/2)+fy;
           
           if(scr_x-((ds.right/2)-(res_x/2))>res_x/2+199){
                fx+=scr_x-ds.right/2;
                SetCursorPos(ds.right/2,scr_y);
           }
           else if(scr_x-((ds.right/2)-(res_x/2))<res_x/2-199){
                fx-=ds.right/2-scr_x;
                SetCursorPos(ds.right/2,scr_y);
           }
              
           if((scr_y-((ds.bottom/2)-(res_y/2))>res_y/2+199)&&(elevation<90)&&(elevation>-90)){
                fy+=scr_y-ds.bottom/2;              
                SetCursorPos(scr_x,ds.bottom/2);
           }
           else if((scr_y-((ds.bottom/2)-(res_y/2))<res_y/2-199)&&(elevation<90)&&(elevation>-90)){
                fy-=ds.bottom/2-scr_y;
                SetCursorPos(scr_x,ds.bottom/2);
           }
           
           azimuth=(x-308)*sens;
           if(((y-218)*sens<90)&&((y-218)*sens>-90))elevation=(y-218)*sens;
           else if((y-218)*sens>90) elevation=90;
           else if((y-218)*sens<-90) elevation=-90;
}

void DrawButton(button btn){
     if(btn.selected()) glColor4f(0,0.0,0.6,1);
     else glColor4f(0,0.0,0.3,1);
     glBegin(GL_QUADS);
     glVertex3f(btn.get_x()+btn.get_width(), -btn.get_y(), -0.1111);
     glVertex3f(btn.get_x(), -btn.get_y(), -0.1111);
     glVertex3f(btn.get_x(), -btn.get_y()-btn.get_height(), -0.1111);
     glVertex3f(btn.get_x()+btn.get_width(),-btn.get_y()-btn.get_height(), -0.1111);
     glEnd();
}

void WriteText (float tx, 
               float ty,
               float ts,
               CHAR* SText,
               float tr,
               float tg,
               float tb,
               float ta,
               CHAR* SType,
               GLuint font_tex)
{
                          
    //char SFCHR[1024];
    int STcnt=0;
    int U,V=0;
    //STlnt=strlen(SText); 
    glPushMatrix ();          
    glScalef(ts,ts,1);
    glTranslatef(tx/ts,ty/ts,0);
    //strcpy(SFCHR, SText);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glEnable(GL_TEXTURE_2D);
    char c;
   while (SText[STcnt]!='\0') 
   {
    c = SText[STcnt];
    if((unsigned char)c>223) c-=32;
    if(c=='Р') c='P'; //Cyrilic bug fix
    else if(c=='А') c='A';
    V=((int)c/16)-1;
    U=(int)c-(V*16)-1;
    if (U!=15) V++;
    if (SType=="Vertical") {ty=tx; tx=0;}
    if (SType=="Diagonal//") ty=tx;
    if (SType=="Diagonal\\") ty=-tx;
    glTranslatef(tx,ty,0);
    if ((SType=="Defaut") or (SType!="Diagonal//") or (SType!="Diagonal\\") or (SType==NULL) or (SType!="Vertical")) ty=0;     
    glBegin(GL_QUADS);
     glColor4f(tr,tg,tb,ta);           
     glTexCoord2f(U*0.0625, 0.064+V*0.0625);
     glVertex3f(-0.1, -0.1, -0.11);
     glTexCoord2f(0.064+U*0.0625, 0.064+V*0.0625);
     glVertex3f(0.1, -0.1, -0.11);
     glTexCoord2f(0.064+U*0.0625, V*0.0625);
     glVertex3f(0.1, 0.1, -0.11);
     glTexCoord2f(U*0.0625, V*0.0625);
     glVertex3f(-0.1, 0.1, -0.11);
    glEnd();
    STcnt++;
    tx=+0.2;
    } 
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();  
}

/**************************
 * WinMain
 *
 **************************/

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;

    //data* lines;
    long int offset_x,offset_y;
    //bool opened_file;   

    //BuildSewage(num_pipes,num_sh,pipes,shafts,lines);
    
    free(lines);
    free(pipes);
    free(shafts);
    
    button open_btn(-1.33,-0.84,0.25,0.15,1);
    button export_btn(-1.06,-0.84,0.29,0.15,1);
    button zoomm_btn(1.2,-0.76,0.07,0.07,1);
    button zoomp_btn(1.1,-0.76,0.07,0.07,1);
    button mright_btn(1.0,-0.76,0.07,0.07,1);
    button rstpos_btn(0.9,-0.76,0.07,0.07,1);
    button mleft_btn(0.8,-0.76,0.07,0.07,1);
    button mfwd_btn(0.9,-0.86,0.07,0.07,1);
    button mbck_btn(0.9,-0.66,0.07,0.07,1);
    button mup_btn(0.5,-0.86,0.07,0.07,1);
    button mdwn_btn(0.5,-0.66,0.07,0.07,1);

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
    RECT ds;
    RECT rect;
    int width=800;
    int height=600;
    GetWindowRect(GetDesktopWindow(), &ds);
    hWnd = CreateWindow (
      "GLSample", "SEW-3D Storm sewage 3D visualisation", 
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      ((ds.right/2)-width/2), ((ds.bottom/2)-height/2), width, height,
      NULL, NULL, hInstance, NULL);

    HANDLE h2 = (HANDLE)_beginthread((void(*)(void*))Phys_Thread, 0, NULL);
    spec.set_coords(0,0,0);

    float m_x,m_y;
    
    help_message += "- За отваряне на файл, натиснете бутона \"Отвори файл\" или натиснете [Ctrl]+[O]\n";
    help_message +="- За прехвърляне във формат на триизмерен модел,\nнатиснете бутона \"Експорт\" или [Ctrl]+[E]\n";
    help_message +="- За движение, използвайте стрелките на клавиатурата или бутоните с икони на стрелки в горния десен ъгъл на екрана\n";
    help_message +="- За увеличаване и намаляне на мащаба на модела, натиснете [Ctrl] + [-] или [Ctrl] + [+] или бутоните в горния десен ъгъл";
    help_message +=" с икони '+' и '-' под числото, което показва мащаба\n";
    help_message +="- За движение нагоре или надолу, натиснете [Shift] или [Space] или използвайте бутоните с икона '+' или '-', ";
    help_message +="разположени под и над надписа \"Виочина (м)\"\n";
    help_message +="- За да се покажат кадрите в секунди , натиснете [F3]\n";
    help_message +="\nПрограмата е създадена от Кристиян Попов\nе_mail: kristian@popovbg.eu\n";
    help_message +="Всички права запазени (c) 2015\nPowered by SAD Engine I\n";  

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    
    GLuint font = loadTexture(true, "./font2.dds" );
    GLuint compass = loadTexture(true, "./compass.dds" );
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {   
             GetCursorPos( &pt );
             m_x=(float)(pt.x-rect.left-width/2)*((float)width/height)/(width/2.015)-0.02;
             m_y=-(float)(pt.y-rect.top-height/2)/(height/2)-0.03;
             
             if((open_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) o_cmd=true;
             if((export_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) e_cmd=true;
             if((zoomm_btn.detect(m_x,-m_y,click)==2)&&(any_btn)){ 
                if(global_scale>0.1) global_scale-=0.05;
                click=false;
             }
             if((zoomp_btn.detect(m_x,-m_y,click)==2)&&(any_btn)){ 
                global_scale+=0.05;
                click=false;
             }
             if((mright_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_right(cam_speed);
             if((rstpos_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.set_coords(0,0,0);
             if((mleft_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_left(cam_speed);
             if((mfwd_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_forward(cam_speed);
             if((mbck_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_back(cam_speed);
             if((mup_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_up(cam_speed);
             if((mdwn_btn.detect(m_x,-m_y,click)==2)&&(any_btn)) spec.move_down(cam_speed);
             //else move_right=false;
             
             
             
             if(open_btn.selected()||
             export_btn.selected()||
             zoomm_btn.selected()||
             zoomp_btn.selected()||
             mright_btn.selected()||
             rstpos_btn.selected()||
             mleft_btn.selected()||
             mfwd_btn.selected()||
             mbck_btn.selected()||
             mup_btn.selected()||
             mdwn_btn.selected()
             ) any_btn=true;
             else any_btn=false;
            /* OpenGL animation code goes here */
            
            //cout<<spec.get_x()<<' '<<spec.get_y()<<endl;
            
            if(GetWindowRect(hWnd, &rect))
            {
                if((width != rect.right - rect.left) || (height != rect.bottom - rect.top)){
                  width = rect.right - rect.left;
                  height = rect.bottom - rect.top;
                  glViewport(0, 0, width, height);
                  glMatrixMode(GL_PROJECTION);
                  glLoadIdentity();
                  gluPerspective(40.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);
                  glMatrixMode(GL_MODELVIEW);
                  glLoadIdentity(); 
                  
                  open_btn.set_x(-(GLfloat)width/(GLfloat)height+0.00333);
                  export_btn.set_x(-(GLfloat)width/(GLfloat)height+0.27333);
                  zoomm_btn.set_x((GLfloat)width/(GLfloat)height-0.13333);
                  zoomp_btn.set_x((GLfloat)width/(GLfloat)height-0.23333);
                  mright_btn.set_x((GLfloat)width/(GLfloat)height-0.33333);
                  rstpos_btn.set_x((GLfloat)width/(GLfloat)height-0.43333);   
                  mleft_btn.set_x((GLfloat)width/(GLfloat)height-0.53333);
                  mfwd_btn.set_x((GLfloat)width/(GLfloat)height-0.43333);
                  mbck_btn.set_x((GLfloat)width/(GLfloat)height-0.43333); 
                  mup_btn.set_x((GLfloat)width/(GLfloat)height-0.83333); 
                  mdwn_btn.set_x((GLfloat)width/(GLfloat)height-0.83333);         
                }
            }
            
            if(cam_ctrl) XY2Polar(ds,pt.x,pt.y,azth,elev,width,height,0.1);
	        
         glClear(GL_DEPTH_BUFFER_BIT  | GL_COLOR_BUFFER_BIT);
         glDepthFunc(GL_LESS);
         glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
   	     glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
   	     glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
   	     glLightfv(GL_LIGHT0, GL_POSITION, position0);
   	     
   	     // enable color tracking
            glEnable(GL_COLOR_MATERIAL);
            // set material properties which will be assigned by glColor
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glFrontFace(GL_CCW);
         
         glPushMatrix ();
             
             spec.set_angles(azth,elev);
             CameraUse(spec);
             //glScalef(0.41,0.415,1);
              
         //glScalef(0.75,1,1);
         //glScalef(0.41,0.415,1);
         glPushMatrix();
         glTranslatef(0,0,0);
         glScalef(global_scale,global_scale,-global_scale);
         
         if(opened_file){
         glDisable(GL_LIGHTING);
         glColor3f(0.0f, 1.0f, 0.0f);
         glPointSize(5);
         glBegin(GL_POINTS);
         for(int k=0; k<progress; k++){
                 glVertex3f(pipes[k].get_start_x(), pipes[k].get_start_z(), pipes[k].get_start_y());
                 glVertex3f(pipes[k].get_end_x(), pipes[k].get_end_z(), pipes[k].get_end_y());
                 glVertex3f(pipes[k].get_start_x(), lines[k].start_z_terrain, pipes[k].get_start_y());
                 glVertex3f(pipes[k].get_end_x(), lines[k].end_z_terrain, pipes[k].get_end_y());
         }
         glEnd();
         glLineWidth(1.5);
         glBegin(GL_LINES);
         for(int k=0; k<progress; k++){
            if(((fabs(pipes[k].get_start_x()*global_scale-spec.get_x())<50*global_scale)&&(fabs(pipes[k].get_start_y()*global_scale-spec.get_y())<50*global_scale)&&(fabs(pipes[k].get_start_z()*global_scale-spec.get_z())<50*global_scale))||((fabs(pipes[k].get_end_x()*global_scale-spec.get_x())<50*global_scale)&&(fabs(pipes[k].get_end_y()*global_scale-spec.get_y())<50*global_scale)&&(fabs(pipes[k].get_end_z()*global_scale-spec.get_z())<50*global_scale))){
                 glColor3f(0.0f, 0.0f, 0.5f);
            }                                                                                                                                                                                                                                                                                                               
            else glColor3f(0.7f, 0.7f, 0.7f);
                 glVertex3f(pipes[k].get_start_x(), pipes[k].get_start_z(), pipes[k].get_start_y());
                 glVertex3f(pipes[k].get_end_x(), pipes[k].get_end_z(), pipes[k].get_end_y());
         } 
         glEnd();
         glEnable(GL_LIGHTING);
         
         glBegin(GL_TRIANGLES);
         glColor4f(1.0f, 1.0f, 1.0f,1);
         for(int k=0; k<progress; k++){
            if(((fabs(pipes[k].get_start_x()*global_scale-spec.get_x())<100*global_scale)&&(fabs(pipes[k].get_start_y()*global_scale-spec.get_y())<100*global_scale)&&(fabs(pipes[k].get_start_z()*global_scale-spec.get_z())<100*global_scale))||((fabs(pipes[k].get_end_x()*global_scale-spec.get_x())<100*global_scale)&&(fabs(pipes[k].get_end_y()*global_scale-spec.get_y())<100*global_scale)&&(fabs(pipes[k].get_end_z()*global_scale-spec.get_z())<100*global_scale))){
            //if((sqrt(pow(pipes[k].get_start_x(),2)+pow(pipes[k].get_start_y(),2))-sqrt(pow(spec.get_x(),2)+pow(spec.get_y(),2))<100/global_scale)||((sqrt(pow(pipes[k].get_end_x(),2)+pow(pipes[k].get_end_y(),2)))-sqrt(pow(spec.get_x(),2)+pow(spec.get_y(),2))<100/global_scale)){
             for(int i=0; i<pipes[k].num_f; i++){
                     glNormal3d(pipes[k].arr_vn[pipes[k].f[i].a.n*3], pipes[k].arr_vn[pipes[k].f[i].a.n*3+2], pipes[k].arr_vn[pipes[k].f[i].a.n*3+1]);
                     //glVertex3f(pipes[k].arr_v[pipes[k].arr_f[i]*3], pipes[k].arr_v[pipes[k].arr_f[i]*3+2], pipes[k].arr_v[pipes[k].arr_f[i]*3+1]);
                     glVertex3f(pipes[k].arr_v[pipes[k].f[i].a.v*3], pipes[k].arr_v[pipes[k].f[i].a.v*3+2], pipes[k].arr_v[pipes[k].f[i].a.v*3+1]);
                     glNormal3d(pipes[k].arr_vn[pipes[k].f[i].b.n*3], pipes[k].arr_vn[pipes[k].f[i].b.n*3+2], pipes[k].arr_vn[pipes[k].f[i].b.n*3+1]); 
                     glVertex3f(pipes[k].arr_v[pipes[k].f[i].b.v*3], pipes[k].arr_v[pipes[k].f[i].b.v*3+2], pipes[k].arr_v[pipes[k].f[i].b.v*3+1]);
                     glNormal3d(pipes[k].arr_vn[pipes[k].f[i].c.n*3], pipes[k].arr_vn[pipes[k].f[i].c.n*3+2], pipes[k].arr_vn[pipes[k].f[i].c.n*3+1]); 
                     glVertex3f(pipes[k].arr_v[pipes[k].f[i].c.v*3], pipes[k].arr_v[pipes[k].f[i].c.v*3+2], pipes[k].arr_v[pipes[k].f[i].c.v*3+1]); 
             }
            }
         }
              
         glEnd();
         
         for(int i=0; i<num_sh; i++){
                 if((fabs(shafts[i].get_x()*global_scale-spec.get_x())<100*global_scale)&&(fabs(shafts[i].get_y()*global_scale-spec.get_y())<100*global_scale)&&(fabs(shafts[i].get_z_terrain()*global_scale-spec.get_z())<100*global_scale)){
                      glPushMatrix();
                      glTranslatef(shafts[i].get_x(),shafts[i].get_z_bottom()-0.5,shafts[i].get_y());
                      glScalef(shafts[i].get_diameter()*0.001,shafts[i].get_z_terrain()-shafts[i].get_z_bottom()+0.5,shafts[i].get_diameter()*0.001);
                      glBegin(GL_TRIANGLES);
                      glColor4f(1.0f, 1.0f, 1.0f,1);
                      for(int k=0; k<shaft_sd.num_f; k++){
                             glNormal3d(shaft_sd.arr_vn[shaft_sd.f[k].a.n*3], shaft_sd.arr_vn[shaft_sd.f[k].a.n*3+2], shaft_sd.arr_vn[shaft_sd.f[k].a.n*3+1]);
                             glVertex3f(shaft_sd.arr_v[shaft_sd.f[k].a.v*3], shaft_sd.arr_v[shaft_sd.f[k].a.v*3+2], shaft_sd.arr_v[shaft_sd.f[k].a.v*3+1]);
                             glNormal3d(shaft_sd.arr_vn[shaft_sd.f[k].b.n*3], shaft_sd.arr_vn[shaft_sd.f[k].b.n*3+2], shaft_sd.arr_vn[shaft_sd.f[k].b.n*3+1]);
                             glVertex3f(shaft_sd.arr_v[shaft_sd.f[k].b.v*3], shaft_sd.arr_v[shaft_sd.f[k].b.v*3+2], shaft_sd.arr_v[shaft_sd.f[k].b.v*3+1]);
                             glNormal3d(shaft_sd.arr_vn[shaft_sd.f[k].c.n*3], shaft_sd.arr_vn[shaft_sd.f[k].c.n*3+2], shaft_sd.arr_vn[shaft_sd.f[k].c.n*3+1]);
                             glVertex3f(shaft_sd.arr_v[shaft_sd.f[k].c.v*3], shaft_sd.arr_v[shaft_sd.f[k].c.v*3+2], shaft_sd.arr_v[shaft_sd.f[k].c.v*3+1]);
                      }
                      glEnd();
                      glPopMatrix();
                      glPushMatrix();
                      glTranslatef(shafts[i].get_x(),shafts[i].get_z_terrain()+0.5,shafts[i].get_y());
                      glRotatef(spec.get_angle_z(),0,1,0);
                      glRotatef(spec.get_elevation(),1,0,0);
                      WriteText(0,0,3,shafts[i].get_name(),1,0,0,1,NULL,font);
                      glPopMatrix();                                                                                                                                                                                                                       
                      //glVertex3f(shafts[i].get_x(), shafts[i].get_z_terrain(), shafts[i].get_y());                                                                                                                                                                                                                        
                 }                                                                                                                                                                                                                             
         }
         glDisable(GL_LIGHTING);
         for(int k=0; k<progress; k++){
            if(((fabs(pipes[k].get_start_x()*global_scale-spec.get_x())<100*global_scale)&&(fabs(pipes[k].get_start_y()*global_scale-spec.get_y())<100*global_scale)&&(fabs(pipes[k].get_start_z()*global_scale-spec.get_z())<100*global_scale))||((fabs(pipes[k].get_end_x()*global_scale-spec.get_x())<100*global_scale)&&(fabs(pipes[k].get_end_y()*global_scale-spec.get_y())<100*global_scale)&&(fabs(pipes[k].get_end_z()*global_scale-spec.get_z())<100*global_scale))){         
                      glPushMatrix();
                      //glTranslatef(pipes[k].get_start_x()+(pipes[k].get_hor_length()*cos(pipes[k].get_top_angle()*PI/180)/2),pipes[k].get_start_z()+0.5,pipes[k].get_start_y()+(pipes[k].get_hor_length()*sin(pipes[k].get_top_angle()*PI/180)/2));
                      if(pipes[k].get_end_y()>pipes[k].get_start_y()) glTranslatef(pipes[k].get_start_x()+(pipes[k].get_hor_length()*cos(pipes[k].get_top_angle()*PI/180)/2),pipes[k].get_start_z()+pipes[k].get_slope()*pipes[k].get_hor_length()/2+1,pipes[k].get_start_y()+(pipes[k].get_hor_length()*sin(pipes[k].get_top_angle()*PI/180)/2));
                      else glTranslatef(pipes[k].get_start_x()+(pipes[k].get_hor_length()*cos(pipes[k].get_top_angle()*PI/180)/2),pipes[k].get_start_z()+pipes[k].get_slope()*pipes[k].get_hor_length()/2+1,pipes[k].get_start_y()-(pipes[k].get_hor_length()*sin(pipes[k].get_top_angle()*PI/180)/2));
                      glRotatef(spec.get_angle_z(),0,1,0);
                      glRotatef(spec.get_elevation(),1,0,0);
                      WriteText(0,0,3,pipes[k].get_clone_name(),0,0,0,1,NULL,font);
                      //conv_str = to_string(pipes[k].get_slope());
                      //WriteText(0,-0.12,3,(char*)conv_str.c_str(),0,0,0,1,NULL,font);
                      glPopMatrix(); 
                      glPushMatrix();
                      glTranslatef(pipes[k].get_start_x(),pipes[k].get_start_z()+2,pipes[k].get_start_y());
                      if(pipes[k].get_end_y()>pipes[k].get_start_y()) glRotatef(pipes[k].get_top_angle(),0,-1,0);
                      else glRotatef(pipes[k].get_top_angle(),0,1,0);
                      //glRotatef(spec.get_elevation(),1,0,0);
                      glColor3f(1,1,0);
                      glLineWidth(2);
                      glBegin(GL_LINES);
                      glVertex3f(pipes[k].get_hor_length()/2-1.5,1,0);
                      glVertex3f(pipes[k].get_hor_length()/2+1,1,0);

                      glEnd();
                      glEnable(GL_LIGHTING);
                      glBegin(GL_TRIANGLES);
                      glNormal3d(-0.9284,0,0.37139);
                      glVertex3f(pipes[k].get_hor_length()/2+1,1,0);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,1.2,0.2);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,0.8,0.2);
                      glNormal3d(-0.9284,0,-0.37139);
                      glVertex3f(pipes[k].get_hor_length()/2+1,1,0);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,1.2,-0.2);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,0.8,-0.2);
                      glNormal3d(-0.9284,0.37139,0);
                      glVertex3f(pipes[k].get_hor_length()/2+1,1,0);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,1.2,0.2);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,1.2,-0.2);
                      glNormal3d(-0.9284,-0.37139,0);
                      glVertex3f(pipes[k].get_hor_length()/2+1,1,0);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,0.8,0.2);
                      glVertex3f(pipes[k].get_hor_length()/2+0.5,0.8,-0.2);
                      glEnd();
                      glPopMatrix();
                      glDisable(GL_LIGHTING);
            }
         } 
         glEnable(GL_LIGHTING);
         }
          
          glPopMatrix ();
          glPopMatrix ();
          
          glPushMatrix();
            glTranslatef((GLfloat)width/(GLfloat)height*0.04-0.012,-0.03,0);
            glRotatef(spec.get_angle_z(),0,0,1);
            glDisable(GL_LIGHTING);
            glBindTexture(GL_TEXTURE_2D, compass);
            glEnable(GL_TEXTURE_2D);          
              glColor4f(1,1,1,1);
              glBegin(GL_QUADS);
              glTexCoord3f(1, 1, 0);
              //glNormal3f(0, 0, 0);
              glVertex3f(0.01, -0.01, -0.111);
              glTexCoord3f(0, 1, 0);
              //glNormal3f(1, 0, 0);
              glVertex3f(-0.01, -0.01, -0.111);
              glTexCoord3f(0, 0, 0);
              //glNormal3f(1, 1, 0);
              glVertex3f(-0.01, 0.01, -0.111);
              glTexCoord3f(1, 0, 0);
              //glNormal3f(0, 1, 0);
              glVertex3f(0.01,0.01, -0.111);
              glEnd();
          glDisable(GL_TEXTURE_2D); 
          //glEnable(GL_LIGHTING);
          glPopMatrix();          
          
          glPushMatrix();
               glScalef(0.04,0.04,1);
                              
               if(show_fps%2){
               GetSystemTime(&FPStime);
                   if(FPStime.wSecond!=old_time){
                       FPS_=FPS;
                       FPS=0;
                       old_time=FPStime.wSecond;
                   }
                   itoa(FPS_,conv_c_str,10);
                   WriteText(-(GLfloat)width/(GLfloat)height*0.82,-0.7,0.2,"FPS:",1,1,1,1,NULL,font);
                   WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.15,-0.7,0.2,conv_c_str,1,1,1,1,NULL,font);
               }
               itoa(progress,conv_c_str,10);
               WriteText(-(GLfloat)width/(GLfloat)height*0.82,-0.65,0.2,"Тръби:",1,1,1,1,NULL,font);
               WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.2,-0.65,0.2,conv_c_str,1,1,1,1,NULL,font);
               WriteText(-(GLfloat)width/(GLfloat)height*0.82,-0.75,0.2,"За помощ, натиснете F1",1,1,1,1,NULL,font);
               if(spec.get_angle_z()>=0) itoa((int)spec.get_angle_z()%360,conv_c_str,10);
               else itoa((int)spec.get_angle_z()%360+360,conv_c_str,10);
               WriteText((GLfloat)width/(GLfloat)height*0.8-0.245,-0.49,0.25,"^",1,0,0,1,NULL,font);
               WriteText((GLfloat)width/(GLfloat)height*0.82-0.272,-0.6,0.2,conv_c_str,1,1,1,1,NULL,font);
               WriteText((GLfloat)width/(GLfloat)height*0.82-0.272,-0.65,0.2,"DEG",0,0.5,1,1,NULL,font);

              DrawButton(open_btn);
              DrawButton(export_btn);
              DrawButton(zoomm_btn);
              DrawButton(zoomp_btn);
              DrawButton(mright_btn);
              DrawButton(rstpos_btn);
              DrawButton(mleft_btn);
              DrawButton(mfwd_btn);
              DrawButton(mbck_btn);
              DrawButton(mup_btn);
              DrawButton(mdwn_btn);
              itoa((int)(global_scale*100),conv_c_str,10);
              WriteText((GLfloat)width/(GLfloat)height*0.82-0.17,0.655,0.2,conv_c_str,1,1,1,1,NULL,font);
              WriteText((GLfloat)width/(GLfloat)height*0.82-0.07,0.655,0.2,"%",1,1,1,1,NULL,font);
              WriteText((GLfloat)width/(GLfloat)height*0.82-0.8,0.615,0.2,"ВИСОЧИНА(M)",1,1,1,1,NULL,font);
              itoa((int)(spec.get_z()/global_scale+offset_z),conv_c_str,10);
              WriteText((GLfloat)width/(GLfloat)height*0.82-0.7,0.574,0.2,conv_c_str,1,1,1,1,NULL,font);
              if(open_btn.selected()){
                  WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.015,0.655,0.2,"ОТВОРИ",1,1,1,1,NULL,font);
                  WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.04,0.615,0.2,"ФАЙЛ",1,1,1,1,NULL,font);
              }
              else{
                  WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.015,0.655,0.2,"ОТВОРИ",0.4,0.8,1,1,NULL,font);
                  WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.04,0.615,0.2,"ФАЙЛ",0.5,0.85,1,1,NULL,font);                   
              }
              if(export_btn.selected()) WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.236,0.632,0.2,"ЕКСПОРТ",1,1,1,1,NULL,font);
              else WriteText(-(GLfloat)width/(GLfloat)height*0.82+0.236,0.632,0.2,"ЕКСПОРТ",0.5,0.85,1,1,NULL,font);
              if(rstpos_btn.selected()) WriteText((GLfloat)width/(GLfloat)height*0.88-0.378,0.632,0.13,"RST",1,1,1,1,NULL,font);
              else WriteText((GLfloat)width/(GLfloat)height*0.88-0.378,0.632,0.13,"RST",0.5,0.85,1,1,NULL,font);
              
              glLineWidth(1);
              glBegin(GL_LINES);
              if(zoomm_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.11333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.08333,0.725,-0.111);
              if(zoomp_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.21333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.18333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.2,0.71,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.2,0.74,-0.111);
              if(mright_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);              
              glVertex3f((GLfloat)width/(GLfloat)height-0.31333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.28333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.29333,0.715,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.28333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.29333,0.735,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.28333,0.725,-0.111);
              if(mleft_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);              
              glVertex3f((GLfloat)width/(GLfloat)height-0.51333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.48333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.50333,0.715,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.51333,0.725,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.50333,0.735,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.51333,0.725,-0.111);
              if(mfwd_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.81,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.84,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.84,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.41,0.83,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.84,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.39,0.83,-0.111);
              if(mbck_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.61,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.64,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.61,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.41,0.62,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.4,0.61,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.39,0.62,-0.111);
              if(mup_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.81333,0.825,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.78333,0.825,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.8,0.81,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.8,0.84,-0.111);
              if(mdwn_btn.selected()) glColor4f(1,1,1,1);
              else glColor4f(0.5,0.85,1,1);
              glVertex3f((GLfloat)width/(GLfloat)height-0.81333,0.625,-0.111);
              glVertex3f((GLfloat)width/(GLfloat)height-0.78333,0.625,-0.111);
              glEnd();

          glPopMatrix();

            SwapBuffers (hDC);
            FPS++;
            theta += 0.1f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;
        
    
    case WM_LBUTTONDOWN: click=true;
                         if(!any_btn){
                         if (!cam_ctrl){ 
                             ShowCursor(false);
                             if((x!=0) && (y!=0)){ 
                                   SetCursorPos(x,y);
                             }
                             else{
                                  GetCursorPos( &pt );
                                  x=pt.x;
                                  y=pt.y;
                             }
                         }
                         cam_ctrl=true;
                         }
          
        break;
        
      case WM_LBUTTONUP: click=false;
                         if(cam_ctrl)ShowCursor(true);
                    cam_ctrl=false;
                    GetCursorPos( &pt );
                    x=pt.x;
                    y=pt.y;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
               case 'D': move_right=true;
                         break;
               case 'A': move_left=true;
                         break;
               case 'W': move_forward=true;
                         break;
               case 'S': move_back=true;
                         break;
               case VK_RIGHT: move_right=true;
                         break;
               case VK_LEFT: move_left=true;
                         break;
               case VK_UP: move_forward=true;
                         break;
               case VK_DOWN: move_back=true;
                         break;
               case VK_SHIFT: move_down=true;
                         break;
               case VK_CONTROL: ctrl=true;
                         break;
               case ' ': move_up=true;
                         break;
               case VK_F1: MessageBox(NULL,
                           help_message.c_str(),
                           "Помощ",
                           	MB_ICONINFORMATION | MB_OK);
               case VK_F3: show_fps++;
                         break;
               case VK_ADD: if (ctrl) global_scale+=0.05;
                         break;
               case 0xBB: if (ctrl) global_scale+=0.05;
                         break;
               case VK_SUBTRACT: if((global_scale>0.1)&&(ctrl)) global_scale-=0.05;
                         break;
               case 0xBD: if((global_scale>0.1)&&(ctrl)) global_scale-=0.05;
                         break;
               case 'O': if(ctrl) o_cmd=1;
                        break;
               case 'E': if(ctrl) e_cmd=1;
                        break;
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;
	case WM_KEYUP :
         switch (wParam)
         {
          case 'D': move_right=false;
                    break;
          case 'A': move_left=false;
                    break;
	      case 'W': move_forward=false;
                    break;
	      case 'S': move_back=false;
                    break;
          case VK_RIGHT: move_right=false;
                    break;
          case VK_LEFT: move_left=false;
                    break;
          case VK_UP: move_forward=false;
                    break;
          case VK_DOWN: move_back=false;
                    break;
          case VK_SHIFT: move_down=false;
                    break;
          case VK_CONTROL: ctrl=false;
                    break;
          case ' ': move_up=false;
                    break;
         }
         
		break;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}

/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
    GL2Init();
             
     glViewport(0, 0, 800, 600);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(40.0f,(GLfloat)800/(GLfloat)600,0.1f,1000.0f);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glEnable(GL_DEPTH_TEST);
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_COLOR_MATERIAL);
     glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
     glShadeModel(GL_SMOOTH);
     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
     glEnable(GL_POLYGON_SMOOTH);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthFunc(GL_LEQUAL);
     glClearDepth(1.0f);
     //glEnable(GL_CULL_FACE);
     glEnable(GL_NORMALIZE);
            //glFrustum( - 1 , 1 , - 1 , 1 , 1 , 100 );
            glClearColor (0.0f, 0.5f, 1.0f, 0.0f);	    

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

void Phys_Thread(void* PARAMS){
     
    while(1){
       Sleep(10);
       //Do something
       
       if(o_cmd){  opened_file=false;
                   spec.set_coords(0,-400,100);
                   global_scale=0.1;
                   num_pipes=0;
                   num_sh=0;
                   if(num_pipes){
                   free(lines);
                   free(pipes);
                   free(shafts);
                   }
                   lines = OpenFile(num_pipes,progress);
                   if(num_pipes){
                      opened_file=true;
                      BuildSewage(num_pipes,num_sh,pipes,shafts,lines,progress);
                   }
                   else opened_file=false;
                   //global_scale=1;   
                   spec.set_coords(0,0,0);
                   o_cmd=0;
                   any_btn=false;
       }
       if(e_cmd){
           ExportFile(num_pipes,num_sh,pipes,shafts,shaft_sd);
           e_cmd=0;
       }
       
       if(move_forward){
           spec.move_forward(cam_speed);          
       }
       if(move_back){
           spec.move_back(cam_speed);
       }
       if(move_left){ 
           spec.move_left(cam_speed);
       }
       if(move_right){ 
           spec.move_right(cam_speed);
       }
       if(move_up){ 
           spec.move_up(cam_speed);
       }
       if(move_down){ 
           spec.move_down(cam_speed);
       }         
    }    
}
