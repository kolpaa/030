#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int roadW = 3000;
int segL = 200;
float camD = 0.84;
int width = 1980;
int height = 1080;
int roadLength = 5000;
int visionRange = 1000;
float acceleration = 0.5;
float speed = 0;
int maxSpeed = 500000000;
float playerX = 0;
int incline = 0;
FILE *myfile;
int oldposition;
int flag = 0;
float temp;
float roadX = 0;

struct Line
{
  float x, y, z, cos;
  float X, Y, W;
  float scale, curve;
};


struct Turn
{
  int begin, end, curve;
};

struct Hill
{
  int begin, end, heigh, climb, type;
};

void project(struct Line *self, int camX, int camY, int camZ)
{
  self->scale = camD / ((self->z) - camZ);
  //fprintf(myfile, "SCALE: %f / (%f - %d) = %f\n", camD, self->z, camZ, self->scale);
  self->X = (1 + (self->scale) * ((self->x) - camX)) * width / 2;
  self->Y = (height / 2) - (height / 2) * (((self->y) - camY) * self->scale);
  //fprintf(myfile, "Y: (%d / 2) - (%d / 2) * ((%f - %d) * %f) = %f\n", height, height, self->y, camY, self->scale, self->Y);
  self->W = (self->scale) * roadW * width / 2;
}

void drawQuad(Color color, int x1, int y1, int w1, int x2, int y2, int w2)
{
  Vector2 vec[] = {{x2 - w2, y2}, {x1 - w1, y1}, {x1 + w1, y1}, {x2 + w2, y2}};
  DrawTriangleFan(vec, 4, color);
}

int main()
{
  srand(time(NULL));
  myfile = fopen("hello.txt", "w");
  Color SKY = (Color){229, 233, 244, 255};
  InitWindow(width, height, "");
  SetTargetFPS(60);

  struct Line lines[roadLength];
  struct Line linesHelp[visionRange];


  int turnsCount = 0;
  struct Turn turns[roadLength];
  int curves[6] = { -0.5, -1, 1, 0.5};

  int hillsCount = 0;
  struct Hill hills[roadLength];

  for (int i = 1; i < roadLength + 1; i++)
  {
    int MyRandomValue1 = rand();
    int MyRandomValue2 = rand();
    
    struct Line line;
    line.z = (float)i * segL;
    line.x = 0;
    line.y = 0;
    line.cos = 0;
    line.curve = 0;

    if (MyRandomValue1 % 600 == 1 && i < roadLength - 400){
      struct Turn turn;
      turn.begin = i;
      int coef = rand() % 4;
      turn.end = i + (rand() % 400 + 100);
      turn.curve = curves[coef];
      turns[turnsCount] = turn;
      turnsCount++;

    }
    for (int j = 0; j < turnsCount; j++){
        if (i > turns[j].begin && i < turns[j].end){
          line.curve = turns[j].curve;
        }
    }
   if (sin(i/30.0) - 1 > -0.01 || sin(i/30.0) + 1 < 0.01){
        if (hillsCount - 1 > -1 && i < hills[hillsCount - 1].end){
          ;

        }
        else if (MyRandomValue2 % 20 == 1){
        struct Hill hill;
        if (sin(i/30.0) - 1 > -0.01){
          hill.type = 1;
        }
        else{
          hill.type = 0;
        }
        hill.climb = 30;
        hill.heigh = rand() % 3000 + 1000;
        hill.begin = i;
        
        int k = 1;
        hill.end = i + hill.climb * PI * 2 * k;
        if (hill.end < roadLength){
            hills[hillsCount] = hill;
            hillsCount++;
        }
        
      }
    }

    
    
    for (int j = 0; j < hillsCount; j++){
        if (i >= hills[j].begin && i <= hills[j].end){
          if (hills[j].type == 1){
              line.y += (sin(i/(float)hills[j].climb) - 1) * hills[j].heigh;
          }
          else{
            line.y += (sin(i/(float)hills[j].climb) + 1) * hills[j].heigh;
          }
          line.cos += ((float)hills[j].heigh/10)/(float)hills[j].climb * cos(i/(float)hills[j].climb);
        }
    }
    //if (i > 400 && i < 540)
    //  line.curve = 2;
    //if (i > 100 && i < 600)
      //line.curve = -2;

    //if (i > 200 && i < 400)
    //  line.curve = -3;

    if (sin(i/130.0) - 1 > -0.01){
        printf("%d\n", i);
    }

    if (i>=186 && i<=4306) {

      line.y += (sin(i/130.0) - 1) * 2500;
      line.cos += 250/130.0 * cos(i/130.0);
    }


    lines[i - 1] = line;
    if (i <= visionRange){
      linesHelp[i - 1] = line;
      linesHelp[i - 1].z += (float)roadLength * segL;
    }

  }


  int N = roadLength;
  float pos = 0;

  while (!WindowShouldClose())
  {
    
    if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)))
      playerX += fmin(speed, 100);
    if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)))
      playerX -= fmin(speed, 100);

    if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && speed <= 0)
      speed += acceleration * 2;
    if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && speed <= maxSpeed){
      if (speed < 30)  speed = 30;
      speed += acceleration * 2;
    } 

    else if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && speed >= 0)
      speed -= acceleration * 10;
    else if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))&& speed >= -maxSpeed)
      speed -= acceleration * 5;
    
    else if (speed < 0)
    {
      speed += acceleration;
    }
    else if (speed > 0)
    {
      speed -= 1 - acceleration;
    }

    if (playerX > roadW || playerX < -roadW){
      speed = fmin(speed, 40);
    }
    
    speed -= lines[oldposition % N].cos / 8;
    pos += speed;


    

    

    int startPos = pos / segL;

    if (startPos >= (roadLength - 1) && flag == 1){
        flag = 0;
        pos = 0;
        startPos = 0;
      }

    oldposition = startPos;

    int camH = 1500 + lines[startPos % N].y;
    float maxy = (float)height;
    float x = 0, dx = 0;
    //printf("%f ", lines[startPos].y);
    //printf("%d\n", camH);
    fprintf(myfile, "\n\nNEW STRING\n\n");
    


    playerX -= lines[startPos % N].curve * speed * 0.2;

    //printf("%f %f %f %f\n", lines[startPos % N].x, lines[startPos % N].curve, playerX, speed);

    BeginDrawing();
    ClearBackground(SKY);
    

    for (int n = startPos; n < (startPos + visionRange); n++)
    {
      fprintf(myfile, "%d %f\n", startPos, pos);
      
      struct Line *l = &(lines[n % N]);
      
       x += dx;
       dx += l->curve;
       l->x = x;
      
      project(l, playerX, camH, pos);

      if(l->scale < 0){

        l = &(linesHelp[n % N]);  
        flag = 1;
        project(l, playerX, camH, pos);
      }

       x += dx;
       dx += l->curve;
       l->x = x;
     
    
      Color grass = (n / 3) % 2 ? (Color){170, 166, 165, 255} : (Color){195, 197, 209, 255};
      Color rumble = (n / 3) % 2 ? (Color){255, 255, 255, 255} : (Color){0, 0, 0, 255};
      Color road = (n / 3) % 2 ? (Color){114, 105, 90, 255} : (Color){140, 129, 111, 255};

      struct Line p = lines[(n - 1) % N]; 
      if (p.scale < 0){
        p = linesHelp[(n - 1) % N]; 
      }   

      //fprintf(myfile, "%d\n", (n - 1) % N);
      double x1, y1, w1, x2, y2, w2;

      double yy = l->Y;
      double yyy = p.Y;
      if (yy < 0 || yyy < 0 || isinf(yyy) || isinf(yy))
        continue;


      //fprintf(myfile,"%f %d\n", l->Y, maxy);
      if (l->Y >= maxy) {
          //fprintf(myfile, "%f fff %f fff %d\n", l->Y,  maxy, height);
          continue;
      }
      maxy = l->Y;


      //fprintf(myfile, "%f %f\n", yyy, yy);
      drawQuad(grass, 0, yyy, width, 0, yy, width);
      drawQuad(road, p.X, yyy, p.W, l->X, yy, l->W);
      DrawRectangle(width/2 - 50, 920, 100, 100, BLACK);  
    }

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
