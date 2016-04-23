// domino.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <windows.h>
#include <math.h>        // sin, cos
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>        // GLUT
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <mmsystem.h>

#ifndef M_PI
#define M_PI  3.1415
#endif
//-------------------------------------------------------------------------------------------------


static float angle = 0;         // ��� �������� �����
int          width=1, height=1;
int          button = -1;       // ������ ���� (-1 - �� ������, 0 - �����, 2 - ������)
float        angleX, angleY;    // ������� ���� �������� ����� 
float        mouseX, mouseY;    // ������� ���������� ����
int			 keyMod; //������� CTRL,ALT,Shift
GLdouble	GLX, GLY, GLZ;		//���������� ���� � ���������� ������� ���������
float        distZ = -17;       // ���������� �� ��� Z �� �����
float        offset_x=0;		//�������� �� X
bool         meny = true;

// ��������� �������� ���� 
float mat1_dif[]={0.8f,0.8f,0.0f};   // ���� ��������� ��������� �������� 
                                     // (��������� ���� ��'����)
float mat1_amb[] = {0.2f,0.2f,0.2f}; // �������� ���� �������� (���� �������� � ��)
float mat1_spec[] = {0.6f,0.6f,0.6f};// ���� ������������ ��������� �������� 
                                     // (����� � ����� �������� �� ���� �������)
float mat1_shininess=0.5f*128;       // ������ ������������ ��������� ��������

// ��������� �������� ������ 
float mat2_dif[] = {0.0f,0.0f,0.8f};
float mat2_amb[] = {0.2f,0.2f,0.2f};
float mat2_spec[] = {0.6f,0.6f,0.6f};
float mat2_shininess = 0.7f*128;

// ��������� �������� ��� 
float mat3_dif[] = {0.9f,0.2f,0.0f};
float mat3_amb[] = {0.2f,0.2f,0.2f};
float mat3_spec[] = {0.6f,0.6f,0.6f};
float mat3_shininess = 0.9f*128;

// ��������� �������� ���� 
float mat4_dif[] = {1.0f,1.0f,1.0f};
float mat4_amb[] = {0.2f,0.2f,0.2f};
float mat4_spec[] = {0.6f,0.6f,0.6f};
float mat4_shininess = 0.1f*128;

// ��������� �������� ��� 
float mat5_dif[] = {0.1f,0.9f,0.1f};
float mat5_amb[] = {0.2f,0.2f,0.2f};
float mat5_spec[] = {0.6f,0.6f,0.6f};
float mat5_shininess = 0.5f*128;

void draw_string_bitmap(void *font, const char* string) {
  while (*string)
    glutBitmapCharacter(font, *string++);
}

struct pointxy{
	int x,y;
};

//������������� ���������� ��������� ����������� ����� �� ������(�� ��������)
pointxy one[1]= {{0,0}},                        // 1
        two[2]= {{0,-1},{0,1}},					// 2
        three[3]= {{0,0},{-1,1},{1,-1}},		// 3
		four[4]= {{-1,-1},{1,1},{-1,1},{1,-1}}; // 4
//��� 5(1+4) � 6 (2+4) 

//���������� ���������������
void piped2(GLfloat a, GLfloat b, GLfloat c){	
glScalef (a, b, c); // ��� ����������� ���� � ��������������-������������ �� x,y,z
glutSolidCube(1.0); // ������ ���-� ���������������� - �������������� � ������ ������� 1.0;
}

/*��������� ��������� ����������� ����� �� ����������� �� �������� one,two,tree,four
len-����� �������,arr-������ ���������, x,y,z-���������� ������ ��������� ���������.
������������ ���� ����� ����� ������������� ���������� �� �������, ����������� 
�� ��������� ����������� offset.*/
void FillFromArray(int len,pointxy arr[],float x,float y,float z,float offset){
for (int i=0;i<len;i++){
   glPushMatrix();
   glTranslatef(x+arr[i].x*offset,y+arr[i].y*offset,z);
   glutSolidSphere (0.1f, 20, 20);
   glPopMatrix();
}
}

/*��������� �������� ���������
��������� ����� 5 ������������ ���������������� ������� �������
FillFromArray(1,one,x,y,z,offset);
� FillFromArray(4,four,x,y,z,offset);
���������� ��� ����� 6
*/
void FillDots1(int number,float x,float y,float z,float offset){
	if (number==3) {
		FillFromArray(3,three,x,y,z,offset);
	}
	else {
		if ((number & 1)>0){
			FillFromArray(1,one,x,y,z,offset);
		}
		if ((number & 2)>0){
			FillFromArray(2,two,x,y,z,offset);
		}
		if ((number & 4)>0){
			FillFromArray(4,four,x,y,z,offset);
		}

	}

}

//����� ���������

class domino {

public:
int playerN;  //����� ������ 0-����, 1-��������� ,2 �����.
int Dots[2];	//������ �������� ������������ ��������� 0..6
float x;
float y;		// ���������� x,y,z ������ ���������
float z;
float height;	// ������ ���������
float width;	// ������ ���������
int baseangle;	// ������� ���� �������� 
int angle;	// ���� �������� 

/*����������� 
pN-����� ������, l - ����� � ����� �����, r - ����� � ������ �����,
ax, ay, az - �������������� ���������� ������ ���������*/
domino(int pN, int l, int r,float ax,float ay,float az){
	Dots[0]=l;
	Dots[1]=r;
	playerN=pN;
	height=0.3;
	width=1.0;
	x=ax;
	y=ay;
	z=az;
	baseangle=0;
/*�� ��� �������� ����� �������� � ������ �� ��������� � ������ ����� 
������ ��������:
	angle=90.0;
*/
	if (playerN==0 && l!=r) {
		angle=0.0;
	} 
	else {
	angle=90.0;
	}
}
//��������� ��������� ���������
void SetPosXYZ(float ax,float ay,float az){
	x=ax;
	y=ay;
	z=az;
}

//�������� �� ��, ��� - ����� � ������ �������� ���������
bool isDuplet() {
	return Dots[0]==Dots[1];
}

/*�������� �� ��, ��� - ����� ��� ������ �������� �������� �����,
��������� � ��������� dot
*/
bool DotsIn(int dot){
	return ((Dots[0]==dot) || (Dots[1]==dot));
}

//��������� ���������
void draw(bool draw1_on) {
	glPushMatrix();
	glTranslatef(x,y,z);//����������� �������� ������ � �������� ����������
	glRotatef((baseangle+angle) % 360,0.0f,0.0f,1.0f); //������� �� �������� ����
	//���������� �������� ������
	float a=width/20; //������ ����������� ������
	float c=height/10;//������ ����������� ������
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat4_amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat4_dif);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat4_spec);
	glMaterialf(GL_FRONT,GL_SHININESS,mat4_shininess);
	//���� ����� ��������
	glPushMatrix();
	glTranslatef(-width/2,0.0f,0.0f);
	piped2(width-a/2,width,height);
	glPopMatrix();
	//����������� ������
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-c/2);
	piped2(a,width,height-c);
	glPopMatrix();
	//���� ������ ��������
	glPushMatrix();
	glTranslatef(width/2+a/4,0.0f,0.0f);
	piped2(width-a/2,width,height);
	glPopMatrix();
	if (playerN !=1 || draw1_on) {
   //������� ������� ����
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat2_amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat2_dif);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat2_spec);
	glMaterialf(GL_FRONT,GL_SHININESS,mat2_shininess);
	a=width/3;
	c=height/2;
	//���� ��������
	FillDots1(Dots[0],-width/2,0.0f,c,a);
	//������ ��������
	FillDots1(Dots[1],width/2,0.0f,c,a);
	}
	glPopMatrix();

}

};


float box_W_2=10.5;
float box_H_2=3.5;

class dominoBox {
domino *items[28];  //������ ������
domino *founditems[28]; //������ ��������� ��� ������

public:
int count; //���������� ��������� ������
int foundcount; //���������� ��������� ���������
domino *leftitem; //������ � ������ ����
domino *rightitem; //������ � ������� ����
domino *selected; //��������� ������
double selXYZ[3]; //����������� ��������� ���������� �� ������ ���������
int cur_player; //������� �������� �����
bool gameover; //������� ����� ����

//�����������
dominoBox(){
	count=0;
	foundcount=0;
	leftitem=NULL;
	rightitem=NULL;
	selected=NULL;
}

//����������
~dominoBox(){
	free();
}

void newGame() { // ���������� � ����� ����
	free();
	selected=NULL;
	gameover=false;
	srand(time(NULL)); //������������� ���������� ��������� �����
	// ������� � �������
	int n=0;
	float x,y;
//��������� ���������� x ��� ������� � ������� ������ - ������� ����� �������
	float x1=-11;
	float x2=-11;
	for (int i=0;i<7;i++) {
		for (int j=i;j<7;j++) {
			//���������� ������ �� ����������� ����������
			int k=rand() % 2;
			if (k==0) {if (n<14) n++; else k=1;}
			else {if (count-n>=14) k=0;}
			//� ����������� �� ������ ���������� ���������� �� ����
			if (k==0) {
				y=8.0f; //���������� Y ��������� ������1(����������)
				x1+=1.5; //��������� �������� �� x ��� ��������� ��������� 1 ������(����������)
				x=x1;
			}
			else { 
				y=-8.0f;  //���������� Y ��������� ������2
				x2+=1.5; //��������� �������� �� x ��� ��������� ��������� 2 ������
				x=x2;
			}
			//������� ���������� � ������������ ���� �����������
			items[count]=new domino(k+1,i,j,x,y,0.0);
			count++;
		}
	}

	// �����/����� �� ���� � ��������� ������������� ����������
	if (foundDotsAND(0,0)) {
		founditems[0]->SetPosXYZ(0,0,0);
		cur_player=founditems[0]->playerN;
		founditems[0]->playerN=0;
		leftitem=founditems[0];
		rightitem=founditems[0];
//���� �������� ������ �����, �� ������(���������) ����� �������������
		if (cur_player==2) {
			goAuto(1);
		}
	}
	
}

void loadfromfile(const char* fname){
	int PN,d1,d2,ri,li,b,a;
	float x,y,z,h,w;
	free();
	selected=NULL;
	gameover=false;
	FILE * file = fopen(fname, "r");
	if (file!=NULL) {
  // ���� ���� ������
	 for (int i=0;i<28;i++) {
	fscanf(file, "%d %d %d %f %f %f %f %f %d %d",&PN,&d1,&d2,&x,&y,&z,&h,&w,&b,&a);
	items[i]=new domino(PN,d1,d2,x,y,z);
	items[i]->height=h;
	items[i]->width=w;
	items[i]->baseangle=b;
	items[i]->angle=a;
	 }
    count=28;
	fscanf(file, "%d %d %d",&cur_player,&li,&ri);
	leftitem=items[li];
	rightitem=items[ri];
 }
  fclose(file);
 } 

void savetofile(const char* fname){
FILE * file = fopen(fname, "w");
if (file!=NULL) {
  // ���� ���� ������
int ri,li;
 for (int i=0;i<28;i++) {
  fprintf(file, "%d %d %d %f %f %f %f %f %d %d\n", items[i]->playerN,items[i]->Dots[0],items[i]->Dots[1],items[i]->x,items[i]->y,items[i]->z,items[i]->height,items[i]->width,items[i]->baseangle,items[i]->angle);
  if (items[i]==rightitem) ri=i;
  if (items[i]==leftitem) li=i;
 }
  fprintf(file, "%d %d %d\n", cur_player,li,ri);
  fclose(file);
 } 
}
/* ��������� ���������
��������� � ����� ��� ��������� �� ������� ��������� ��������� ax, ay
� ���������� ����, ������� ��� ��������. ����� ��� ����������� � 
����� ������. 
� ���� �� ������� ����� ���������,
����������� �� ����������� z, ������ 1. �� ���� �����������
*/
bool select(int aplayerN,float ax,float ay){
	float w=items[0]->width;
	for (int i=0;i<28;i++) {
		if (items[i]->playerN==aplayerN) {
			if ((abs(items[i]->x-ax)<w/2) && (abs(items[i]->y-ay)<w)) {
				selected=items[i];
				selXYZ[0]=selected->x;
				selXYZ[1]=selected->y;
				selXYZ[2]=selected->z;
				selected->z=1;
				return true;

			}
		}
	}
	unselect();
	return false;
}

//���� ���� ���������� ���������, ������ �� ��� ���, ������������
void unselect() {
	if (selected !=NULL) {
		selected->z=0;
		selected=NULL;
	}
}

//��������� �����, ����������� �� ������� ���� ������
int getrightdot(){
	if (rightitem->angle<=90) return rightitem->Dots[1]; else return rightitem->Dots[0];
}

//��������� �����, ����������� �� ������� ���� �����
int getleftdot(){
	if (leftitem->angle<=90) return leftitem->Dots[0]; else return leftitem->Dots[1];
}

// ������ ��� ��������� ���������� �������� select
bool goselected(){
	if (gameover) return false;
domino **d0;
int dot;
int angle=0;
int baseangle;
float offsetX,offsetY;
float aoffsetX[9]={ 1.5, 0.5,-1.5, 1.5, 0.0,-2.0, 2.0, 0.5,-1.5};
float aoffsetY[9]={ 0.0, 1.5, 0.5, 0.0, 2.0, 0.0, 0.0, 1.5, 0.5};
int     aangle[9]={ 90 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 };
bool ret=false;
int k=0;
//���� ���������� ��������� ����������
if (selected !=NULL) {
/*����������- � ����� ��� ������ ����� ��������� �������� ���������
����� ��������� ����������
d0, dot � �������� ����� ������ ��������
*/
	angle=0;
	if (selected->x > 0) {
		d0=&rightitem;
		dot=getrightdot();
	} else {
		d0=&leftitem;
		dot=getleftdot();
	}
	
/*�������� �� ��, ��� ��������� ������ �������� ������ �����, � �����
��� ��������� ������ �� ���� ������� ������
*/
	if (abs(selected->y)<7 && selected->DotsIn(dot)) {
		baseangle=(*d0)->baseangle;
		int k2=0;
/*�������� ��� ���� ��� �������: 
1. ���������� ������- ����� � ���������� ������(�� ������) � ��������� - ����������(������)
2. ���������� ������ -������ � ���������� -������
3. ��� �� �������.
� ��� ������� �� �������� - ��� �������� 
1. ������ ��� (baseangle=0)
2. ������������ ��� (baseangle=90)
3. �������� ��� (baseangle=180)
�������� 9 ���������.
��� ������� �������� ��������� ������� aoffsetX[9],aoffsetY[9],aangle[9] ��.����
�.�. �������� �� X , �������� �� Y, � ���� �������� ������������ baseangle

����� ��������� ����� � ��� �������, � ������� ��������������� �������� �� ��������
�� � ����� ������ �������������, ������� ��������� �� ����� �������.
*/

		if (selected->isDuplet()) {
			k=0;
		} else {
			if ((*d0)->isDuplet()) {k+=3;}
			else {k+=6;}
		}
		switch (baseangle) {
		case 0: if (abs((*d0)->x)>=box_W_2) {k2=1;} else {
					offsetX=aoffsetX[k];
					offsetY=0;
					angle=aangle[k];
				}

			break;
		case 90:if (abs((*d0)->y)>=box_H_2) {k2=2;} else {
					offsetY=aoffsetX[k];
					offsetX=0;
					angle=aangle[k];
					if ((*d0)->isDuplet() && ((*d0)->angle==0 || (*d0)->angle==180)) {
						offsetY+=0.5;
					}
				}
			break;
		case 180:offsetX=-aoffsetX[k];
				offsetY=0;
				angle=aangle[k];
				if ((*d0)->isDuplet() && ((*d0)->angle==0 || (*d0)->angle==180)) {
					offsetX-=0.5;
				}
				break;
		}
		if (k2>0) {
			baseangle=k2*90;
			offsetX=aoffsetX[k+k2];offsetY=aoffsetY[k+k2];angle=aangle[k+k2];
		}
		if (selected->Dots[1]==dot) {
			angle+=180; 
		}
		if (selected->x<0) {offsetX=-offsetX;offsetY=-offsetY;angle+=180;}
		angle=angle % 360; 
		cur_player=selected->playerN;
//����������� ���������� ������ ��� ����� ������� � ���� ��������
		selected->playerN=0;
//		if (selected->x<0) offsetX=-offsetX;
		selected->x=(*d0)->x+offsetX;
		selected->y=(*d0)->y+offsetY;
		selected->baseangle=baseangle;
		selected->angle=angle;
		*d0=selected;/*� ��� �� ���� ������� ����� ������� ���������
������������. �� ���������� ������� ������� ��������� ������ ��� ����,
����� ��������� *d0=selected; ��� ������, ��� ���� � �� �������������
������ � ������ �����, �� �� ��������� rightitem=selected, � ����
� �����, �� leftitem=selected
*/
		ret=true;
//��������� ���� ����� ������ �� �����
PlaySound(TEXT("sound\\domino1.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
/*���� ������ �� ��������, ���������� ��������� �� ������ ����� � 
����������� ������ ����
*/
		selected->x=selXYZ[0];
		selected->y=selXYZ[1];
PlaySound(TEXT("sound\\domino2.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	unselect();
}
isGameOver();
return ret;
}

// ������ ��� ��������� ������ 1(����������)
bool goAuto(int aplayerN) {
	int ldot=getleftdot();
	int rdot=getrightdot();
	bool ret=false;
/*����� �� ���� ��� ��������� ������ aplayerN , � ������� ������������
�����, ����������� � ����� ��� ������ ����� �������� ����
*/
	if (foundDotsOR(aplayerN,ldot,rdot)){
		srand(time(NULL)); //������������� ���������� ��������� �����
//	foundcount �������� ����� ��������� ���������
/* ������ � ������� founditems ������� ��������� ������� �� ����������
��������� ���������.
����� �������� ��� ���������
*/
		int k=rand() % foundcount;
		select(aplayerN,founditems[k]->x,founditems[k]->y);
/*����������- � ����� ��� ������ ����� ���� ���� ��������� � � �����������
�� ����� ��������� ���������� X �� ����. ���������� Y ����������� ������
0 (����� ����).
*/
		if (selected->DotsIn(ldot)) {
			selected->x=leftitem->x-2;
		} else {
			selected->x=rightitem->x+2;
		}
		selected->y=0;
		ret=goselected();
	}
	return ret;
}


bool foundDotsAND(int dots1,int dots2){
	foundcount=0;
	for (int i=0;i<count;i++) {
		if ((items[i]->Dots[0]==dots1 || items[i]->Dots[0]==dots2) && (items[i]->Dots[1]==dots1 || items[i]->Dots[1]==dots2)) {
			founditems[foundcount]=items[i];
			foundcount++;
			break;
		}
	}
	return foundcount>0;
}
bool foundDotsOR(int aplayerN,int dots1,int dots2){
	foundcount=0;
	for (int i=0;i<count;i++) {
		if ((items[i]->playerN==aplayerN) && (items[i]->DotsIn(dots1) || items[i]->DotsIn(dots2))) {
			founditems[foundcount]=items[i];
			foundcount++;
		}
	}
	return foundcount>0;
}

int getPlayerSaldo(int playerN) { //������� ����� �� ������
	int sum=0;
	for (int i=0;i<count;i++) {
		if (items[i]->playerN==playerN) {
			sum=sum+items[i]->Dots[0]+items[i]->Dots[1];
		}
	} 
	return sum;
}

bool isGameOver() {
	for (int i=1;i<=2;i++){
		if (getPlayerSaldo(i)==0) {gameover=true;return true;}
      if (foundDotsOR(i,getleftdot(),getrightdot())) {
		  gameover=false;
		  return false;
	  }
	}
	gameover=true;
	return true;
}

int who_winner(){
int s1=getPlayerSaldo(1);
int s2=getPlayerSaldo(2);
if (s1==s2)	cur_player=0;
else 
if (s1<s2) cur_player=1; 
else cur_player=2;
return cur_player;
}

void draw() { // ����� �� �����
   for (int i=0;i<count;i++) {
   items[i]->draw(gameover);
   }
   if (gameover) {
	 who_winner();
	    glColor3f (1.0F, 0.0F, 0.0F); 
		glRasterPos3f (0.0F, 2.0F,2.5F);
		switch (cur_player) {
		case 0:	draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24 , "Draw"); 
//PlaySound(TEXT("sound\\�����������_�������.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 1:	draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24 ,"You loose("); 
//PlaySound(TEXT("sound\\fail.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 2:	draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24 ,"YOU WIN!)"); 
//PlaySound(TEXT("sound\\Cheer.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;

		}
  }

}

void free(){ // �������� ������ ������
   for (int i=0;i<count;i++) {
   delete items[i];
   }
   count=0;
}

};
//-------------------------------------------------------------------------------------------------

dominoBox *dmnBox;


void on_paint()
{
  angle += 0.03f; // ��������� ���� ��������

  glViewport(0, 0, width, height); // ���������� ������� ��������� ����, ��� ���� �������� ��� ����

  // ������������ ��������� �������� � ������� �����
  GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ���� �������� ��������� 
  GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���� ��������� ��������� 
  GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// ���� ������������ �����������
  GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };// ��������� ������� �����

  // ������������ ��������� ������� ����� 
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glClearColor(0, 0.25, 0.25, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// ������� ������ OpenGL


  glMatrixMode(GL_PROJECTION);// �������� ����� ������ � �������� ��������
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width/height, 1, 100);// ������ ������� ����� ��������
                                                    // � ����������� ������ ���������, 
                                                    // 60 - ��� �������� � �������� �� �� �,
                                                    // (GLfloat)width/height - ��� �������� ������ �� x,
                                                    // 1 � 100 - ������� �� �����������
                                                    // �� ������ �������� �� ������.
  
  glMatrixMode(GL_MODELVIEW);// �������� ����� ������ � ������� ��������
  glLoadIdentity();

  glTranslatef(offset_x,0, distZ);  // ������ � ������� ��������� ��������� �� distZ, 
  glRotatef(angleX, 0.0f, 1.0f, 0.0f);  // ���� ����������� �� �� Oy
  glRotatef(angleY, 1.0f, 0.0f, 0.0f);  // ���� ����������� �� �� Ox

  glEnable(GL_DEPTH_TEST);  // �������� ����� �������
  
  glEnable(GL_LIGHTING);   // �������� ����� ��� ��������� ���������
  glEnable(GL_LIGHT0);     // ������ ������� ����� � 0 (�� ���� ���� �� 8), ����� �� ������ � "����"

  dmnBox->draw();


// ������ ����

   glMaterialfv(GL_FRONT,GL_AMBIENT,mat5_amb);
   glMaterialfv(GL_FRONT,GL_DIFFUSE,mat5_dif);
   glMaterialfv(GL_FRONT,GL_SPECULAR,mat5_spec);
   glMaterialf(GL_FRONT,GL_SHININESS,mat5_shininess);

   glPushMatrix();
   glTranslatef(0.0f,0.0f,-0.7);
   piped2(64, 20, 1);
   glPopMatrix();

  
  // �������� ���, �� ��������
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glFlush();
  glutSwapBuffers(); // ����������� ������
}

void on_keyboard(unsigned char key, int x, int y) //������� ��䳿 �� ���������
{
//  switch (key) {
//    case 27 : exit(0); break;                     // escape - �����
//}
}
//-------��������� ����������� ������-------------------------------
void on_presskey(int key, int xx, int yy) {
	int mod = glutGetModifiers();
	switch (key) {
/*��� ������� ������� CTRL ������� �� ������� ����� ������ ������� 
������� �� X �� 5, ��� ������� �� 0.5
*/
	case GLUT_KEY_LEFT :
		if (mod== GLUT_ACTIVE_CTRL) offset_x=offset_x+5.0f; else 	offset_x=offset_x+0.5f;
		break;
	case GLUT_KEY_RIGHT :
		if (mod== GLUT_ACTIVE_CTRL) offset_x=offset_x-5.0f; else offset_x=offset_x-0.5f;break;
//	case GLUT_KEY_UP :
//	case GLUT_KEY_DOWN :
}
}

/*������� �������������� ��������� ��������� ���� 
� ���������� ���������� �����
*/
void mouseXY2GLXY() {
	GLdouble Xb,Yb,Zb;
	Xb = mouseX;
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );
	Yb = (float)viewport[3] - (float)mouseY;
	gluUnProject( Xb, Yb, 0.95, modelview, projection, viewport, &GLX, &GLY, &GLZ );
// 0.95 ��������� ���������������� ��� distZ=-17
}

//������� ��������� �������� ������� ����
void on_motion(int x, int y)
{
  switch (button) {
/*���� ������ ����� ������ ���� � ���� ���������� ���������, �� ��� 
��������� �������� ����� �� ����� 
*/
	case 0:
	  if (dmnBox->selected !=NULL) {
		mouseX = x;
		mouseY = y;
		mouseXY2GLXY();
		dmnBox->selected->x=GLX;
		dmnBox->selected->y=GLY;
	  }
	  else {
/*���� ������ ����� ������ ���� , �� �� ���� ���������, �� ��������, ��
���������� �������� �����
*/
		  angleX += x - mouseX;
		  angleY += y - mouseY;
		  mouseX = x;
		  mouseY = y;
	  }
      break;
//� ��� ����� �� ��������, ������� ����� ������
//������
  	case 2:
      distZ += y - mouseY;
      mouseY = y;
      break;
//� �� ��� ���

  }
}

//������� ��������� ������� ������ ����
void on_mouse(int _button, int state, int x, int y)
{
	if (state == 1) {    // 0 - ������ �� ������, 1 - ��������� ������
	if ((button>=0) && (dmnBox->selected !=NULL)) {
		if (dmnBox->goselected()) dmnBox->goAuto(1);
	}
	button = -1;      // ������� ������ �� ������
    return;
  }
  switch (button = _button) {
    case 0: 
//���� ������ ����� ������, �� �������� ���������, ���� ������� ������� 
	mouseX = x; mouseY = y; 
	mouseXY2GLXY();
	dmnBox->select(2,GLX,GLY);
		break;
    case 2:  break; //��� ������ ���� ����� ������
  }
  
}

//������� , ������� ���������� ��� ��������� �������� ���� 
void on_size(int w, int h)
{
  width = w;
  height = h;
  if (height == 0) height = 1;
}

void on_timer(int value)
{

if (button==0) {
	int mod = glutGetModifiers();
	if (mouseX<20) offset_x+=1.0f; 
	else if (mouseX>width-20) offset_x-=1.0f;
}
  on_paint();                     // �������������� �����
  glutTimerFunc(33, on_timer, 0); // ����� 33�� ������� ������� �������
}


//���������� ������������ ����
void option(GLint selop)
{
  switch (selop)
  {
  case 1:dmnBox->newGame();
	  angleX = 0; angleY = 0;
	  offset_x=0;
                break;
  case 2: dmnBox->goAuto(1); 
	  break;
  case 3: angleX = 0; angleY = 0; 
	  //distZ = -17;
	  break;
  case 4: offset_x=0;break;
  case 5:exit(0); break;
  case 6:dmnBox->loadfromfile("domino.dat"); break;
  case 7:dmnBox->savetofile("domino.dat"); break;
  }
} 

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);                                     // ������������� GLUT
  glutInitWindowSize(1000, 700);                              // ��������� �������� ����
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); // ��������� OpenGL ���������
  glutCreateWindow("������");                        // �������� ����
  dmnBox=new dominoBox(); //�������� ���������� dominoBox
  dmnBox->newGame();		//�������� ����
  glutDisplayFunc(on_paint);                  // ��������� ������� ����������� ����
  glutReshapeFunc(on_size);                   // --//-- ������� ���������� ��� ��������� �������� ����
  glutKeyboardFunc(on_keyboard);              // --//-- ��� ������� ������� �� ����������
  glutSpecialFunc(on_presskey);				// ��������� ���������� ����������� ������(CTRL, ������� ����������� �������)
  glutMotionFunc(on_motion);                  // --//-- ��� ����������� ����
  glutMouseFunc(on_mouse);                    // --//-- ��� ������� �� ������ ����
  glutCreateMenu(option);
  glutAddMenuEntry("����� ����", 1);
  glutAddMenuEntry("��������� ����", 6);
  glutAddMenuEntry("��������� ����", 7);
  glutAddMenuEntry("���������� ���", 2);
  glutAddMenuEntry("����������� ���", 3);
  glutAddMenuEntry("� ����� ", 4); 
glutAddMenuEntry("�����", 5);
glutAttachMenu( GLUT_RIGHT_BUTTON ); 
  glutTimerFunc(33, on_timer, 0);             // ���� 33�� ����������� �� �������
  glutMainLoop();                             // ���� ������� ����������

  delete dmnBox;
}    