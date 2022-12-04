#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

int roadW = 3000;
int segL = 200;
float camD = 0.84;
int width = 1024;
int height = 768;
int roadLength = 10000;
float acceleration = 0.243 ;
float speed = 0;
int maxSpeed = 10000000;
int playerX = 0;
FILE *myfile;


struct Line
{
  float x, y, z; //3d center of line
  float X,Y,W; //screen coord
  float scale, curve;
};

void project(struct Line* self, int camX,int camY,int camZ)
  {
    self -> scale = camD/((self -> z)-camZ);
    fprintf(myfile, "SCALE: %f / (%f - %d) = %f\n", camD, self -> z, camZ, self -> scale);
    self -> X = (1 + (self -> scale) *((self -> x) - camX)) * width/2;
    //self -> Y = (1 - (self -> scale) *((self -> y) - camY)) * height/2;
    self -> Y = (height/2) - (height/2) * (((self -> y) - camY) * self -> scale);
    fprintf(myfile, "Y: (%d / 2) - (%d / 2) * ((%f - %d) * %f) = %f\n", height, height, self -> y, camY, self -> scale, self -> Y);
    self -> W = (self -> scale) * roadW  * width/2;
  }


void drawQuad(Color color, int x1, int y1, int w1, int x2, int y2, int w2){

    Vector2 vec[] = { { x2 - w2, y2}, {x1 - w1, y1 }, { x1 + w1, y1}, { x2 + w2, y2} };
	DrawTriangleFan( vec, 4, color );


}


int main() {
    myfile = fopen("hello.txt", "w");
    Color SKY = (Color){229, 233, 244, 255 } ;
    InitWindow(width, height, "");
    SetTargetFPS(60);    
    
    struct Line lines[roadLength];

     for(int i=0;i<roadLength;i++)
     {  
       struct Line line;
       line.z = (float)i*segL;
       line.x = 0;
       line.y = 0;
       line.curve = 0;
       if (i > 100 && i < 500) line.curve = 5;
       lines[i] = line;
     }

    int N = roadLength;
    float pos = 0;

    while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_RIGHT)) playerX += 100;
    if (IsKeyDown(KEY_LEFT)) playerX -= 100;
    if (IsKeyDown(KEY_UP) && speed <= maxSpeed) speed += acceleration;
    else if (IsKeyDown(KEY_DOWN) && speed >= -maxSpeed) speed -= acceleration;
    else if (speed < 0){
      speed += acceleration;
    }
    else if (speed > 0 ){
      speed -= acceleration;
    }
    pos += speed;
        
    
		BeginDrawing();
		ClearBackground(SKY); 
    int startPos = pos/segL;
    float x = 0, dx = 0;


    float temp;
    fprintf(myfile, "NEW STRING\n");

    int flag = 0;

    for(int n = startPos; n < (startPos + 100); n++)  
            {
                
                struct Line* l = &(lines[n%N]); 
                x+=dx;
                dx += l -> curve;
                l -> x = x;

                if (!flag){
                  playerX -= dx * speed * 0.1;
                  flag = 1;
                }
                printf("%f\n", dx);
                project(l, playerX, 1500, pos);

                Color grass  = (n/3)%2 ? (Color){170, 166, 165, 255 }: (Color){195, 197, 209, 255};
                Color rumble = (n/3)%2 ? (Color){ 255, 255, 255, 255 } : (Color){ 0, 0, 0, 255 };
                Color road   = (n/3)%2 ? (Color){ 114, 105, 90, 255 } : (Color){ 140, 129, 111, 255 };

                struct Line p = lines[(n-1)%N]; 
                double x1, y1, w1, x2, y2, w2;

               double yy = l -> Y;
               double yyy = p.Y;
               if (yy < 0  || yyy < 0 || isinf(yyy) || isinf(yy))
               continue;
               fprintf(myfile, "%f %f\n", yyy, yy);
               drawQuad(grass, 0, yyy, width, 0, yy, width);
               drawQuad(road, p.X, yyy, p.W, l -> X, yy, l -> W);

               
              
            }
    
		EndDrawing();
    }
    CloseWindow();               
    return 0;
}
