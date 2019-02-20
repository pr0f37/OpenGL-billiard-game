#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <stdio.h>          

#define LCOORD		-RCOORD			// LEFT COORDINATE
#define RCOORD		3.0f			// RIGHT COORDINATE
#define DCOORD		0.0f			// DOWN	COORDINATE
#define UCOORD		0.9f			// UP COORDINATE
#define LICOORD		-RICOORD		// LEFT-INTERNAL COORDINATE
#define RICOORD		2.75f			// RIGHT-INTERNAL COORDINATE
#define DICOORD		0.2f			// DOWN-INTERNAL COORDINATE
#define FGCOORD		6.0f			// FOREGORUND COORDINATE
#define FGICOORD	5.75f			// FOREGROUND-INTERNAL COORDINATE
#define BGCOORD		-FGCOORD		// BACKGROUND COORDINATE
#define BGICOORD	-FGICOORD		// BACKGROUND-INTERNAL

HGLRC hRC=NULL;         // kontekst renderownia
HDC hDC=NULL;         // kontekst graficzny
HWND hWnd=NULL;         // uchwyt okna
HINSTANCE hInstance;         // instancja aplikacji

bool keys[256];         // Stan klawiszy
bool active=TRUE;         // flaga zminimalizowania, domy�lnie na true
bool fullscreen=TRUE;         // tryb pe�noekranowy. domy�lnie na true

bool bp;         // flaga do B 
bool blend;

GLfloat xrot;         // Obr�t na osi X
GLfloat yrot;         // Obr�t na osi Y
GLfloat xspeed;         // Pr�dko�� obracania si� X
GLfloat yspeed;         // Pr�dko�� obracania si� Y
GLfloat z=-15.0f;         // Pozycja Z

GLfloat ballXPos[] = {2.5f,2.5f,2.0f,3.0f}; // pozycje X sfer
GLfloat ballZPos[] = {10.0f,3.0f,2.2f,2.2f}; // pozycje Z sfer
GLfloat ballXSpeed[] = {0.0f,0.0f,0.0f,0.0f}; // pr�dko�ci X sfer
GLfloat ballZSpeed[] = {0.0f,0.0f,0.0f,0.0f}; // pr�dko�ci Z sfer


GLfloat LightAmbient1[]={ 1.0f, 1.0f, 1.0f, 1.0f };         // Warto�ci �wiat�a otaczaj�cego  
GLfloat LightAmbient2[]={ 1.0f, 1.0f, 1.0f, 1.0f };         // Warto�ci �wiat�a rozproszonego 
GLfloat LightPosition[]={ 0.0f, 0.0f, 2.0f, 1.0f };         // Pozycja �wiat�a 
GLfloat LightPosition2[]={ 0.0f, 5.0f, -10.0f, 1.0f };         // Pozycja �wiat�a 

GLuint texture[6];         // tekstury
GLuint table;         // miejsce na list� wy�wietlania
GLuint sky;

GLUquadricObj *quadratic;         // Kwadryki 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);         // Deklaracja funkcji WndProc 


AUX_RGBImageRec *LoadBMP(char *Filename)         // �aduje bitmap� z pliku
{
    FILE *File=NULL;         // wska�nik na plik
	if (!Filename)         // Upewnij si� �e podano nazw� pliku
    {
        return NULL;         // Je�eli nie to zwr�� NULL
    } 
	File=fopen(Filename,"r");         // Sprawd� czy plik istnieje 
	if (File)         // Czy plik istnieje
    {
        fclose(File);         // Zamknij plik
        return auxDIBImageLoad(Filename);         // Za�aduj bitmap� i zwr�� wska�nik
    } 
	return NULL;         // Je�eli nie uda�o si� za�adowa� pliku, zwr�� NULL
} 
int LoadGLTextures()         // �aduj bitmap� i przekszta�� j� w tekstur�
{ 
	int Status=FALSE;         // Status tworzenia tekstury 
	AUX_RGBImageRec *TextureImage[1];         // Rekord dla naszego obrazka 
	memset(TextureImage,0,sizeof(void *)*1);         // Wyzeruj wska�nik 
	    // Za�aduj obrazek
    if (TextureImage[0]=LoadBMP("wood3.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[0]);         // Stw�rz obiekt tekstury 
		        // Stw�rz mipmappow� tekstur�
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 
    } 
	if (TextureImage[0]=LoadBMP("floor2.bmp")) // pod�oga
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[1]);         // Stw�rz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 
	}
	if (TextureImage[0]=LoadBMP("wall2.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[2]);         // Stw�rz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("woodwall4.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[3]);         // Stw�rz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("ball3.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[4]);         // Stw�rz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("red.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[5]);         // Stw�rz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0])         // Je�eli tekstura istnieje
    {
        if (TextureImage[0]->data)         // Je�eli obrazek tekstury istnieje
        {
            free(TextureImage[0]->data);         // Zwolnij pami�� obrazka tekstury
        }
        free(TextureImage[0]);         // Zwolnij struktur� obrazka
    }
	return Status;         // Zwr�� status operacji
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
	if (height==0) {       // zapobiegnij dzieleniu przez zero...
		height=1;         // ...ustawiaj�c liczb� 1
	}
	glViewport(0, 0, width, height);         // zresetuj pole widzenia
	glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
	glLoadIdentity();         // zresetuj j�
        // oblicz perspektyw� dla okna
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
	glLoadIdentity();         // zresetuj j�
} 

GLvoid BuildLists()         // Stw�rz listy wy�wietlania
{ 
	table=glGenLists(2);         // przydziel miejsce na dwie listy wy�wietlania 
	glNewList(table,GL_COMPILE);         // nowa lista wy�wietlania. pude�ko, skompilowana 
	glBegin(GL_QUADS);
        // Przednia �ciana
		glNormal3f( 0.0f, 0.0f, 1.0f);         // Normalna wskazuj�ca na widza
        glTexCoord2f(0.0f, 0.0f); glVertex3f(LCOORD, DCOORD, FGCOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);         // Prawy dolny wierzcho�ek tekstury i czworok�ta
        glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
        glTexCoord2f(0.0f, 1.0f); glVertex3f(LCOORD, UCOORD, FGCOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Prawa �ciana
		glNormal3f( 1.0f, 0.0f, 0.0f);         // Normalna wskazuj�ca na widza
        glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);		// Prawy g�rny	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);		// Lewy g�rny
		// Tylna �ciana
		glNormal3f( 0.0f, 0.0f, -1.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);		// Prawy g�rny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);		// Lewy g�rny
		// Lewa �ciana
		glNormal3f( -1.0f, 0.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, DCOORD, FGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);		// Prawy g�rny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);		// Lewy g�rny
		// Sp�d
		glNormal3f( 0.0f, -1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, DCOORD, FGCOORD);         // Lewy dolny 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);         // Prawy dolny 
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		 // Prawy g�rny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Lewy g�rny
		// Przednia g�ra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Prawa g�ra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD,BGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Tylna g�ra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LICOORD, UCOORD, BGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Lewa g�ra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, BGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Przednia wewn�trzna
		glNormal3f( 0.0f, 0.0f, -1.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Prawy lewy wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Prawa wewn�trzna
		glNormal3f( -1.0f, 0.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);         // Prawy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Tylna wewn�trzna
		glNormal3f( 0.0f, 0.0f, 1.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f(LICOORD, DICOORD, BGICOORD);         // Prawy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f(LICOORD, UCOORD, BGICOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Lewa wewn�trzna
		glNormal3f( 1.0f, 0.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f(LICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f(LICOORD, DICOORD,BGICOORD);         // Prawy dolny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f(LICOORD, UCOORD,BGICOORD);         // Prawy g�rny wierzcho�ek tekstury i czworok�ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f(LICOORD, UCOORD, FGICOORD);         // Lewy g�rny wierzcho�ek tekstury i czworok�ta
		// Powierzchnia sto�u
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj�ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f(LICOORD, DICOORD, FGICOORD);         // Lewy dolny 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Prawy dolny 
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);		 // Prawy g�rny
		glTexCoord2f(0.0f, 1.0f); glVertex3f(LICOORD, DICOORD, BGICOORD);		// Lewy g�rny
	glEnd();
	glEndList();         // zako�cz budowanie listy 

	sky = table + 1;
	glNewList(sky,GL_COMPILE);
	glBegin(GL_QUADS);
	// przednia �ciana
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 45.0f, 40.0f);		//Lewy g�rny wierzcho�ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho�ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho�ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 45.0f, 40.0f);			// Prawy g�rny wierzcho�ek
	// prawa �ciana
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 45.0f, 40.0f);		//Lewy g�rny wierzcho�ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho�ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, -40.0f); // Prawy dolny wierzcho�ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 45.0f, -40.0f);			// Prawy g�rny wierzcho�ek
	// tylna �ciana
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 45.0f, -40.0f);		//Lewy g�rny wierzcho�ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, -5.0f, -40.0f); // Lewy dolny wierzcho�ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, -5.0f, -40.0f); // Prawy dolny wierzcho�ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 45.0f, -40.0f);			// Prawy g�rny wierzcho�ek
	// lewa �ciana
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 45.0f, -40.0f);		//Lewy g�rny wierzcho�ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, -40.0f); // Lewy dolny wierzcho�ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho�ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 45.0f, 40.0f);			// Prawy g�rny wierzcho�ek
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	// sp�d
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, -5.0f, -40.0f);		//Lewy g�rny wierzcho�ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho�ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho�ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, -5.0f, -40.0f);			// Prawy g�rny wierzcho�ek
	glEnd();
	glEndList();
	
}

void leg() { // rysowanie nogi sto�u
	GLfloat height = 5.0f;
	glPushMatrix();
		glTranslatef(0.0f, -5.0f, 0.0f);         // Przesu�
		glRotatef(-90.0f,1.0f,0.0f,0.0f);
		gluCylinder(quadratic, 0.1f, 0.1f, 0.4f,32,10);
		glTranslatef(0.0f, 0.0f, 0.4f);         // Przesu�
		gluCylinder(quadratic, 0.1f, 0.13f, 0.1f,32,10);
		glRotatef(90.0f,1.0f,0.0f,0.0f);
		glTranslatef(0.0f, 0.15f, 0.0f);
		gluSphere(quadratic, 0.14f, 32, 32);
		glTranslatef(0.0f, 0.12f, 0.0f);         // Przesu� 
		glRotatef(-90.0f,1.0f,0.0f,0.0f);
		gluCylinder(quadratic, 0.08f, 0.2f, 2.5f,32,32);
		glTranslatef(0.0f, 0.0f, 2.5f);         // Przesu� 
		gluCylinder(quadratic, 0.2f, 0.18f, 0.8f,32,32);		
		glTranslatef(0.0f, 0.0f, 0.8f);         // Przesu� 
		gluCylinder(quadratic, 0.18f, 0.22f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesu� 
		gluCylinder(quadratic, 0.22f, 0.14f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesu�
		gluCylinder(quadratic, 0.14f, 0.22f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesu�
		gluCylinder(quadratic, 0.22f, 0.25f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesu�
		gluCylinder(quadratic, 0.25f, 0.25f, 0.7f,32,10);
	glPopMatrix();
}
GLfloat ballPosition(GLfloat* ball, GLfloat* plane) { // wyliczanie pozycji kuli w stosunku do danej �ciany sto�u
	GLfloat position = 0;
	for (int i = 0; i < 3; i++)
		position += ball[i] * plane[i];
	return position;
}
void hit(GLfloat& xPos, GLfloat& zPos, GLfloat& xSpeed, GLfloat& zSpeed){ // funkcja wyliczaj�ca zderzenie ze �cian�
	
	GLfloat frontPlane[] = {0.0f, 0.0f, 1.0f, 10.77f}; // powiekszone o promie� kuli
	GLfloat rightPlane[] = {1.0f, 0.0f, 0.0f, 4.85f};
	GLfloat backPlane[] = {0.0f, 0.0f, 1.0f, 0.1f};
	GLfloat leftPlane[] = {1.0f, 0.0f, 0.0f, 0.15f};
	GLfloat vectLenght = 0;
	if (ballXSpeed[0] != 0 || ballZSpeed[0] !=0) // �eby nie wyci�ga� pierwiastka z 0
		vectLenght = sqrt(pow(xSpeed,2) + pow(zSpeed,2));
	GLfloat ballVector[] = {xPos,5.59f,zPos,vectLenght};
	GLfloat ballPos = 0;
	ballPos = ballPosition(ballVector, frontPlane);
	if ((ballPos + zSpeed) > frontPlane[3]) {// zderzenie
		zSpeed = -zSpeed;
		zSpeed /= 1.1f;
		zPos += zSpeed;
	}
	ballPos = ballPosition(ballVector, backPlane);
	if ((ballPos + zSpeed) < backPlane[3]) {// zderzenie
		zSpeed = -zSpeed;
		zSpeed /= 1.1f;
		zPos += zSpeed;
	}
	ballPos = ballPosition(ballVector, rightPlane);
	if ((ballPos + xSpeed) > rightPlane[3]) {// zderzenie
		xSpeed = -xSpeed;
		xSpeed /= 1.1f;
		xPos += xSpeed;
	}
	ballPos = ballPosition(ballVector, leftPlane);
	if ((ballPos + xSpeed) < leftPlane[3]) {// zderzenie
		xSpeed = -xSpeed;
		xSpeed /= 1.1f;
		xPos += xSpeed;
	}	
}
void hitBall(GLfloat* xPos, GLfloat* zPos, GLfloat* xSpeed, GLfloat* zSpeed) {
	for (int i = 0; i<3; i++) {
		for (int j = i+1; j<4; j++) {
			GLfloat distance = sqrt(pow(xPos[i] - xPos[j], 2) + pow(zPos[i] - zPos[j], 2));
			if (distance < 0.8f ) { // zderzenie kulek
				GLfloat temp = xSpeed[i];
				xSpeed[i] = xSpeed[j];
				xSpeed[j] = temp;
				temp = zSpeed[i];
				zSpeed[i] = zSpeed[j];
				zSpeed[j] = temp;
				xPos[i] += 2*xSpeed[i]; 
				zPos[i] += 2*zSpeed[i];
				xPos[j] += 2*xSpeed[j];
				zPos[j] += 2*zSpeed[j];
			}
		}
	}
}
void motion(GLfloat& xPos, GLfloat& zPos, GLfloat& xSpeed, GLfloat& zSpeed) {
	xPos += xSpeed;
	if (xSpeed != 0.0f)
		xSpeed /= 1.005f;
	if (abs(xSpeed) < 0.0009f)
		xSpeed = 0.0f;

	zPos += zSpeed;
	if (zSpeed != 0.0f)
		zSpeed /= 1.005f;
	if (abs(zSpeed) < 0.0009f)
		zSpeed = 0.0f;
}
int InitGL(GLvoid)
{ 
	if (!LoadGLTextures())         // Za�aduj tekstury 
    {
        return FALSE;         // Je�eli tekstury nie za�adowano zrw�� FALSE 
    }
	BuildLists();         // stw�rz listy wy�wietlania
    glEnable(GL_TEXTURE_2D);         // W��cz nak�adanie tekstur 
    glShadeModel(GL_SMOOTH);         // W��cz g�adkie cieniowanie
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);         // Czarne t�o
    glClearDepth(1.0f);         // Ustawienie bufora g��bi
    glEnable(GL_DEPTH_TEST);         // W��cz testowanie g��bi
    glDepthFunc(GL_LEQUAL);         // Spos�b testowania g��bi (mniejsze, r�wne)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);         // Dobre obliczenia perspektywy
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightAmbient1);         // Ustaw �wiat�o otaczaj�ce 
	glLightfv(GL_LIGHT2, GL_POSITION,LightPosition2);         // Pozycja �wiat�a 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightAmbient2);         // Ustaw �wiat�o otaczaj�ce 
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);         // Pozycja �wiat�a 
	glEnable(GL_LIGHT1);         // W��cz �wiat�o numer jeden
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);         // W��cz �wiat�o
	glColor4f(1.0f,1.0f,1.0f,0.5f);         // Pe�na jasno��, wsp�czynnik alpha = 50%
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);         // Funkcja mieszaj�ca kolory dla p�przezroczysto�ci oparta o warto�� �r�d�ow� alpha ( NOWE ) 
	quadratic=gluNewQuadric();         // wska�nik na kwadryk�
    gluQuadricNormals(quadratic, GLU_SMOOTH);         // wyg�adzanie kwadryk
    gluQuadricTexture(quadratic, GL_TRUE);         // Tworzy wsp�rz�dne tekstury 
	return TRUE;
}

int DrawGLScene(GLvoid)         // funkcja rysuj�ca
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Wyczy�� ekran
	glLoadIdentity();         // Zresetuj aktualn� macierz
	glTranslatef(0.0f,-1.0f,z);         // Przesu� ekran 
	glRotatef(xrot,0.1f,0.0f,0.0f);         // Obr�� na osi X
	glRotatef(yrot,0.0f,0.1f,0.0f);         // Obr�� na osi Y
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glCallList(sky);
		glBindTexture(GL_TEXTURE_2D, texture[0]);          
		glCallList(table);         // narusuj st�
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTranslatef(LCOORD+0.5f, 0.0f, FGCOORD-0.5f);         // Przesu� w ekran 5 jednostek
		leg();
		glTranslatef(5.0f, 0.0f, 0.0f);         // Przesu� w ekran 5 jednostek
		leg();
		glTranslatef(0.0f, 0.0f, -11.0f);         // Przesu� w ekran 5 jednostek
		leg();
		glTranslatef(-5.0f, 0.0f, 0.0f);         // Przesu� w ekran 5 jednostek
		leg();	
		glBindTexture(GL_TEXTURE_2D, texture[4]); // tekstura dla kuli 1
		hit(ballXPos[0], ballZPos[0], ballXSpeed[0], ballZSpeed[0]);
		hitBall(ballXPos, ballZPos, ballXSpeed, ballZSpeed);
		motion(ballXPos[0], ballZPos[0], ballXSpeed[0], ballZSpeed[0]);
		glPushMatrix();
			glTranslatef(ballXPos[0], 0.59f, ballZPos[0]);
			glRotatef((fabs(ballZPos[0])*250.0f)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			glRotatef((fabs(ballXPos[0])*250.0f)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kul� 1		
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, texture[5]);	// tekstura dla kuli 2
		hit(ballXPos[1], ballZPos[1], ballXSpeed[1], ballZSpeed[1]);
		motion(ballXPos[1], ballZPos[1], ballXSpeed[1], ballZSpeed[1]);
		glPushMatrix();
			glTranslatef(ballXPos[1], 0.59f, ballZPos[1]);
			//glRotatef((ballZPos[1]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[1]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kul� 2
		glPopMatrix();
		hit(ballXPos[2], ballZPos[2], ballXSpeed[2], ballZSpeed[2]);
		motion(ballXPos[2], ballZPos[2], ballXSpeed[2], ballZSpeed[2]);
		glPushMatrix();
			glTranslatef(ballXPos[2], 0.59f, ballZPos[2]);
			//glRotatef((ballZPos[2]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[2]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kul� 3
		glPopMatrix();
		hit(ballXPos[3], ballZPos[3], ballXSpeed[3], ballZSpeed[3]);
		motion(ballXPos[3], ballZPos[3], ballXSpeed[3], ballZSpeed[3]);
		glPushMatrix();
			glTranslatef(ballXPos[3], 0.59f, ballZPos[3]);
			//glRotatef((ballZPos[3]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[3]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kul� 4
		glPopMatrix();
	glPopMatrix();
	
    
	if (xrot < -5.0f || xrot > 185.0f) 
		xspeed = -xspeed;
	xrot+=xspeed;         // Obr�t na osi X
    yrot+=yspeed;         // Obr�t na osi Y
  	return TRUE;         // wszystko jest ok
} 


GLvoid KillGLWindow(GLvoid)         // zamknij okno
{ 
	gluDeleteQuadric(quadratic);         // Usuwamy kwadryk� - zwalniamy zasoby
	if (fullscreen) { 
		ChangeDisplaySettings(NULL,0);         // przywr�c rozdzielczo��
		ShowCursor(TRUE);         // poka� kursor
	} 
	if (hRC) {         // mamy kontekst renderu? 
		if (!wglMakeCurrent(NULL,NULL)) {        // Czy jeste�my w stanie zwolni� go 
			MessageBox(NULL,"Nie mo�na zwolni� hDC lub hRC!","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC)) {        // Czy mo�emy usun�� kontekst renderu?
			MessageBox(NULL,"Nie mo�na usun�� kontekstu renderowania","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}
	if (hDC && !ReleaseDC(hWnd,hDC)) {        // Czy mo�emy zwolni� hDC
		MessageBox(NULL,"Nie mo�na zwolni� kontekstu urz�dzenia (DC)","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;         // Ustawiamy DC na NULL
	} 
	if (hWnd && !DestroyWindow(hWnd)) {        // Czy mo�emy zwolni� uchwyt okna?	
		MessageBox(NULL,"Nie mo�na zwolni� hWnd","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;         // Ustaw hWnd na Null
	} 
	if (!UnregisterClass("OpenGL",hInstance)) {        // Czy mo�emy wyrejstrowa� klas� okna?
		MessageBox(NULL,"Nie mo�na wyrejstrowa� klasy okna","B��D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;         // Ustawiamy instancj� na NULL
	}
}


bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag){
	GLuint PixelFormat;         // B�dzie przechowywa� znaleziony format piksela 
	WNDCLASS wc;         // Struktura klasy okna 
	DWORD dwExStyle;         // Rozszerzony styl okna
	DWORD dwStyle;         // Normalny styl okna
	RECT WindowRect;         // Tu b�d� rozmiary okna
	WindowRect.left=(long)0;         // Pocz�tek szeroko�ci (od lewej) ma 0
	WindowRect.right=(long)width;         // Szeroko�c bierzemy z parametru naszej funkcji
	WindowRect.top=(long)0;         // Wysoko�� te� zaczynamy od 0 (od g�ry)
	WindowRect.bottom=(long)height;         // Ustawiamy wysoko�� z parametru naszej funkcji
	fullscreen=fullscreenflag;         // Ustawiamy globaln� zmienn� 
	hInstance = GetModuleHandle(NULL);         // Pobieramy instancj� dla okna
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;         // Ustawiamy odmalowywanie
	wc.lpfnWndProc = (WNDPROC) WndProc;         // WndProc b�dzie obs�ugiwa� komunikaty
	wc.cbClsExtra = 0;         // nie wa�ne
	wc.cbWndExtra = 0;         // nie wa�ne
	wc.hInstance = hInstance;         // Ustawiamy instancj�
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);         // Ikona domy�lna
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);         // Kursor - strza�ka
	wc.hbrBackground = NULL;         // T�o
	wc.lpszMenuName = NULL;         // Nie chcemy menu
	wc.lpszClassName = "OpenGL";         // Nazwa klasy okna
	if (!RegisterClass(&wc)) {        // Spr�buj zarejstrowa� klas� okna
		MessageBox(NULL,"Nie uda�o si� zarejstrowa� klasy okna","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zako�cz i zwr�� fa�sz.
	} 
	if (fullscreen){         // Czy ma by� pe�ny ekran ?
		DEVMODE dmScreenSettings;         // Tryb karty graficznej
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));         // Wyczy�� pami��
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);         // Ustaw rozmiar tej struktury
		dmScreenSettings.dmPelsWidth = width;         // Wybierz ��dan� szeroko��
		dmScreenSettings.dmPelsHeight = height;         // Wybierz ��dan� wysoko��
		dmScreenSettings.dmBitsPerPel = bits;         // Wybierz g��bie kolor�w
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT; 
	       // Spr�buj ustawi� pe�ny ekran. CDS_FULLSCREEN usuwa pasek start.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) { 
			        // Je�li si� nie uda, przejd� do okna lub zako�cz program
			if(MessageBox(NULL,"Tryb graficzny nie jest obs�ugiwany przez twoj� kart� graf. Czy u�y� zamiast niego okna?","",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
				fullscreen=FALSE;         // Tryb okienkowy. fullscreen na false.
			}
			else {
				// Uwaga o zamykaniu
				MessageBox(NULL,"Program teraz si� zamknie.","B��D",MB_OK|MB_ICONSTOP);
				return FALSE;         // Zako�cz i zwr�� false
			}
		}
	}
	if(fullscreen) {         // Wci�� jeste�my w trybie pe�noekranowym
		dwExStyle=WS_EX_APPWINDOW;         // Rozszerzony styl okna
		dwStyle=WS_POPUP;         // Styl okna
		ShowCursor(FALSE);         // Ukryj kursor myszy
	}
	else { 
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;         // Rozszerzony styl okna
		dwStyle=WS_OVERLAPPEDWINDOW;         // Styl okna
	} 
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); 
	if( !(hWnd=CreateWindowEx( dwExStyle,         // Rozszerzony styl dla okna
		"OpenGL",         // Nazwa klasy
		title,         // Tytu� okna
		WS_CLIPSIBLINGS |         // Wymagane style okna
		WS_CLIPCHILDREN |         // Wymagane style okna
		dwStyle,         // Wybrane style okna
		0, 0,         // Pozycja okna
		WindowRect.right-WindowRect.left,         // Szeroko��
		WindowRect.bottom-WindowRect.top,         // Wysoko��
		NULL,         // Nie u�ywamy okna potomnego
		NULL,         // �adnego menu
		hInstance,         // Instancja
		NULL))) {         // Nie dawaj nic do WM_CREAT
			KillGLWindow();         // Zresetuj tryb ekranu
			MessageBox(NULL,"Nie mo�na stworzy� okna.","B��D",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;         // zwr�� false
	}  
	static PIXELFORMATDESCRIPTOR pfd =         // pfd m�wi oknu co chcemy
	{
		sizeof(PIXELFORMATDESCRIPTOR),         // Rozmiar opisu piksela
		1,         // Numer wersji
		PFD_DRAW_TO_WINDOW |         // Format musi obs�ugiwa� okno
		PFD_SUPPORT_OPENGL |         // Format musi obs�ugiwa� OpenGL
		PFD_DOUBLEBUFFER,         // Musi obs�ugiwa� Podw�jne buforowanie
		PFD_TYPE_RGBA,         // i format RGBA
		bits,         // Wybieramy g��bie kolor�w
		0, 0, 0, 0, 0, 0,         // ignorujemy
		0,         // Bez bufora alpha
		0,         // Bit ignorujemy
		0,         // ignorujemy
		0, 0, 0, 0,         // ignorujemy
		16,         // 16 bitowy bufor Z
		0,         // ignorujemy
		0,         // ignorujemy
		PFD_MAIN_PLANE,         // G��wna warstwa rysowania
		0,         // zarezerwowane
		0, 0, 0         // ignorujemy maski warstw
	}; 
	if (!(hDC=GetDC(hWnd))) {        // Mamy kontekst urz�dzenia?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo�na stworzy� kontekstu urz�dzenia.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) {        // Czy windows znajdzie taki format pixela?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo�na znale�� ��danego formatu piksela.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!SetPixelFormat(hDC,PixelFormat,&pfd)) {        // Czy mo�emy ustawi� taki format
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo�na ustawi� ��danego formatu piksela.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!(hRC=wglCreateContext(hDC))) {         // Czy mo�emy pobra� hRC
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo�na stworzy� kontekstu renderowania.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	}
	if (!wglMakeCurrent(hDC,hRC)) {         // Czy mo�emy aktywowa� kontekst renderowania?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo�na aktywowa� kontekstu renderowania.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	ShowWindow(hWnd,SW_SHOW);         // Pokazujemy okno
	SetForegroundWindow(hWnd);         // Ustawiamy wy�szy priorytet
	SetFocus(hWnd);         // Dzia�anie klawiatury skierowujemy na okno
	ReSizeGLScene(width, height);         // Ustawiamy perspektyw�
	if (!InitGL())         // Czy GL zanicjowa� si� ?
	{
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Inicjacja GL niepomy�lna.","B��D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	return TRUE;
} 


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {         
	/*
		hWnd uchwyt do okna
		uMsg Wiadomo�� dla okna
		wParam Dodatkowe informajce wiadomo�ci
		lParam Dodatkowe informajce wiadomo�ci

	*/
	
	switch (uMsg) {         // Sprawd� komunikaty okna
		case WM_ACTIVATE:         // Czy to wiadomo�� aktywowania?
		{
			if (!HIWORD(wParam))         // Czy program jest aktywowany
			{
				active=TRUE;         // Program jest aktywny
			}
			else {
				active=FALSE;         // Program nie jest aktywny
			}
			return 0;         // Powr�� do p�tli wiadomo�ci
		} 
		case WM_SYSCOMMAND:         // Czy to komenda systemowa?
		{
			switch (wParam) {        // Sprawdzimy typ
			case SC_SCREENSAVE:         // wygaszacz ekranu chce si� w��czy�
			case SC_MONITORPOWER:         // Monitor chce si� wy��czy�
			return 0;         // Anulujemy wygaszacze itp.
			}
			break;         // koniec
		}
		case WM_CLOSE:         // Czy to rozkaz zamkni�cia?
		{
			PostQuitMessage(0);         // Wy�lij wiadomo�� zamkni�cia
			return 0;         // skocz dalej
		} 
		case WM_KEYDOWN:         // Czy klawisz zosta� wci�ni�ty
		{
			keys[wParam]=TRUE;         // Odpowiednie pole zostaje ustawione
			return 0;         // skocz dalej
		} 
		case WM_KEYUP:         // Czy klawisz zosta� podniesiony
		{
			keys[wParam]=FALSE;         // Odpowiednie pole zostaje ustawione na false
			return 0;         // skocz dalej
		} 
		case WM_SIZE:         // Czy okno si� zmieni�o ?
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));         // Zmieniamy scene OpenGL
			return 0;         // skocz dalej
		}
	} 
	 // Reszta wiadomo�ci idzie do windows
	return DefWindowProc(hWnd,uMsg,wParam,lParam);

} 

void keyboard() {
	if (keys[VK_PRIOR])         // Page Up zosta�o naci�ni�te?
	{	
		if (z > -40.0f)
		z-=0.05f;         // przesu� w g��b ekranu
	} 
	if (keys[VK_NEXT])         // Page Down zosta�o naci�ni�te?
	{
		if (z < 0.0f)
		z+=0.05f;         // przesu� w kierunku widza
	} 
	if (keys[VK_UP])         // Strza�ka w g�r� zosta�a wci�ni�ta?
	{
		xspeed+=0.01f;         // Je�li tak, zmniejsz xspeed
	}
	if (keys[VK_DOWN])         // Strza�ka w d� zosta�a wci�ni�ta?
	{
		xspeed-=0.01f;         // Je�li tak, zwi�ksz xspeed
	}
	if (keys[VK_LEFT])         // Strza�ka w prawo zosta�a wci�ni�ta?
	{
		yspeed+=0.01f;         // Je�li tak, zwi�ksz yspeed
	}
	if (keys[VK_RIGHT])         // Strza�ka w lewo zosta�a wci�ni�ta?
	{
		yspeed-=0.01f;         // Je�li tak, zmniejsz yspeed
	} 
	if (keys[VK_SPACE])			// Zatrzymaj ruch kamery
	{
		xspeed = 0.0f;
		yspeed = 0.0f;
	}
	if (keys['B'] && !bp)         // Czy klawisz B jest wci�ni�ty i bp jest r�wne FALSE?
    {
        bp=TRUE;         // Je�eli tak to bp = TRUE
        blend = !blend;         // Prze��cz mieszanie kolor�w TRUE/FALSE
        if(blend)         // Czy mieszanie kolor�w jest TRUE?
        {
            glEnable(GL_BLEND);         // W��cz mieszanie kolor�w
            glDisable(GL_DEPTH_TEST);         // Wy��cz testowanie g��boko�ci
        }
        else         // W przeciwnym wypadku
        {
            glDisable(GL_BLEND);         // Wy��cz mieszanie kolor�w
            glEnable(GL_DEPTH_TEST);         // W��cz testowanie g��boko�ci
        }
    }
    if (!keys['B'])         // Czy klawisz B zosta� puszczony
    {
        bp=FALSE;         // Je�li tak to bp=FALSE
    }
	if (keys['W'])
	{
		if (ballZSpeed[0] > -0.5f)
		ballZSpeed[0] -= 0.002f;
	}
	if (keys['S']) 
	{
		if (ballZSpeed[0] < 0.5f)
		ballZSpeed[0] += 0.002f;
	}
	if (keys['A'])
	{
		if (ballXSpeed[0] > -0.5f)
		ballXSpeed[0] -= 0.002f;
	}
	if (keys['D'])
	{
		if (ballXSpeed[0] < 0.5f)
		ballXSpeed[0] += 0.002f;
	}
}
int main(){
	MSG msg;         // Struktura przechowuj�ca komunikaty okna
	BOOL done=FALSE;         // Stan dzia�ania programu
	fullscreen=FALSE;         // tryb okienkowy
	
	char* message = "Sterowanie kulk�: W,S,A,D\n\n"
					"Obroty kamery:\n"
					"wok� osi x - strza�ki g�ra, d�\n"
					"wok� osi y - strza�ki lewo,prawo\n"
					"zatrzymanie kamery - spacja\n\n"
					"zoom in/out - page up/page down\n"
					"przezroczysto�� (on/off) - B\n"
					"fullscreen - F1\n"
					"pomoc - F2";
	MessageBox(NULL,message,"Sterowanie",MB_OK | MB_ICONINFORMATION);	
	if (!CreateGLWindow("Adam Nowik Bilard ver. 0.9",640,480,32,fullscreen)) {
		return 0;         // zako�cz program
	} 
	while (!done) { 
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {        // czy s� jakie� wiadomo�ci ? 
			if(msg.message==WM_QUIT)         // czy otrzymano wiadomo�� zamkni�cia ?
			{
				done=TRUE;         // skoro tak, to done = TRUE
			}
			else         // nie otrzymano wiadomo�ci zamkni�cia ?
			{ 
				TranslateMessage(&msg);         // wyt�umacz wiadomo��
				DispatchMessage(&msg);         // wy�lij j�
			}
		}
		else         // nie ma �adnych komunikat�w
		{ 
			        // Rysuj scen� OpenGL
			if(active)         // program jest aktywny ?
			{
				if(keys[VK_ESCAPE])         // czy wci�ni�ty jest ESC ?
				{
					done=TRUE;         // przerwanie warunku p�tli
				}
				else         // nie ma czasu na zamkni�cie, rysujemy scene
				{ 
					DrawGLScene();         // Rysuj scen�
					SwapBuffers(hDC);         // Zamie� bufory (ekrany)
					keyboard();
				}
			}	 
			if(keys[VK_F1])         // czy F1 jest wci�ni�te
			{
				keys[VK_F1]=FALSE;         // ustaw go na false, bo zosta� u�yty
				KillGLWindow();         // Zamknij okno
				fullscreen=!fullscreen;         // Zamie� pe�ny ekran)
					// Stw�rz nowe okno
				if(!CreateGLWindow("Adam Nowik Bilard ver. 0.9",640,480,32,fullscreen))
				{
					return 0;         // Wyst�pi� b��d
				}
			}
			if(keys[VK_F2])
			{
				keys[VK_F2] = FALSE;
				MessageBox(NULL,message,"Sterowanie",MB_OK | MB_ICONINFORMATION);	
			}
		}
	} 
	KillGLWindow();         // Zamknij OpenGL
	return (msg.wParam);         // Koniec programu
}