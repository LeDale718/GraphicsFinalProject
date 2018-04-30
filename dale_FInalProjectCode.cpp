//
//  dale_FinalProjectCode.cpp
//  Graphics
//
//  Created by Donna Le on 4/3/17.
//  Copyright © 2017 Donna Le. All rights reserved.
//

/*
 CREDIT TO GOOGLE IMAGES FOR THE TEXTURES and May Ang at
    https://gist.github.com/thaenor/4d9531cc9a7d1c34b998 for the rain.cpp code that I modified to make my bubbles in the lake.
 
 
 INSTRUCTIONS:
    up arrow - step forward
	down arrow - step backward
	left arrow - step left
	right arrow - step right
	Shift + up arrow - look up
	Shift + down arrow - look up
	Shift + left arrow - look left
	Shift + right arrow - look right
	n - go to night
	s - to swim up when in the lake
	d - go to day
	r - reset scene
	w - to continue walking after being stopped
	1/2 keys - enable/disable lake spotlight
	Right Mouse Click in Environment to show Options Menu
 
 */
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstdlib>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.14159265

#define RATIO 0.85 // Growth ratio = length of tree sub-branch to length of branch.
using namespace std;

static int id1, id2; // Window identifiers.

class Source;
void makeTreeList();
void makeBuildingList();

static unsigned int aTree; // List index.
static unsigned int aWindow; // List index.
static unsigned int aBuilding; // List index.


//****************************       ENVIRONMENT CONDITIONS       ******************************//
static int totheFuture = 0; // Future?
static int isFog = 0; // Is fog on?
static int backtoPast = 0; // Past?
static int isDrought = 0; // Drought
int counter = 0;


//**************************      FIRST-PERSON PERSPECTIVE     ******************************//
static int stopwalking = 0;
static float meX=0, meY=10,meZ=-80;
static float angle=0;
static float Yangle=0;
static float stepsize=3.0;  //step size
static float turnsize=10.0; //degrees to turn

float starSpec[] = { .8, .8, 0, .8 };
float no_mat[] = { 0.0, 0.0, 0.0, 1.0 };

//**********************************      BUBBLES / PARTICLE SYSTEM      ***************************************//

#define MAX_PARTICLES 400

typedef struct {
    // Life
    bool alive;	// is the particle alive?
    float life;	// particle lifespan
    float fade; // decay
    // color
    float red;
    float green;
    float blue;
    // Position/direction
    float xpos;
    float ypos;
    float zpos;
    // Velocity/Direction, only goes down in y dir
    float vel;
    // Gravity
    float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];
float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
int loop;

//**********************************  L A K E  /  W A T E R  ***************************************//

static GLUquadricObj *lake_ob;
float lake_r = 50;
GLboolean fall_in_lake = false;
GLboolean swimming = false;
float lake_x, lake_z = 100;
static float controlPoints[6][4][3] = {
    {{-7, 1, 50}, {-25, 1, 69},{69.5, 1, 61},{52, 1, 50}},
    {{-73, 1, 30},{-25, 1, 30},{25, 1, 30},{0, 1, 30}},
    {{-104, 1, 10},{-25, 1, 10},{25, 1, 10},{50, 1, 10}},
    {{-30, 1, -10},{-25, 1, -10},{25, 1, -10},{63, 1, -10}},
    {{-30, 1, -30},{-25, 1, -30},{25, 1, -30},{66, 1, -30}},
    {{-40, 1, -39}, {-25, 1, -50},{25, 1, -85},{39, 1, -58}}
};
float ctrl_x, ctrl_z, x_tenth, z_tenth;

//**********************************   F  I  S  H   ***************************************//
float fish_x;
float fish_z;
float fish_y = -120;
float fish_angle = 0;
float fish_rad = 20;

//**********************************   B  I  R  D   ***************************************//
float bird_x;
float bird_z;
float bird_y = 90;
float bird_angle = 0;
float bird_rad = 15;


//**********************************   B U I L D I N G   ***************************************//

static GLUquadricObj *building_ob;

float glass_building_height= 110;
float red_brick_height = 75;
float grey_brick_height = 50;
float white_height = 70;
float black_building_height = 30;

float glass_building = -110;
float red_brick= -75;
float grey_brick= -50;
float white_building = -70;
float black_building= -30;

static float building_place[10][3] = {
    {25,0,-100},
    {120,0, 140},
    {80,0,-135},
    {90,0, 45},
    {135,0,-70},
    {-50,0, -90},
    {-70,0,-150},
    {-35,0,0},
    {-90,0,70},
    {-100,0,-50},
};


//*******************************   S  K  Y   ***********************************//

static int fogMode = GL_LINEAR; // Fog mode.
static float fogDensity = 0.01; // Fog density.
static float fogStart = 25.0; // Fog start z value.
static float fogEnd = 2000.0; // Fog end z value.
static float alpha = 1;
bool day = true;

//********************************   T  R  E  E   *************************************//

static int tree1_maxLevel = 5; // Recursion level.
static int tree2_maxLevel = 5; // Recursion level.
static int tree1_bangle = 25; // Tree Branch Angle
static int tree2_bangle = 45; // Tree Branch Angle

// Tree Trunk Settings
static GLUquadricObj *quad_ob;
float trunk_base_rad = 1;
float trunk_top_rad = 1;
float trunk_slices = 20;
float trunk_height = 15;
float trunk_h1 = 15;
float trunk_h2 = 30;
int num_trees = 10;

//////   L  E  A  F   //////

float colors[4][3] = {
    {0,1,0},
    {.8,0.7},
    {.8,0,0},
    {.7,.4,0}
    
};

//**********************************  G R O U N D  / F L O O R  ***************************************//

float city_alpha = 0;
float ground_alpha = 1;
static float glo = 0.9;
static float sunny = 0;
static float spotAngle = 25.0;
static float spotExponent = 40.0; // Spotlight exponent = attenuation.

static float tree_place[15][3] = {
    {0, 0, 0},
    {20,0,0},
    {-35,0,0},
    {-10,0,-40},
    {-100,0,50},
    {-69,0,25},
    {-17,0,5},
    {50,0,-20},
    {125,0,-60},
    {65,0,-70},
    {5,0,-10},
    {82,0,65},
    {-90,0,67},
    {-6,0,-120},
    {-12,0,17},
};


///////////*********************   M  E  N  U   *************************///////////
float button_blue1, button_blue2, button_blue3, button_blue4, button_red1 = .9;

///////////********************* O  T  H  E  R *************************///////////

static unsigned int texture[10]; // Array of texture indices.
bool reset = false;
bool isSelecting=false;

int closestName;
static unsigned int buffer[1024]; // Hit buffer.
static int hits; // Number of entries in hit buffer.
GLsizei w=0, h=0; //width and height of the window

static int animationPeriod = 100; // Time interval between frames.

//***************************************************************************//

void resetScene(){
    meX=0;
    meY=10;
    meZ=-80;
    angle=0;
    Yangle = 0;
    glo = 0.9;
    isSelecting=false;
    
    ground_alpha = 1;
    city_alpha = 0;
    alpha = 1;
    glDisable(GL_LIGHT1);
    
     // STATE
    totheFuture = 0; // Future?
    backtoPast = 0; // Future?
    isFog = 0;
    fogEnd = 1000.0;
    isDrought = 0;
    
    trunk_height = 15;
    trunk_h1 = 15;
    trunk_h2 = 30;
    num_trees = 10;
    
    glass_building = -110;
    red_brick= -75;
    grey_brick= -50;
    white_building = -70;
    black_building= -30;
    
    lake_r = 50;
    fall_in_lake = false;
    lake_x, lake_z = 100;
    
    counter = 0;
    
    tree1_maxLevel = 5; // Recursion level.
    tree2_maxLevel = 5; // Recursion level.
    tree1_bangle = 25; // Tree Branch Angle
    tree2_bangle = 45; // Tree Branch Angle
    
    static float controlPoints[6][4][3] = {
        {{-7, 1, 50}, {-25, 1, 69},{69.5, 1, 61},{52, 1, 50}},
        {{-73, 1, 30},{-25, 1, 30},{25, 1, 30},{0, 1, 30}},
        {{-104, 1, 10},{-25, 1, 10},{25, 1, 10},{50, 1, 10}},
        {{-30, 1, -10},{-25, 1, -10},{25, 1, -10},{63, 1, -10}},
        {{-30, 1, -30},{-25, 1, -30},{25, 1, -30},{66, 1, -30}},
        {{-40, 1, -39}, {-25, 1, -50},{25, 1, -85},{39, 1, -58}}
    };
    
    makeTreeList();
}

// WRITING WORDS ON MENU

static long font = (long)GLUT_BITMAP_8_BY_13;

void writeBitmapString(void *font, char *string)
{
    char *c;
    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}


// Struct of bitmap file.
struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char *data;
};

// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
    BitMapFile *bmp = new BitMapFile;
    unsigned int size, offset, headerSize;
    
    // Read input file name.
    ifstream infile(filename.c_str(), ios::binary);
    
    // Get the starting point of the image data.
    infile.seekg(10);
    infile.read((char *) &offset, 4);
    
    // Get the header size of the bitmap.
    infile.read((char *) &headerSize,4);
    
    // Get width and height values in the bitmap header.
    infile.seekg(18);
    infile.read( (char *) &bmp->sizeX, 4);
    infile.read( (char *) &bmp->sizeY, 4);
    
    // Allocate buffer for the image.
    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];
    
    // Read bitmap data.
    infile.seekg(offset);
    infile.read((char *) bmp->data , size);
    
    // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i+2];
        bmp->data[i+2] = temp;
    }
    
    return bmp;
}

// Load external textures.
void loadExternalTextures()
{
    // Local storage for bmp image data.
    BitMapFile  *image[10];
    
    // Load the textures.
    image[0] = getBMPData("dale_TEXTURES/sky.bmp");
    image[1] = getBMPData("dale_TEXTURES/cherrywood.bmp");
    image[2] = getBMPData("dale_TEXTURES/big_grass.bmp");
    image[3] = getBMPData("dale_TEXTURES/nightsky.bmp");
    image[4] = getBMPData("dale_TEXTURES/seamless_ground_texture.bmp");
    image[5] = getBMPData("dale_TEXTURES/red_brick.bmp");
    image[6] = getBMPData("dale_TEXTURES/grey_brick.bmp");
    image[7] = getBMPData("dale_TEXTURES/glass_windows.bmp");
    image[8] = getBMPData("dale_TEXTURES/red_windows.bmp");
    image[9] = getBMPData("dale_TEXTURES/citypaving1.bmp");
    
    // Bind day sky image to texture index[0]
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
    
    // TERE BARK
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
    
    // GRASS
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);
    
    // NIGHT SKY
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);
    
    
    // DESERT
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);
    
    
    // RED BRICK
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[5]->sizeX, image[5]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[5]->data);
    
    // GREY BRICK
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[6]->sizeX, image[6]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[6]->data);
    
    // GLASS BRICK
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[7]->sizeX, image[7]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[7]->data);
    
    // RED WINDOWS
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[8]->sizeX, image[8]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[8]->data);
    
    // CITY GROUND
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[9]->sizeX, image[9]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[9]->data);
    
}

// Sequel class.
class Sequel
{
public:
    Sequel() { coords.clear(); v.clear(); }
    // a Koch snowflake or variant Koch snowflake.
    void drawTree(); // Routine to draw Sequel object in case it represents a tree.
    friend class Source;
    
private:
    vector<float> coords; // Vector of x, y co-ordinates of points specifying sequel object.
    vector<Source> v; // Vector of associated source objects to be produced recursively.
};

// Routine to draw Sequel object in case it represents a tree.
void Sequel::drawTree()
{
    glBegin(GL_LINE_STRIP);
    for (int i=0; i < 3; i++){
        glVertex3f(coords[2*i], coords[2*i+1], 0);
    }
    glEnd();
}

// Source class.
class Source
{
public:
    Source(){ }
    Source(float coordsVal[4])
    {
        for (int i=0; i < 4; i++) coords[i] = coordsVal[i];
    }
    
    void draw(); // Routine to draw source line segment.
    
    Sequel sourceToSequelTree( int branch_angle); // Routine to generate Sequel object in case of tree.
    void produceTree(int level, int maxLevel, int branch_angle); // Recursive routine to produce tree.
    
    friend class Sequel;
    
private:
    float coords[6]; // x, y co-ordinates of a line segment endpoints.
};

// Routine to draw source line segment.
void Source::draw()
{
    glBegin(GL_LINES);
    for (int i=0; i < 2; i++){
        glVertex2f(coords[2*i], coords[2*i+1]);
    }
    glEnd();
    
            quad_ob = gluNewQuadric();
            glPushMatrix();
            glRotatef(45, 0, 1, 0);
            glPushMatrix();
            glTranslatef(coords[0], 15, 0);
            glRotatef(90, 1, 0, 0);
    
            gluQuadricTexture(quad_ob, true);
            glBindTexture(GL_TEXTURE_2D, texture[1]);
            gluCylinder(quad_ob, trunk_base_rad, trunk_top_rad, trunk_height, trunk_slices, 5);
    
            glPopMatrix();
            glPopMatrix();
}

// Routine to generate Sequel object in case of tree.
Sequel Source::sourceToSequelTree(int branch_angle)
{
    float x0, y0, x1, y1, coordsVal[10], coordsVal1[4];
    float angle = branch_angle;
    int i, j;
    Source s;
    Sequel seq = *new Sequel();
    
    x0 = coords[0]; y0 = coords[1]; x1 = coords[2]; y1 = coords[3];
    
    // From the co-ordinates of the two segment endpoints calculate the co-ordinates of
    // the 3 vertices of the sub-branch V-shape.
    coordsVal[0] = x1 + RATIO*cos((PI/180.0)*angle/2.0)*(x1-x0) - RATIO*sin((PI/180.0)*angle/2.0)*(y1-y0);
    coordsVal[1] = y1 + RATIO*cos((PI/180.0)*angle/2.0)*(y1-y0) + RATIO*sin((PI/180.0)*angle/2.0)*(x1-x0);
    coordsVal[2] = x1;
    coordsVal[3] = y1;
    coordsVal[4] = x1 + RATIO*cos((PI/180.0)*angle/2.0)*(x1-x0) + RATIO*sin((PI/180.0)*angle/2.0)*(y1-y0);
    coordsVal[5] = y1 + RATIO*cos((PI/180.0)*angle/2.0)*(y1-y0) - RATIO*sin((PI/180.0)*angle/2.0)*(x1-x0);
    
    // Enter V-shape vertices into sequel object.
    for (i=0; i < 6; i++) seq.coords.push_back(coordsVal[i]);
    
    // Specify both sub-branch segments of the V-shape for recursive production.
    for (i=0; i < 2; i++)
    {
        coordsVal1[0] = coordsVal[2];
        coordsVal1[1] = coordsVal[3];
        
        for (j=2; j < 4; j++)
        {
            coordsVal1[j] = coordsVal[4*i+j-2];
        }
        s = *new Source(coordsVal1);
        seq.v.push_back(s);
    }
    
    return seq;
}

// Routine to draw leaf called by the following routine to produce tree.
void drawLeaf(float x1, float y1, float x2, float y2)
{
    // random color
    glColor3fv(colors[rand()%4]);
    
    // In a drought, there are less leaves
    if (isDrought){
        glPushMatrix();
        glTranslatef(x1, y1, 0);
        glRotatef(rand()%360, 0.0, 0.0, 1.0);
        
        glBegin(GL_QUADS);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 2.0);
        glVertex2f(0.0, 4.0);
        glVertex2f(-1.0, 2.0);
        glEnd();
        
        glPopMatrix();
    
    }
    else{
     // LOOP TO CREATE A TREE FILLED WITH LEAVES
    for (int y = y1; y <= y2; y=y+2){
        for (int x = -x1 ; x <=x2; x++){
            glPushMatrix();
            glTranslatef(x, y, 0);
            glRotatef(rand()%360, 0.0, 0.0, 1.0);
            glBegin(GL_QUADS);
            glVertex2f(0.0, 0.0);
            glVertex2f(1.0, 2.0);
            glVertex2f(0.0, 4.0);
            glVertex2f(-1.0, 2.0);
            glEnd();
            glPopMatrix();
        }
    }
    }
    glColor3f(1.0, 1.0, 1.0);
}

// Recursive routine to produce tree. takes in level, maxlevel, and angle
void Source::produceTree(int level, int maxLevel, int branch_angle)
{
    glColor3f(0.4, 0.5, 0.5);
    
    // Branches are thinner up the tree.
    glLineWidth(2*(maxLevel - level));
    
    // Source and sequels at all prior levels are drawn (different from Kock and Koch variant).
    if (maxLevel == 0) this->draw();
    else if (maxLevel == 1) {this->draw(); this->sourceToSequelTree(branch_angle).drawTree();}
    else if (level < maxLevel)
    {
        if (level == 0) this->draw();
        this->sourceToSequelTree(branch_angle).drawTree();
	       for (int i=0; i < 2; i++) {
               this->sourceToSequelTree(branch_angle).v[i].produceTree(level+1, maxLevel, branch_angle);
               //drawLeaf(this->sourceToSequelTree().coords[0], this->sourceToSequelTree().coords[1]);
               drawLeaf(this->sourceToSequelTree(branch_angle).coords[0], this->sourceToSequelTree(branch_angle).coords[1], this->sourceToSequelTree(branch_angle).coords[2], this->sourceToSequelTree(branch_angle).coords[3]);
               
               drawLeaf(this->sourceToSequelTree(branch_angle).coords[2], this->sourceToSequelTree(branch_angle).coords[3], this->sourceToSequelTree(branch_angle).coords[4], this->sourceToSequelTree(branch_angle).coords[5]);
               
           }
    }
    
    // Embellish with leaves.
    if (level == maxLevel-1)
    {
        drawLeaf(this->sourceToSequelTree(branch_angle).coords[0], this->sourceToSequelTree(branch_angle).coords[1], this->sourceToSequelTree(branch_angle).coords[2], this->sourceToSequelTree(branch_angle).coords[3]);
        
        drawLeaf(this->sourceToSequelTree(branch_angle).coords[2], this->sourceToSequelTree(branch_angle).coords[3], this->sourceToSequelTree(branch_angle).coords[4], this->sourceToSequelTree(branch_angle).coords[5]);
        
    }
    
    // Restore line width.
    glLineWidth(1.0);
}



// Display list for the 2 types of trees
void makeTreeList(){
    
    aTree = glGenLists(2); // Return a list index.
    
    // Begin create a display list.
    glNewList(aTree, GL_COMPILE);
    
    // Draw a tree.
    float coordsVal4[4] = {0.0, 0.0, 0.0, trunk_h1};
    Source src4 = *new Source(coordsVal4); // Vertical line segment.
    src4.produceTree(0, tree1_maxLevel, tree1_bangle);
    glPushMatrix();
    glRotatef(60, 0, 1, 0);
    src4.produceTree(0, tree1_maxLevel, tree1_bangle);
    glPopMatrix();
    glEndList();
    
    // Begin create a display list.
    glNewList(aTree+1, GL_COMPILE);
    // Draw a tree.
    float coordsVal2[4] = {0.0, 0.0, 0.0, trunk_h2};
    Source src2 = *new Source(coordsVal2); // Vertical line segment.
    src4.produceTree(0, tree2_maxLevel, tree2_bangle);
    glPushMatrix();
    glRotatef(60, 0, 1, 0);
    src4.produceTree(0, tree2_maxLevel,tree2_bangle );
    glPopMatrix();
    glEndList();
    
}

// display list for a window
void makeYellowWindow(){
    
    aWindow = glGenLists(1); // Return a list index.
    
    // Begin create a display list.
    glNewList(aWindow, GL_COMPILE);
    
    glScalef(1.5, 2, .5);
    glutSolidCube(2.0);
    // Draw a window
    glEndList();

}


// display list for the 5 types of buildings in the environment
void makeBuildingList(){
    
    aBuilding = glGenLists(5); // Return a list index.
    
    glNewList(aBuilding, GL_COMPILE);
    // building 1
    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2); //dark gray
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glScalef(1, 2, 1);
    glutSolidCube(black_building_height);
    glPopMatrix();

    // make all the windows for building #1
    glMaterialfv(GL_FRONT, GL_EMISSION, starSpec);
    glPushMatrix();
    glTranslatef(-8, 0, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 0, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8, 0, -15);
    glCallList(aWindow);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-8, 20, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 20, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8, 20, -15);
    glCallList(aWindow);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-8, 10, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 10, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8, 10, -15);
    glCallList(aWindow);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-8, -10, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, -10, -15);
    glCallList(aWindow);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8, -10, -15);
    glCallList(aWindow);
    glPopMatrix();
    
    glPopMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glEndList();  // end building 1
    
    // make more building, but using gluQuadrics to make texture mapping easier
    glNewList(aBuilding+1, GL_COMPILE);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    building_ob =gluNewQuadric();
    gluQuadricTexture(building_ob, true);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    gluCylinder(building_ob, 25, 25, red_brick_height, 4, 30);
    glPopMatrix();
    glEndList(); // end building 2
    
    glNewList(aBuilding+2, GL_COMPILE);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    building_ob =gluNewQuadric();
    gluQuadricTexture(building_ob, true);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    gluCylinder(building_ob, 15, 15, grey_brick_height, 6, 30);
    glPopMatrix();
    glEndList(); // end building 3
    
    glNewList(aBuilding+3, GL_COMPILE);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    building_ob =gluNewQuadric();
    gluQuadricTexture(building_ob, true);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    gluCylinder(building_ob, 20, 20, glass_building_height, 4, 30);
    glPopMatrix();
    glEndList(); // end building 4
    
    glNewList(aBuilding+4, GL_COMPILE);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    glRotatef(-90, 1, 0, 0);
    building_ob =gluNewQuadric();
    gluQuadricTexture(building_ob, true);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    gluCylinder(building_ob, 30, 30, white_height, 4, 30);
    glPopMatrix();
    glEndList(); // end building 5

}

// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
    par_sys[i].alive = true;
    par_sys[i].life = 10.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;
    par_sys[i].xpos = (float) (rand() % 100) -lake_r;   // make bubbles are in the lake
    par_sys[i].zpos = (float) (rand() % 80) + lake_z;
    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;
    
    par_sys[i].vel = velocity;
    
    // moves the bubbles closer to the user as the user gets deeper into lake
    if (meY < -140){
        par_sys[i].gravity = -1;
        par_sys[i].ypos = (float) (rand() % 20) - 100;
    }

    else{
        par_sys[i].gravity = -8;//-0.8;
        par_sys[i].ypos = (float) (rand() % 50) - 50;
    }
}

// draw bubbles in the lake
void drawBubbles() {
    float x, y, z;
    float matShine[] = { 90.0 };
    
    for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
        if (par_sys[loop].alive == true) {
            x = par_sys[loop].xpos;
            y = par_sys[loop].ypos;
            z = par_sys[loop].zpos + zoom;
            
            // draw 2 types of bubbles, that are shiny and a little transparent
            glColor4f(0.0, 0.0, 0.5, .9);
            glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
            glPushMatrix();
            glTranslatef(x, y, z);
            glutSolidSphere(0.5, 16, 16);
            glPopMatrix();
            
            glColor4f(0.7, 0.7, 0.7, .3);
            glPushMatrix();
            glTranslatef(x+5, y-5, z+5);
            glutSolidSphere(0.25, 16, 16);
            glPopMatrix();
            
            // Update values
            //Move
            if (par_sys[loop].ypos <= -180) {
                par_sys[loop].vel = par_sys[loop].vel * -1.0;
            }
            par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000); // * 1000
            par_sys[loop].vel += par_sys[loop].gravity;
            
            // Decay
            par_sys[loop].life -= par_sys[loop].fade;
            
            //Revive
            if (par_sys[loop].life < 0.0) {
                initParticles(loop);
            }
        }
    }
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

}

// draw a bird, and its shadow if neccessary
void drawBird(int shadow){

    glPushMatrix();
    
    if (shadow) glColor4f(0, 0, 0, 1);
    else glColor4f(.6, 0, 0, 1);
    
    glRotatef(bird_angle, 0, 1, 0);
    glTranslatef(bird_x, bird_y, bird_z);
    glTranslatef(0, 20, -50);
    glRotatef(90, 1, 0, 0);
    
    /// Bird Body
    glPushMatrix(); // plane scale
    glScalef(3.0, 1.0, 1.0);
    glutSolidSphere(2.0, 40, 40);
    glPopMatrix(); // plane scale
    
    // WINGS
    if (shadow) glColor3f(0, 0, 0);
    else glColor3f(1, .5, 0);
    glBegin(GL_TRIANGLES);
    glVertex3f(2, -2, 0);
    glVertex3f(-2, -2, 0);
    glVertex3f(3, -10, 0);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(2, 2, 0);
    glVertex3f(-2, 2, 0);
    glVertex3f(3, 10, 0);
    glEnd();
    
     // BECK
    glPushMatrix();
    if (shadow) glColor3f(0, 0, 0);
    else glColor3f(1, 1, 0);
    glTranslatef(-5, 0, 0);
    glRotatef(-90, 0, 1, 0);
    glutSolidCone(1, 3, 15, 15);
    glPopMatrix();
    glPopMatrix();
    glColor3f(1, 1, 1);
}

// draw all the buildings
void drawBuilding(){
    int tr = 0;
    glColor3f(0.0, 0.0, 0.0);
    for (int a = 0; a < 10; a++){
        glPushMatrix();
        glTranslatef(building_place[a][0], 0, building_place[a][2]);
        if (tr % 5 == 0){
            glPushMatrix();
            glTranslatef(0, red_brick, 0);
            glCallList(aBuilding+1);
            glPopMatrix();
        }
        else if(tr % 5 == 1){
            glPushMatrix();
            glTranslatef(0, grey_brick, 0);
            glCallList(aBuilding+2);
            glPopMatrix();
        }
        else if(tr % 5 == 2){
            glPushMatrix();
            glTranslatef(0, glass_building, 0);
            glCallList(aBuilding+3);
            glPopMatrix();
        }
        else if(tr % 5 == 3){
            glPushMatrix();
            glTranslatef(0, white_building, 0);
            glCallList(aBuilding+4);
            glPopMatrix();
        }
        else{
            glPushMatrix();
            glTranslatef(0, black_building, 0);
            glCallList(aBuilding);
            glPopMatrix();
        }
        glPopMatrix();
        tr++;
    }
    glFlush();
}

// draw a fish in the lake
void drawFish(){
    
    glPushMatrix();
    glTranslatef(lake_x, 0, lake_z);
    glRotatef(-fish_angle, 0, 1, 0);
    glTranslatef(fish_x, fish_y, fish_z);   // makes fish follow a circle path. see idle function.
    
    // eye ball
    glPushMatrix();
    glTranslatef(-1, 6, 0);
    glColor4f(0, 0, 0, 1);
    glutSolidSphere(1, 20, 20);
    glColor4f(1, 1, 1, .5);
    glutSolidSphere(2, 20, 20);
    glPopMatrix();
    
    //body
    glColor3f(.7, 0, 1);
    glBegin(GL_TRIANGLES);
    glVertex3f(10, 0, 0);
    glVertex3f(10, 10, 0);
    glVertex3f(-10, 5, 0);
    glEnd();
    
    glColor3f(1, .5, 0);
    glBegin(GL_TRIANGLES);
    glVertex3f(3, 3, -1);
    glVertex3f(3, 6, -1);
    glVertex3f(8, 5, -1);
    glEnd();
    
    glColor3f(.5, 0, .5);
    glBegin(GL_TRIANGLES);
    glVertex3f(15, 2, 0);
    glVertex3f(15, 8, 0);
    glVertex3f(10, 5, 0);
    glEnd();
    
    glPopMatrix();
    glColor3f(1, 1, 1);

}

// draw the ground and maps different textures to the ground
void drawFloor(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    
    // desert floor built on the bottom
    glBlendFunc(GL_ONE, GL_ZERO); // Specify blending parameters to overwrite background.
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(250.0, 0.0, -250.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-250.0, 0.0, -250.0);
    glEnd();
    
    // grass floor. ground_alpha is initilzed to 1.
    glColor4f(1.0, 1.0, 1.0, ground_alpha);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(250.0, 0.0, -250.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-250.0, 0.0, -250.0);
    glEnd();
    
    // city floor. city_alpha is initilzed to 0.
    glColor4f(1.0, 1.0, 1.0, city_alpha);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(250.0, 0.0, 250.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(250.0, 0.0, -250.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-250.0, 0.0, -250.0);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    
    // draw bird shadow
    glPushMatrix();
    glScalef(1.2, 0.05, 1.2);
    drawBird(1);
    glPopMatrix();
}

// draw fog
void drawFog(){
    float fogColor[4] = {0.5, 0.5, 0.5, 1.0};
    // Fog controls.
    if (isFog) glEnable(GL_FOG);
    else glDisable(GL_FOG);
    
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, fogMode);
    glFogf(GL_FOG_START, fogStart);
    glFogf(GL_FOG_END, fogEnd);
    glFogf(GL_FOG_DENSITY, fogDensity);
}

// draw lake
void drawLake(){
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
    glPushMatrix();
    glTranslatef(lake_x, 0, lake_z);
    // Specify and enable the Bezier surface.
    // build 2 lakes with blending to change surface in the dark and light
    //  Bezier Surface - Lake Surface
    glColor3f(0, 0, .8);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPoints[0][0]);
    glEnable(GL_MAP2_VERTEX_3);
    // Draw the Bezier surface using a mesh approximation.
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    glEvalMesh2(GL_FILL, 0, 20, 0, 20);
    
    glColor4f(.7, .7, 1, alpha);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPoints[0][0]);
    glEnable(GL_MAP2_VERTEX_3);
    // Draw the Bezier surface using a mesh approximation.
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    glEvalMesh2(GL_FILL, 0, 20, 0, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glEnable(GL_DEPTH_TEST);
    
    // bottom of the lake
    glPushMatrix();
    glTranslatef(lake_x, -190, lake_z);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);
    for(float z = 100.0; z > -100.0; z -= 1.0)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(float x = -100.0; x < 100.0; x += 1.0)
        {
            glColor3f(0, .2, .7);
            glNormal3f(0.0, 1.0, 0.0);
            glVertex3f(x, 0.0, z - 5.0);
            glVertex3f(x, 0.0, z);
        }
        glEnd();
    }
    glPopMatrix();
    
    // Cylinder of the lake
    glColor3f(0, .2, .7);
    glPushMatrix();
    glTranslatef(lake_x, 0,lake_z);
    glRotatef(90, 1, 0, 0);
    lake_ob = gluNewQuadric();
    gluCylinder(lake_ob, lake_r, 80, 200, 50, 5);
    glPopMatrix();
    
    glPopMatrix();
    
}

// draw sky
void drawSky(){
    
    drawFog();
    glDisable(GL_LIGHTING);
    
    // 1. used sphere mapping.
    // 2. used blending to transition from day to night and back
    GLUquadricObj *sky_ob = gluNewQuadric();
    
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    gluSphere(sky_ob, 350, 50, 50);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    
    glColor4f(1.0, 1.0, 1.0, alpha);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Specify blending parameters to mix skies.
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    gluSphere(sky_ob, 350, 50, 50);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glDisable(GL_FOG);
}

// draw trees
void drawTrees(int num_trees){
    glColor3f(0.0, 0.0, 0.0);
    for (int a = 0; a < num_trees; a++){
        glPushMatrix();
        glTranslatef(tree_place[a][0], 0, tree_place[a][2]);
        glCallList(aTree);
        glTranslatef(tree_place[a][0], 0, tree_place[a][2]-70);
        glCallList(aTree+1);
        glPopMatrix();
    }
    glFlush();
}


void setPointOfView()
{
    gluLookAt(meX,meY, meZ, meX+sin(angle*PI/180)*cos(Yangle*PI/180), meY+sin(Yangle*PI/180), meZ+cos(angle*PI/180)*cos(Yangle*PI/180), 0, 1, 0);
   
    // stops user from walking over the edge of the world
    if (meX > 225 || meX <-225 || meZ > 225 || meZ <-225){
        stopwalking = true;
        
        cout <<" - - - - - - - - - - - - - - - - - -"<<endl;
        cout <<"PLEASE BE CAREFUL" <<endl;
        cout <<"YOU ARE NEAR THE EDGE OF THE WORLD" <<endl;
        cout <<"PRESS 'W' TO START WALKING AGAIN" <<endl;
        cout <<" - - - - - - - - - - - - - - - - - -" <<"\n"<<endl;
    }
    else
        stopwalking = false;
    
    // checks if the user is falling into the lake
    if (meX < lake_x+lake_r && meX > lake_x - lake_r && meZ < lake_z+lake_r && meZ > lake_z - lake_r){
            if (swimming == false)
                fall_in_lake = true;
            else
                fall_in_lake = false;
            
            cout <<" - - - - - - - - - - - - - - - - - -"<<endl;
            cout <<"YOU FELL IN THE LAKE" <<endl;
            cout <<" - - - - - - - - - - - - - - - - - -" <<"\n"<<endl;
        }
}

void setProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,(float)w/(float)h,1,700);
    glMatrixMode(GL_MODELVIEW);
}

void drawAll(){
    
    setPointOfView();
    glEnable(GL_NORMALIZE);
    float lightPos0[] = {lake_x, -10.0, lake_z, 1.0 }; // Spotlight position.
    float lightPos1[] = {0, 30.0, 0, 1.0 }; // Spotlight position.
    float spotDirection[] = {0.0, -1.0, 0.0}; // Spotlight direction.
    float globAmb[] = { glo, glo, glo, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    
    // Spotlight properties for lake light.
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
    
    // Spotlight properties for drought light.
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExponent);
    
    drawSky();
    drawBird(0); // draw bird w/ no shadow
    drawFloor();
    drawLake();
    drawBuilding();
    if (fall_in_lake) // only draw bubbles after user falls into lake
        drawBubbles();

    drawFish();
    drawTrees(num_trees);
    
}

// Initialization routine for environment. Window 1.
void environment_setup(void)
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
    
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);
    
    // Light property vectors.
    float lightAmb[] = { 1, 1, 1, 1.0 };
    float lightAmb1[] = { sunny, sunny, sunny, 1.0 };
    float lightDifAndSpec0[] = { .7, .7, .7, 1.0 };
    float lightDifAndSpec1[] = { sunny, sunny, sunny, 1.0 };
    float globAmb[] = { glo, glo, glo, 1.0 };
    
    // Light0 properties. SPOTLIGHT
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec0);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint
    
    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float matShine[] = { 1.0 };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Create texture index array and load external textures.
    glGenTextures(10, texture);
    loadExternalTextures();
    
    // Turn on OpenGL texturing.
    glEnable(GL_TEXTURE_2D);
    
    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glEnable(GL_BLEND); // Enable blending.
    
    // Specify automatic texture generation for a sphere map.
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_AUTO_NORMAL);
    
    // make display lists
    makeTreeList();
    makeYellowWindow();
    makeBuildingList();
    
    // Initialize particles for bubbles
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        initParticles(loop);
    }
}

// Drawing routine.
void drawScene(void)
{
    glutSetWindow(id1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    
    if (reset){
            resetScene();
        }
    reset = false;
    setProjection();
    drawAll();
    
    glutSwapBuffers();
}


//************ MENU STUFF *************//

// FIND CLOSEST HIT GROUND OBJECT
void findClosestHit(int hits, unsigned int buffer[])
{
    unsigned int *ptr, minZ;
    
    minZ= 0xffffffff; // 2^32 - 1
    ptr = buffer;
    closestName = 0;
    for (int i = 0; i < hits; i++)
    {
        ptr++;
        if (*ptr < minZ)
        {
            minZ = *ptr;
            ptr += 2;
            closestName = *ptr;
            ptr++;
        }
        else ptr += 3;
    }
}

void menuButtons(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if (isSelecting) glLoadName(1);    // drought button
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, button_blue1);
    glVertex3f(-15.0, 20.0, 0.0);
    glVertex3f(-15.0, 30.0, 0.0);
    glVertex3f(15.0, 30.0, 0.0);
    glVertex3f(15.0, 20.0, 0.0);
    glEnd();
    
    if (isSelecting) glLoadName(2);     // air pollution button
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, button_blue2);
    glVertex3f(-15.0, 0.0, 0.0);
    glVertex3f(-15.0, 10.0, 0.0);
    glVertex3f(15.0, 10.0, 0.0);
    glVertex3f(15.0, 0.0, 0.0);
    glEnd();
    
    if (isSelecting) glLoadName(3);    // future button
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, button_blue3);
    glVertex3f(-15.0, -20.0, 0.0);
    glVertex3f(-15.0, -10.0, 0.0);
    glVertex3f(15.0, -10.0, 0.0);
    glVertex3f(15.0, -20.0, 0.0);
    glEnd();
    
    if (isSelecting) glLoadName(4);     // past button
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, button_blue4);
    glVertex3f(-15.0, -40.0, 0.0);
    glVertex3f(-15.0, -30.0, 0.0);
    glVertex3f(15.0, -30.0, 0.0);
    glVertex3f(15.0, -40.0, 0.0);
    glEnd();
    
    if (isSelecting) glLoadName(5);     // reset button
    glBegin(GL_POLYGON);
    glColor3f(button_red1, 0.0, 0);
    glVertex3f(-45.0, 20.0, 0.0);
    glVertex3f(-45.0, 30.0, 0.0);
    glVertex3f(-20.0, 30.0, 0.0);
    glVertex3f(-20.0, 20.0, 0.0);
    glEnd();
    
    glColor3f(1.0, 1.0, 1);
    glRasterPos3f(-5.0, 25.0, 5.0);
    writeBitmapString((void*)font, "DROUGHT");
    
    glRasterPos3f(-10.0, 4.0, 5.0);
    writeBitmapString((void*)font, "AIR POLLUTION");
    
    glRasterPos3f(-12.0, -14.5, 5.0);
    writeBitmapString((void*)font, "INTO THE FUTURE");
    
    glRasterPos3f(-12.0, -34.5, 5.0);
    writeBitmapString((void*)font, "BACK TO THE PAST");
    
    glRasterPos3f(-36.0, 25.0, 5.0);
    writeBitmapString((void*)font, "RESET");
    
    glLineWidth(1.0);  //restore width and color
    glColor3f(0.0,0.0,0.0);
    
    if (isSelecting) glPopName();
}

// Initialization routine for menu.
void menu_setup(void)
{
    //white background
    glClearColor(1, 1, 1, 1.0);
    glEnable(GL_DEPTH_TEST);
}

void drawMenu(){
    
    glutSetWindow(id2);
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0,50.0, -50.0,50.0, -5.0,5.0);
    
    // Title of the Menu
    glColor3f(0.0, 0.0, 0);
    glRasterPos3f(-25.0, 40.0, 0.0);
    writeBitmapString((void*)font, "MAKE CHANGES TO YOUR ENVIRONMENT");
    
    isSelecting = 0; // for picking and selecting
    menuButtons();
    glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    w = width;
    h=height;
    setProjection();
}

// Timer function.
void animate(int value)
{
    // AIR POLLUTION
    if (isFog){
        glDisable(GL_LIGHT1);
        
        if (fogEnd > 100)
            fogEnd = fogEnd-50;
    }
    
    // FUTURE
    if (totheFuture)
    {
        glDisable(GL_LIGHT1);
        if (tree2_maxLevel > 0){
            tree2_maxLevel = tree2_maxLevel - 1;
            makeTreeList();
            
        }
        else if (tree1_maxLevel > 0){
            tree1_maxLevel = tree1_maxLevel - 1;
            makeTreeList();}
        if (trunk_h1 > 0){
            trunk_h1 = trunk_h1 - 1;
            makeTreeList();
        }
        if (trunk_h2 > 0){
            trunk_h2 = trunk_h2 - 1;
            makeTreeList();
        }
        else if (trunk_height > 0){
            trunk_height = trunk_height -1;
            makeTreeList();
        }
        
        if (city_alpha<1)
            city_alpha = city_alpha + .1;
        
        
        if (black_building< 30){
            black_building = black_building + 5;
        }
        else if (white_building < 0)
            white_building = white_building + 3;
        if (red_brick < 0)
            red_brick = red_brick + 2;
        if (grey_brick < 0)
            grey_brick = grey_brick + 5;
        else if(glass_building < 0)
            glass_building = glass_building + 10;
        
    }
    
    // PAST
    if (backtoPast)
    {
        glDisable(GL_LIGHT1);
        
        if (ground_alpha < 1)
            ground_alpha = ground_alpha + .1;
        else if (city_alpha > 0)
            city_alpha = city_alpha - .1;
        
        if (black_building > -30 ){
            black_building = black_building - 5;
        }
        else if (white_building > -70)
            white_building = white_building - 3;
        
        if (red_brick > -75)
            red_brick = red_brick - 2;
        if (grey_brick > -50)
            grey_brick = grey_brick - 5;
        else if(glass_building > -110)
            glass_building = glass_building - 10;
        else {
            
            if (num_trees < 10)
                num_trees += 1;
            
            if (tree2_maxLevel < 5){
                tree2_maxLevel = tree2_maxLevel + 1;
                makeTreeList();
                
            }
            else if (tree1_maxLevel < 3){
                tree1_maxLevel = tree1_maxLevel -+1;
                makeTreeList();}
            if (trunk_h1 < 15){
                trunk_h1 = trunk_h1 + 1;
                makeTreeList();
            }
            if (trunk_h2 < 30){
                trunk_h2 = trunk_h2 + 1;
                makeTreeList();
            }
            else if (trunk_height < 15){
                trunk_height = trunk_height +1;
                makeTreeList();
            }
        }
    }
    
    // DROUGHT
    if (isDrought){
        glo = 1;
        trunk_height = 15;
        makeTreeList();
        glEnable(GL_LIGHT1);
        
        // show desert ground
        if (sunny < 1)
            sunny = sunny +.05;
        if (lake_r > 5)
            lake_r = lake_r - 5;
        
        if (ground_alpha > 0)
            ground_alpha = ground_alpha - .1;
        else if (city_alpha > 0)
            city_alpha = city_alpha - .1;
        
        // cut down trees
        if (num_trees > 2)
            num_trees -= 1;
        
        // Make Lake Smaller
        if (counter < 20){
            for (int a = 0; a < 6; a++){
                for(int b = 0; b < 4; b++){
                    ctrl_x = controlPoints[a][b][0];
                    ctrl_z = controlPoints[a][b][2];
                    
                    x_tenth = ctrl_x*(.1);
                    z_tenth = ctrl_z * (.1);
                    
                    controlPoints[a][b][0] = ctrl_x - x_tenth;
                    controlPoints[a][b][2]=ctrl_z - z_tenth;
                }
            }
            counter = counter +1;
        }
    }
    
    // make user fall in the lake
    if(fall_in_lake){
        if (meY > -60)
            meY = meY - 10;
        else if (meY > -100)
            meY = meY - 5;
        else if (meY > -160)
            meY = meY - 2;
        
    }
    glutTimerFunc(animationPeriod, animate, 1);
    glutPostRedisplay();
}

// make button color darker when mouse hovers over it.
void mouseRoll(int roll_x, int roll_y){
    glutSetWindow(id2);
    double x_world, y_world, xsc, ysc;
    
    // convert screen coords to world coords
    xsc = roll_x;
    ysc = h-roll_y;
    x_world = xsc * (.2) - 50;
    y_world= ysc*(.2)- 50;
    

    if (x_world < 15 && x_world >-15 && y_world > 20 && y_world < 30){
        button_blue1 = .5;
    
    }
    else if (x_world < -20 && x_world >-50 && y_world > 20 && y_world < 30){
        button_red1 = .5;
        
    }
    else if (x_world < 15 && x_world >-15 && y_world > 00 && y_world < 10){
        button_blue2 = .5;
        
    }
    else if (x_world < 15 && x_world >-15 && y_world > -20 && y_world < -10){
        button_blue3 = .5;
        
    }
    else if (x_world < 15 && x_world >-15 && y_world > -40 && y_world < 1-30){
        button_blue4 = .5;
        
    }
    else{
        button_blue1 = .9;
        button_blue2 = .9;
        button_blue3 = .9;
        button_blue4 = .9;
        button_red1 = .9;
    }
    glutPostRedisplay();
}

// selecting button on menu
void mouseControl(int button, int state, int x, int y) {
    int viewport[4];
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        //cout << " in mourse control " << endl;
        
        glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.
        glSelectBuffer(1024, buffer); // Specify buffer to write hit records in selection mode
        (void) glRenderMode(GL_SELECT); // Enter selection mode.
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        
        // Define a viewing volume corresponding to selecting in
        //    3 x 3 region around the cursor.
        glLoadIdentity();
        gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);
        glOrtho(-50.0, 50.0, -50.0, 50.0, -5.0,5.0);
        
        glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
        glLoadIdentity();  //I do this later, in setPointOfView
        
        glInitNames(); // Initializes the name stack to empty.
        glPushName(0); // Puts name 0 on top of stack.
        
        // Determine hits by drawing so that names are assigned.
        isSelecting = 1;
        menuButtons();
        
        hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.
        
        // Determine closest of the hit objects (if any).
        // closest object name will be saved in closestName.
        findClosestHit(hits, buffer);
        if (closestName == 1){      // DOUGHT
            totheFuture = false;
            backtoPast = false;
            isDrought = true;
            glutSetWindow(id1);
            glutPostRedisplay();
        }
        else if (closestName == 2){     // FOG
            isFog = true;
            glutSetWindow(id1);
            glutPostRedisplay();
        }
        else if (closestName == 3){     // FUTURE
            totheFuture = true;
            backtoPast = false;
            isDrought = false;
            glutSetWindow(id1);
            glutPostRedisplay();
        }
        else if (closestName == 4){     // PAST
            totheFuture = false;
            backtoPast = true;
            isDrought = false;
            glutSetWindow(id1);
            glutPostRedisplay();
        }
        else if (closestName == 5){     // RESET
            reset = true;
            glutSetWindow(id1);
            glutPostRedisplay();
        }
    }
    glutSetWindow(id1);
    glutPostRedisplay();
}


void specialKeyInput(int key, int x, int y)
{
    switch(key){
        case GLUT_KEY_UP: //forward
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
                Yangle += turnsize;
            }
            else if(stopwalking){
                meZ = meZ;
                meX = meX;
            }
            else{
                meZ +=stepsize*cos(angle*PI/180);
                meX +=stepsize*sin(angle*PI/180);
            }
            
            break;
        case GLUT_KEY_DOWN: //back
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
                Yangle -= turnsize;
            else if(stopwalking){
                meZ = meZ;
                meX = meX;
            }
            else{
                meZ -=stepsize*cos(angle*PI/180);
                meX -=stepsize*sin(angle*PI/180);
            }
            break;
        case GLUT_KEY_RIGHT: //turn right
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
                angle -= turnsize;
            }
            else if(stopwalking){
                meZ = meZ;
                meX = meX;
            }
            else{
                meZ +=stepsize*sin(angle*PI/180);
                meX -=stepsize*cos(angle*PI/180);
            }
            
            break;
        case GLUT_KEY_LEFT: //turn left
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
                angle += turnsize;
            }
            else if(stopwalking){
                meZ = meZ;
                meX = meX;
            }
            else{
                meZ -=stepsize*sin(angle*PI/180);
                meX +=stepsize*cos(angle*PI/180);
            }
            break;
    }//end switch
    glutPostRedisplay();
}


// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'r':
            cout << "Reset Scene" << endl;
            reset = true;
            glutPostRedisplay();
            break;
        case 's':  // swimming up
            swimming = true;
            if (meY <10)
                meY = meY +3;
            glutPostRedisplay();
            break;
        case 'n':  // go into night
            if (alpha > 0){
                alpha = alpha - .1;
                if (glo > .1)
                    glo = glo - .1;
            }
            glutPostRedisplay();
            break;
        case 'd':  // night to day
            if (alpha < 1){
                alpha = alpha + .1;
                if (glo < 1)
                    glo = glo + .1;
            }
            glutPostRedisplay();
            break;
        case 'w':  // walk
            if (meX < -225)
                meX = -223;
            else if (meX > 225)
                meX = 223;
            
            if (meZ< -225)
                meZ = -223;
            else if (meZ > 225)
                meZ = 223;
            
            stopwalking = false;
            glutPostRedisplay();
            break;
        case '1':
            glEnable(GL_LIGHT0);
            glutPostRedisplay();
            break;
        case '2':
            glDisable(GL_LIGHT0);
            glutPostRedisplay();
            break;
        default:
            break;
    }
}


// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
        cout << "Interaction:" << endl;
        cout << "\tup arrow - step forward" << endl;
        cout << "\tdown arrow - step backward" << endl;
        cout << "\tleft arrow - step left" << endl;
        cout << "\tright arrow - step right" << endl;
        cout << "\tShift + up arrow - look up" << endl;
        cout << "\tShift + down arrow - look up" << endl;
        cout << "\tShift + left arrow - look left" << endl;
        cout << "\tShift + right arrow - look right" << endl;
        cout << "\tn - go to night "<<endl;
        cout << "\ts - to swim up when in the lake "<<endl;
        cout << "\td - go to day "<<endl;
        cout << "\tr - reset scene "<<endl;
        cout << "\tw - to continue walking after being stopped" << endl;
        cout << "\t1/2 keys - enable/disable lake spotlight" << endl;
        cout << "\tRight Mouse Click in Environment to show Options Menu" << endl;
    
}

void idle(){
    
    // swimming fish
    // cirle path, angle,
    fish_angle = fish_angle + 1;
    fish_x = fish_rad*cos(fish_angle*PI/180);
    fish_z = fish_rad*sin(fish_angle*PI/180);
    
    // flying bird
    bird_angle = bird_angle + 2;
    bird_x = bird_rad*cos(bird_angle*PI/180);
    bird_z = bird_rad*sin(bird_angle*PI/180);
    glutPostRedisplay();
}

// The sub-menu callback function.
void transition_state(int id)
{
    switch (id){
        case 1:
            exit(0);
            break;
        case 2:
            isDrought = true;
            totheFuture = 0;
            backtoPast = 0;
            isFog = 0;
            glutSetWindow(id1);
            
            break;
        case 3:
            isFog = true;
            totheFuture = 0;
            backtoPast = 0;
            isDrought = 0;
            glutSetWindow(id1);
            break;
        case 4:
            totheFuture = true;
            isFog = 0;
            backtoPast = 0;
            isDrought = 0;
            glutSetWindow(id1);
            break;
        case 5:
            backtoPast = true;
            totheFuture = 0;
            isFog = 0;
            isDrought = 0;
            glutSetWindow(id1);
            break;
        default:
            break;
    
    }
    glutPostRedisplay();
}

void makeOptionsMenu(void)
{
    // The sub-menu is created first (because it should be visible when the top
    // menu is created): its callback function is registered and menu entries added.
    
    // The top menu is created: its callback function is registered and menu entries,
    // including a submenu, added.
    glutCreateMenu(transition_state);
    glutAddMenuEntry("Drought", 2);
    glutAddMenuEntry("Carbon Emission", 3);
    glutAddMenuEntry("Go to the Future",4);
    glutAddMenuEntry("Back to the Past",5);
    glutAddMenuEntry("Quit",1);
    
    // The menu is attached to a mouse button.
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    
    // MAKE MENU WINDOW
    glutInitWindowPosition(700, 100);
    id2 = glutCreateWindow("Menu");
    menu_setup();
    glutDisplayFunc(drawMenu);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseControl);
    glutPassiveMotionFunc(mouseRoll);
    
    // MAKE ENVIRONMENT WINDOW
    glutInitWindowPosition(100, 100);
    id1 = glutCreateWindow("Environment");
    environment_setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);
    glutIdleFunc(idle);
    glutTimerFunc(5, animate, 1);
    makeOptionsMenu(); // RIGHT-CLICK MENU

    glutMainLoop();
    
    return 0;  
}
