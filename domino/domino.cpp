// domino.cpp: определяет точку входа для консольного приложения.
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


static float angle = 0;         // кут повороту фігур
int          width=1, height=1;
int          button = -1;       // кнопка миши (-1 - не нажата, 0 - левая, 2 - правая)
float        angleX, angleY;    // текущий угол поворота сцены 
float        mouseX, mouseY;    // текущие координаты мыши
int			 keyMod; //Клавиши CTRL,ALT,Shift
GLdouble	GLX, GLY, GLZ;		//Координаты мыши в глобальной системе координат
float        distZ = -17;       // расстояние по оси Z до сцены
float        offset_x=0;		//смещение по X
bool         meny = true;

// параметри матеріалу тору 
float mat1_dif[]={0.8f,0.8f,0.0f};   // колір дифузного відбивання матеріалу 
                                     // (природний колір об'єкта)
float mat1_amb[] = {0.2f,0.2f,0.2f}; // розсіяний колір матеріалу (колір матеріалу в тіні)
float mat1_spec[] = {0.6f,0.6f,0.6f};// колір дзеркального відбивання матеріалу 
                                     // (розмір і форму відблисків на його поверхні)
float mat1_shininess=0.5f*128;       // ступінь дзеркального відбивання матеріалу

// параметри матеріалу конуса 
float mat2_dif[] = {0.0f,0.0f,0.8f};
float mat2_amb[] = {0.2f,0.2f,0.2f};
float mat2_spec[] = {0.6f,0.6f,0.6f};
float mat2_shininess = 0.7f*128;

// параметри матеріалу кулі 
float mat3_dif[] = {0.9f,0.2f,0.0f};
float mat3_amb[] = {0.2f,0.2f,0.2f};
float mat3_spec[] = {0.6f,0.6f,0.6f};
float mat3_shininess = 0.9f*128;

// параметри матеріалу куба 
float mat4_dif[] = {1.0f,1.0f,1.0f};
float mat4_amb[] = {0.2f,0.2f,0.2f};
float mat4_spec[] = {0.6f,0.6f,0.6f};
float mat4_shininess = 0.1f*128;

// параметри матеріалу ліній 
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

//Относительные координаты точечного отображения чисел на домино(на половине)
pointxy one[1]= {{0,0}},                        // 1
        two[2]= {{0,-1},{0,1}},					// 2
        three[3]= {{0,0},{-1,1},{1,-1}},		// 3
		four[4]= {{-1,-1},{1,1},{-1,1},{1,-1}}; // 4
//Для 5(1+4) и 6 (2+4) 

//Построение параллелепипеда
void piped2(GLfloat a, GLfloat b, GLfloat c){	
glScalef (a, b, c); // для вытягивания куба в параллелепипед-масштабируем по x,y,z
glutSolidCube(1.0); // Строим куб-в действительности - параллелепипед с длиной стороны 1.0;
}

/*Отрисовка точечного отображения чисел по координатам из массивов one,two,tree,four
len-длина массива,arr-массив координат, x,y,z-координаты центра половинки доминошки.
Относительно этой точки будут откладываться координаты из массива, домноженные 
на маштабный коэффициент offset.*/
void FillFromArray(int len,pointxy arr[],float x,float y,float z,float offset){
for (int i=0;i<len;i++){
   glPushMatrix();
   glTranslatef(x+arr[i].x*offset,y+arr[i].y*offset,z);
   glutSolidSphere (0.1f, 20, 20);
   glPopMatrix();
}
}

/*Отрисовка половины доминошки
Отрисовка числа 5 производится последовательным вызовом функций
FillFromArray(1,one,x,y,z,offset);
и FillFromArray(4,four,x,y,z,offset);
Аналогично для числа 6
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

//класс Доминошка

class domino {

public:
int playerN;  //Номер игрока 0-стол, 1-компьютер ,2 игрок.
int Dots[2];	//массив числовых эквивалентов половинок 0..6
float x;
float y;		// Координаты x,y,z центра доминошки
float z;
float height;	// Высота доминошки
float width;	// Ширина доминошки
int baseangle;	// Базовый угол поворота 
int angle;	// Угол доворота 

/*Конструктор 
pN-номер игрока, l - число в левой части, r - число в правой части,
ax, ay, az - соотвенственно координаты центра доминошки*/
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
/*Ну эту проверку можно выкинуть и ничего не изменится и вместо этого 
просто написать:
	angle=90.0;
*/
	if (playerN==0 && l!=r) {
		angle=0.0;
	} 
	else {
	angle=90.0;
	}
}
//Установка координат доминошки
void SetPosXYZ(float ax,float ay,float az){
	x=ax;
	y=ay;
	z=az;
}

//Проверка на то, что - левая и правая половина одинаковы
bool isDuplet() {
	return Dots[0]==Dots[1];
}

/*Проверка на то, что - левая или правая половина содердат число,
указанное в аргументе dot
*/
bool DotsIn(int dot){
	return ((Dots[0]==dot) || (Dots[1]==dot));
}

//Отрисовка доминошки
void draw(bool draw1_on) {
	glPushMatrix();
	glTranslatef(x,y,z);//Перемешение костяшки домино в заданные координаты
	glRotatef((baseangle+angle) % 360,0.0f,0.0f,1.0f); //Поворот на заданный угол
	//Построение костяшки домино
	float a=width/20; //Ширина центральной выемки
	float c=height/10;//Высота центральной выемки
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat4_amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat4_dif);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat4_spec);
	glMaterialf(GL_FRONT,GL_SHININESS,mat4_shininess);
	//Тело одной половины
	glPushMatrix();
	glTranslatef(-width/2,0.0f,0.0f);
	piped2(width-a/2,width,height);
	glPopMatrix();
	//Центральная выемка
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-c/2);
	piped2(a,width,height-c);
	glPopMatrix();
	//Тело второй половины
	glPushMatrix();
	glTranslatef(width/2+a/4,0.0f,0.0f);
	piped2(width-a/2,width,height);
	glPopMatrix();
	if (playerN !=1 || draw1_on) {
   //Наносим счетные поля
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat2_amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat2_dif);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat2_spec);
	glMaterialf(GL_FRONT,GL_SHININESS,mat2_shininess);
	a=width/3;
	c=height/2;
	//Одна половина
	FillDots1(Dots[0],-width/2,0.0f,c,a);
	//Вторая половина
	FillDots1(Dots[1],width/2,0.0f,c,a);
	}
	glPopMatrix();

}

};


float box_W_2=10.5;
float box_H_2=3.5;

class dominoBox {
domino *items[28];  //Массив домино
domino *founditems[28]; //Массив элементов для поиска

public:
int count; //Количество элементов домино
int foundcount; //Количество найденных элементов
domino *leftitem; //Домино с левого края
domino *rightitem; //Домино с правого края
domino *selected; //Выбранное домино
double selXYZ[3]; //Сохраняются начальные координаты на момент выделения
int cur_player; //Текущий активный игрок
bool gameover; //Признак конца игры

//Котструктор
dominoBox(){
	count=0;
	foundcount=0;
	leftitem=NULL;
	rightitem=NULL;
	selected=NULL;
}

//Деструктор
~dominoBox(){
	free();
}

void newGame() { // Подготовка к новой игре
	free();
	selected=NULL;
	gameover=false;
	srand(time(NULL)); //Инициализация генератора случайных чисел
	// Тасовка и раздача
	int n=0;
	float x,y;
//Начальные координаты x для первого и второго игрока - крайняя левая позиция
	float x1=-11;
	float x2=-11;
	for (int i=0;i<7;i++) {
		for (int j=i;j<7;j++) {
			//Закрепляем игрока за создаваемой доминошкой
			int k=rand() % 2;
			if (k==0) {if (n<14) n++; else k=1;}
			else {if (count-n>=14) k=0;}
			//В зависимости от игрока определяем координаты на поле
			if (k==0) {
				y=8.0f; //Координата Y доминошек игрока1(компьютера)
				x1+=1.5; //Добавляем смещение по x для следующей доминошки 1 игрока(компьютера)
				x=x1;
			}
			else { 
				y=-8.0f;  //Координата Y доминошек игрока2
				x2+=1.5; //Добавляем смещение по x для следующей доминошки 2 игрока
				x=x2;
			}
			//Создаем доминошину с вычисленными выше параметрами
			items[count]=new domino(k+1,i,j,x,y,0.0);
			count++;
		}
	}

	// пусто/пусто на поле и начальная инициализация переменных
	if (foundDotsAND(0,0)) {
		founditems[0]->SetPosXYZ(0,0,0);
		cur_player=founditems[0]->playerN;
		founditems[0]->playerN=0;
		leftitem=founditems[0];
		rightitem=founditems[0];
//Если начинает второй игрок, то первый(компьютер) ходит автоматически
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
  // если файл найден
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
  // если файл найден
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
/* Выделение доминошки
Проверяем в цикле все доминошки на предмет попадания координат ax, ay
в координаты поля, которое она занимает. Также еще проверяется и 
номер игрока. 
И если мы находим такую доминошку,
присваиваем ей кооординату z, равную 1. То есть приподымаем
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

//Если есть выделенная доминошка, делаем ее как все, невыделенной
void unselect() {
	if (selected !=NULL) {
		selected->z=0;
		selected=NULL;
	}
}

//Вычисляем число, находящееся на игровом поле справа
int getrightdot(){
	if (rightitem->angle<=90) return rightitem->Dots[1]; else return rightitem->Dots[0];
}

//Вычисляем число, находящееся на игровом поле слева
int getleftdot(){
	if (leftitem->angle<=90) return leftitem->Dots[0]; else return leftitem->Dots[1];
}

// делает ход доминошка выделенная командой select
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
//Если выделенная доминошка существует
if (selected !=NULL) {
/*Определяем- в левой или правой части находится выденная доминошка
затем заполняем переменные
d0, dot с которыми будем дальше работать
*/
	angle=0;
	if (selected->x > 0) {
		d0=&rightitem;
		dot=getrightdot();
	} else {
		d0=&leftitem;
		dot=getleftdot();
	}
	
/*Проверка на то, что выбранное домино содержит нужное число, а также
оно выдвинуто хотябы на одну позицию вперед
*/
	if (abs(selected->y)<7 && selected->DotsIn(dot)) {
		baseangle=(*d0)->baseangle;
		int k2=0;
/*Выделяем для себя три момента: 
1. предыдущее домино- числа в половинках разные(не дуплет) в выделеном - одинаковые(дуплет)
2. предыдущее домино -дуплет в выделенном -дуплет
3. оба не дуплеты.
И для каждого из моментов - три варианта 
1. Прямой ход (baseangle=0)
2. Вертикальный ход (baseangle=90)
3. Обратный ход (baseangle=180)
Получаем 9 вариантов.
Для каждого зараннее заполняем массивы aoffsetX[9],aoffsetY[9],aangle[9] см.выше
т.е. смещение по X , смещение по Y, и угол доворота относительно baseangle

Далее вычисляем какой у нас вариант, и заносим соответствующие значения из массивов
Ну а затем вносим корректировки, которые выявились на этапе отладки.
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
//Присваиваем выбранному домино его новую позицию и угол поворота
		selected->playerN=0;
//		if (selected->x<0) offsetX=-offsetX;
		selected->x=(*d0)->x+offsetX;
		selected->y=(*d0)->y+offsetY;
		selected->baseangle=baseangle;
		selected->angle=angle;
		*d0=selected;/*А вот на этом моменте нужно немного подробнее
остановиться. Мы специально выбрали двойной указатель именно для того,
чтобы проделать *d0=selected; Это значит, что если у мы устанавливали
домино в правую часть, то мы присвоили rightitem=selected, а если
с левой, то leftitem=selected
*/
		ret=true;
//Проиграть звук удара домино по столу
PlaySound(TEXT("sound\\domino1.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
/*Если домино не подходит, возвращаем доминошку на старое место и 
проигрываем другой звук
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

// делает ход доминошка игрока 1(компьютера)
bool goAuto(int aplayerN) {
	int ldot=getleftdot();
	int rdot=getrightdot();
	bool ret=false;
/*Здесь мы ищем все доминошки игрока aplayerN , у которых присутствуют
числа, находящиеся в левой или правой части игрового поля
*/
	if (foundDotsOR(aplayerN,ldot,rdot)){
		srand(time(NULL)); //Инициализация генератора случайных чисел
//	foundcount содержит число найденных доминошек
/* Индекс в массиве founditems находим случайным образом из количества
найденных доминошек.
Далее выделяем эту доминошку
*/
		int k=rand() % foundcount;
		select(aplayerN,founditems[k]->x,founditems[k]->y);
/*Определяем- в левую или правую часть идет наша доминошка и в зависимости
от этого вычисляем координату X на поле. Координату Y присваиваем равной
0 (центр поля).
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

int getPlayerSaldo(int playerN) { //Подсчет очков по игроку
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

void draw() { // Вывод на экран
   for (int i=0;i<count;i++) {
   items[i]->draw(gameover);
   }
   if (gameover) {
	 who_winner();
	    glColor3f (1.0F, 0.0F, 0.0F); 
		glRasterPos3f (0.0F, 2.0F,2.5F);
		switch (cur_player) {
		case 0:	draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24 , "Draw"); 
//PlaySound(TEXT("sound\\Нейтральная_мелодия.wav"), NULL, SND_FILENAME | SND_ASYNC);
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

void free(){ // Очистить массив домино
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
  angle += 0.03f; // збільшення кута повороту

  glViewport(0, 0, width, height); // Установити область перегляду таку, щоб вона вміщувала все вікно

  // ініціалізували параметри матеріалів і джерела світла
  GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // колір фонового освітлення 
  GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // колір дифузного освітлення 
  GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// колір дзеркального відображення
  GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };// положення джерела світла

  // встановлюємо параметри джерела світла 
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glClearColor(0, 0.25, 0.25, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// очистка буферів OpenGL


  glMatrixMode(GL_PROJECTION);// включаємо режим роботи з матрицею проекцій
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width/height, 1, 100);// задаємо усічений конус видимості
                                                    // в лівосторонній системі координат, 
                                                    // 60 - кут видимості в градусах по осі у,
                                                    // (GLfloat)width/height - кут видимості уздовж осі x,
                                                    // 1 и 100 - відстань від спостерігача
                                                    // до площин відсікання по глибині.
  
  glMatrixMode(GL_MODELVIEW);// включаємо режим роботи з видовою матрицею
  glLoadIdentity();

  glTranslatef(offset_x,0, distZ);  // камера з початку координат зсувається на distZ, 
  glRotatef(angleX, 0.0f, 1.0f, 0.0f);  // потім повертається по осі Oy
  glRotatef(angleY, 1.0f, 0.0f, 0.0f);  // потім повертається по осі Ox

  glEnable(GL_DEPTH_TEST);  // включаємо буфер глибини
  
  glEnable(GL_LIGHTING);   // включаємо режим для установки освітлення
  glEnable(GL_LIGHT0);     // додаємо джерело світла № 0 (їх може бути до 8), зараз він світить з "очей"

  dmnBox->draw();


// рисуем стол

   glMaterialfv(GL_FRONT,GL_AMBIENT,mat5_amb);
   glMaterialfv(GL_FRONT,GL_DIFFUSE,mat5_dif);
   glMaterialfv(GL_FRONT,GL_SPECULAR,mat5_spec);
   glMaterialf(GL_FRONT,GL_SHININESS,mat5_shininess);

   glPushMatrix();
   glTranslatef(0.0f,0.0f,-0.7);
   piped2(64, 20, 1);
   glPopMatrix();

  
  // вимкнемо все, що включили
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glFlush();
  glutSwapBuffers(); // перемикання буферів
}

void on_keyboard(unsigned char key, int x, int y) //обробка події від клавіатури
{
//  switch (key) {
//    case 27 : exit(0); break;                     // escape - вихід
//}
}
//-------Обработка специальных клавиш-------------------------------
void on_presskey(int key, int xx, int yy) {
	int mod = glutGetModifiers();
	switch (key) {
/*При нажатой клавише CTRL нажатие на стрелки влево вправо смещают 
позицию по X на 5, при отжатой на 0.5
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

/*функция преобразования локальных координат мыши 
в глобальные координаты сцены
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
// 0.95 посчитано экспериментально для distZ=-17
}

//функция обработки движения курсора мыши
void on_motion(int x, int y)
{
  switch (button) {
/*Если нажата левая кнопка мышы и есть выделенная доминошка, то эта 
доминошка движется вслед за мышью 
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
/*Если нажата левая кнопка мышы , но ни одна доминошка, не выделена, то
происходит вращение сцены
*/
		  angleX += x - mouseX;
		  angleY += y - mouseY;
		  mouseX = x;
		  mouseY = y;
	  }
      break;
//А это какой то рудимент, который нужно убрать
//Отсюда
  	case 2:
      distZ += y - mouseY;
      mouseY = y;
      break;
//и до сих пор

  }
}

//функция обработки нажатий клавиш мыши
void on_mouse(int _button, int state, int x, int y)
{
	if (state == 1) {    // 0 - нажали на кнопку, 1 - отпустили кнопку
	if ((button>=0) && (dmnBox->selected !=NULL)) {
		if (dmnBox->goselected()) dmnBox->goAuto(1);
	}
	button = -1;      // никакая кнопка не нажата
    return;
  }
  switch (button = _button) {
    case 0: 
//Если нажата левая кнопка, то выделяем доминошку, если таковая имеется 
	mouseX = x; mouseY = y; 
	mouseXY2GLXY();
	dmnBox->select(2,GLX,GLY);
		break;
    case 2:  break; //Эту строку тоже можно убрать
  }
  
}

//Событие , которое происходит при изменении размеров окна 
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
  on_paint();                     // перерисовываем экран
  glutTimerFunc(33, on_timer, 0); // через 33мс вызывем функцию таймера
}


//Обработчик контекстного меню
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
  glutInit(&argc, argv);                                     // инициализация GLUT
  glutInitWindowSize(1000, 700);                              // установка размеров окна
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); // настройка OpenGL контекста
  glutCreateWindow("Домино");                        // создание окна
  dmnBox=new dominoBox(); //Создание экземпляра dominoBox
  dmnBox->newGame();		//Начинаем игру
  glutDisplayFunc(on_paint);                  // указываем функцию перерисовки окна
  glutReshapeFunc(on_size);                   // --//-- функция вызывается при изменении размеров окна
  glutKeyboardFunc(on_keyboard);              // --//-- при нажатии клавиши на клавиатуре
  glutSpecialFunc(on_presskey);				// указываем обработчик специальных клавиш(CTRL, клавиши перемещения курсора)
  glutMotionFunc(on_motion);                  // --//-- при перемещении мыши
  glutMouseFunc(on_mouse);                    // --//-- при нажатии на кнопку мыши
  glutCreateMenu(option);
  glutAddMenuEntry("Новая игра", 1);
  glutAddMenuEntry("Загрузить игру", 6);
  glutAddMenuEntry("Сохранить игру", 7);
  glutAddMenuEntry("Пропустить ход", 2);
  glutAddMenuEntry("Фронтальный вид", 3);
  glutAddMenuEntry("В центр ", 4); 
glutAddMenuEntry("Выход", 5);
glutAttachMenu( GLUT_RIGHT_BUTTON ); 
  glutTimerFunc(33, on_timer, 0);             // кожні 33мс викликається ця функція
  glutMainLoop();                             // цикл обробки повідомлень

  delete dmnBox;
}    