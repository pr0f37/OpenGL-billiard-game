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
bool active=TRUE;         // flaga zminimalizowania, domyœlnie na true
bool fullscreen=TRUE;         // tryb pe³noekranowy. domyœlnie na true

bool bp;         // flaga do B 
bool blend;

GLfloat xrot;         // Obrót na osi X
GLfloat yrot;         // Obrót na osi Y
GLfloat xspeed;         // Prêdkoœæ obracania siê X
GLfloat yspeed;         // Prêdkoœæ obracania siê Y
GLfloat z=-15.0f;         // Pozycja Z

GLfloat ballXPos[] = {2.5f,2.5f,2.0f,3.0f}; // pozycje X sfer
GLfloat ballZPos[] = {10.0f,3.0f,2.2f,2.2f}; // pozycje Z sfer
GLfloat ballXSpeed[] = {0.0f,0.0f,0.0f,0.0f}; // prêdkoœci X sfer
GLfloat ballZSpeed[] = {0.0f,0.0f,0.0f,0.0f}; // prêdkoœci Z sfer


GLfloat LightAmbient1[]={ 1.0f, 1.0f, 1.0f, 1.0f };         // Wartoœci œwiat³a otaczaj¹cego  
GLfloat LightAmbient2[]={ 1.0f, 1.0f, 1.0f, 1.0f };         // Wartoœci œwiat³a rozproszonego 
GLfloat LightPosition[]={ 0.0f, 0.0f, 2.0f, 1.0f };         // Pozycja œwiat³a 
GLfloat LightPosition2[]={ 0.0f, 5.0f, -10.0f, 1.0f };         // Pozycja œwiat³a 

GLuint texture[6];         // tekstury
GLuint table;         // miejsce na listê wyœwietlania
GLuint sky;

GLUquadricObj *quadratic;         // Kwadryki 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);         // Deklaracja funkcji WndProc 


AUX_RGBImageRec *LoadBMP(char *Filename)         // £aduje bitmapê z pliku
{
    FILE *File=NULL;         // wskaŸnik na plik
	if (!Filename)         // Upewnij siê ¿e podano nazwê pliku
    {
        return NULL;         // Je¿eli nie to zwróæ NULL
    } 
	File=fopen(Filename,"r");         // SprawdŸ czy plik istnieje 
	if (File)         // Czy plik istnieje
    {
        fclose(File);         // Zamknij plik
        return auxDIBImageLoad(Filename);         // Za³aduj bitmapê i zwróæ wskaŸnik
    } 
	return NULL;         // Je¿eli nie uda³o siê za³adowaæ pliku, zwróæ NULL
} 
int LoadGLTextures()         // £aduj bitmapê i przekszta³æ j¹ w teksturê
{ 
	int Status=FALSE;         // Status tworzenia tekstury 
	AUX_RGBImageRec *TextureImage[1];         // Rekord dla naszego obrazka 
	memset(TextureImage,0,sizeof(void *)*1);         // Wyzeruj wskaŸnik 
	    // Za³aduj obrazek
    if (TextureImage[0]=LoadBMP("wood3.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[0]);         // Stwórz obiekt tekstury 
		        // Stwórz mipmappow¹ teksturê
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 
    } 
	if (TextureImage[0]=LoadBMP("floor2.bmp")) // pod³oga
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[1]);         // Stwórz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); 
	}
	if (TextureImage[0]=LoadBMP("wall2.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[2]);         // Stwórz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("woodwall4.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[3]);         // Stwórz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("ball3.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[4]);         // Stwórz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0]=LoadBMP("red.bmp"))
    {
        Status=TRUE;         // Ustaw Status na true (powodzenie)
		glGenTextures(1, &texture[5]);         // Stwórz obiekt tekstury 
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); 
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	if (TextureImage[0])         // Je¿eli tekstura istnieje
    {
        if (TextureImage[0]->data)         // Je¿eli obrazek tekstury istnieje
        {
            free(TextureImage[0]->data);         // Zwolnij pamiêæ obrazka tekstury
        }
        free(TextureImage[0]);         // Zwolnij strukturê obrazka
    }
	return Status;         // Zwróæ status operacji
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
	if (height==0) {       // zapobiegnij dzieleniu przez zero...
		height=1;         // ...ustawiaj¹c liczbê 1
	}
	glViewport(0, 0, width, height);         // zresetuj pole widzenia
	glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
	glLoadIdentity();         // zresetuj j¹
        // oblicz perspektywê dla okna
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
	glLoadIdentity();         // zresetuj j¹
} 

GLvoid BuildLists()         // Stwórz listy wyœwietlania
{ 
	table=glGenLists(2);         // przydziel miejsce na dwie listy wyœwietlania 
	glNewList(table,GL_COMPILE);         // nowa lista wyœwietlania. pude³ko, skompilowana 
	glBegin(GL_QUADS);
        // Przednia œciana
		glNormal3f( 0.0f, 0.0f, 1.0f);         // Normalna wskazuj¹ca na widza
        glTexCoord2f(0.0f, 0.0f); glVertex3f(LCOORD, DCOORD, FGCOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);         // Prawy dolny wierzcho³ek tekstury i czworok¹ta
        glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
        glTexCoord2f(0.0f, 1.0f); glVertex3f(LCOORD, UCOORD, FGCOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Prawa œciana
		glNormal3f( 1.0f, 0.0f, 0.0f);         // Normalna wskazuj¹ca na widza
        glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);		// Prawy górny	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);		// Lewy górny
		// Tylna œciana
		glNormal3f( 0.0f, 0.0f, -1.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);		// Prawy górny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);		// Lewy górny
		// Lewa œciana
		glNormal3f( -1.0f, 0.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Lewy dolny
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, DCOORD, FGCOORD);		// Prawy dolny
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);		// Prawy górny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);		// Lewy górny
		// Spód
		glNormal3f( 0.0f, -1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, DCOORD, FGCOORD);         // Lewy dolny 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, DCOORD, FGCOORD);         // Prawy dolny 
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RCOORD, DCOORD, BGCOORD);		 // Prawy górny
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LCOORD, DCOORD, BGCOORD);		// Lewy górny
		// Przednia góra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Prawa góra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, UCOORD, FGCOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD,BGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Tylna góra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RCOORD, UCOORD, BGCOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LICOORD, UCOORD, BGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Lewa góra
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LCOORD, UCOORD, BGCOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( LCOORD, UCOORD, FGCOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, BGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Przednia wewnêtrzna
		glNormal3f( 0.0f, 0.0f, -1.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( LICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Prawy lewy wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( LICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Prawa wewnêtrzna
		glNormal3f( -1.0f, 0.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);         // Prawy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Tylna wewnêtrzna
		glNormal3f( 0.0f, 0.0f, 1.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f(LICOORD, DICOORD, BGICOORD);         // Prawy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f(LICOORD, UCOORD, BGICOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f( RICOORD, UCOORD, BGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Lewa wewnêtrzna
		glNormal3f( 1.0f, 0.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f(LICOORD, DICOORD, FGICOORD);         // Lewy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 0.0f); glVertex3f(LICOORD, DICOORD,BGICOORD);         // Prawy dolny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(1.0f, 1.0f); glVertex3f(LICOORD, UCOORD,BGICOORD);         // Prawy górny wierzcho³ek tekstury i czworok¹ta
		glTexCoord2f(0.0f, 1.0f); glVertex3f(LICOORD, UCOORD, FGICOORD);         // Lewy górny wierzcho³ek tekstury i czworok¹ta
		// Powierzchnia sto³u
		glNormal3f( 0.0f, 1.0f, 0.0f);         // Normalna wskazuj¹ca na widza
		glTexCoord2f(0.0f, 0.0f); glVertex3f(LICOORD, DICOORD, FGICOORD);         // Lewy dolny 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( RICOORD, DICOORD, FGICOORD);         // Prawy dolny 
		glTexCoord2f(1.0f, 1.0f); glVertex3f( RICOORD, DICOORD, BGICOORD);		 // Prawy górny
		glTexCoord2f(0.0f, 1.0f); glVertex3f(LICOORD, DICOORD, BGICOORD);		// Lewy górny
	glEnd();
	glEndList();         // zakoñcz budowanie listy 

	sky = table + 1;
	glNewList(sky,GL_COMPILE);
	glBegin(GL_QUADS);
	// przednia œciana
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 45.0f, 40.0f);		//Lewy górny wierzcho³ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho³ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho³ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 45.0f, 40.0f);			// Prawy górny wierzcho³ek
	// prawa œciana
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 45.0f, 40.0f);		//Lewy górny wierzcho³ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho³ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, -40.0f); // Prawy dolny wierzcho³ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 45.0f, -40.0f);			// Prawy górny wierzcho³ek
	// tylna œciana
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 45.0f, -40.0f);		//Lewy górny wierzcho³ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, -5.0f, -40.0f); // Lewy dolny wierzcho³ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, -5.0f, -40.0f); // Prawy dolny wierzcho³ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 45.0f, -40.0f);			// Prawy górny wierzcho³ek
	// lewa œciana
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 45.0f, -40.0f);		//Lewy górny wierzcho³ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, -40.0f); // Lewy dolny wierzcho³ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho³ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 45.0f, 40.0f);			// Prawy górny wierzcho³ek
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	// spód
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, -5.0f, -40.0f);		//Lewy górny wierzcho³ek
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, -5.0f, 40.0f); // Lewy dolny wierzcho³ek
		glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, -5.0f, 40.0f); // Prawy dolny wierzcho³ek
		glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, -5.0f, -40.0f);			// Prawy górny wierzcho³ek
	glEnd();
	glEndList();
	
}

void leg() { // rysowanie nogi sto³u
	GLfloat height = 5.0f;
	glPushMatrix();
		glTranslatef(0.0f, -5.0f, 0.0f);         // Przesuñ
		glRotatef(-90.0f,1.0f,0.0f,0.0f);
		gluCylinder(quadratic, 0.1f, 0.1f, 0.4f,32,10);
		glTranslatef(0.0f, 0.0f, 0.4f);         // Przesuñ
		gluCylinder(quadratic, 0.1f, 0.13f, 0.1f,32,10);
		glRotatef(90.0f,1.0f,0.0f,0.0f);
		glTranslatef(0.0f, 0.15f, 0.0f);
		gluSphere(quadratic, 0.14f, 32, 32);
		glTranslatef(0.0f, 0.12f, 0.0f);         // Przesuñ 
		glRotatef(-90.0f,1.0f,0.0f,0.0f);
		gluCylinder(quadratic, 0.08f, 0.2f, 2.5f,32,32);
		glTranslatef(0.0f, 0.0f, 2.5f);         // Przesuñ 
		gluCylinder(quadratic, 0.2f, 0.18f, 0.8f,32,32);		
		glTranslatef(0.0f, 0.0f, 0.8f);         // Przesuñ 
		gluCylinder(quadratic, 0.18f, 0.22f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesuñ 
		gluCylinder(quadratic, 0.22f, 0.14f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesuñ
		gluCylinder(quadratic, 0.14f, 0.22f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesuñ
		gluCylinder(quadratic, 0.22f, 0.25f, 0.1f,32,32);
		glTranslatef(0.0f, 0.0f, 0.1f);         // Przesuñ
		gluCylinder(quadratic, 0.25f, 0.25f, 0.7f,32,10);
	glPopMatrix();
}
GLfloat ballPosition(GLfloat* ball, GLfloat* plane) { // wyliczanie pozycji kuli w stosunku do danej œciany sto³u
	GLfloat position = 0;
	for (int i = 0; i < 3; i++)
		position += ball[i] * plane[i];
	return position;
}
void hit(GLfloat& xPos, GLfloat& zPos, GLfloat& xSpeed, GLfloat& zSpeed){ // funkcja wyliczaj¹ca zderzenie ze œcian¹
	
	GLfloat frontPlane[] = {0.0f, 0.0f, 1.0f, 10.77f}; // powiekszone o promieñ kuli
	GLfloat rightPlane[] = {1.0f, 0.0f, 0.0f, 4.85f};
	GLfloat backPlane[] = {0.0f, 0.0f, 1.0f, 0.1f};
	GLfloat leftPlane[] = {1.0f, 0.0f, 0.0f, 0.15f};
	GLfloat vectLenght = 0;
	if (ballXSpeed[0] != 0 || ballZSpeed[0] !=0) // ¿eby nie wyci¹gaæ pierwiastka z 0
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
	if (!LoadGLTextures())         // Za³aduj tekstury 
    {
        return FALSE;         // Je¿eli tekstury nie za³adowano zrwóæ FALSE 
    }
	BuildLists();         // stwórz listy wyœwietlania
    glEnable(GL_TEXTURE_2D);         // W³¹cz nak³adanie tekstur 
    glShadeModel(GL_SMOOTH);         // W³¹cz g³adkie cieniowanie
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);         // Czarne t³o
    glClearDepth(1.0f);         // Ustawienie bufora g³êbi
    glEnable(GL_DEPTH_TEST);         // W³¹cz testowanie g³êbi
    glDepthFunc(GL_LEQUAL);         // Sposób testowania g³êbi (mniejsze, równe)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);         // Dobre obliczenia perspektywy
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightAmbient1);         // Ustaw œwiat³o otaczaj¹ce 
	glLightfv(GL_LIGHT2, GL_POSITION,LightPosition2);         // Pozycja œwiat³a 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightAmbient2);         // Ustaw œwiat³o otaczaj¹ce 
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);         // Pozycja œwiat³a 
	glEnable(GL_LIGHT1);         // W³¹cz œwiat³o numer jeden
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);         // W³¹cz œwiat³o
	glColor4f(1.0f,1.0f,1.0f,0.5f);         // Pe³na jasnoœæ, wspó³czynnik alpha = 50%
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);         // Funkcja mieszaj¹ca kolory dla pó³przezroczystoœci oparta o wartoœæ Ÿród³ow¹ alpha ( NOWE ) 
	quadratic=gluNewQuadric();         // wskaŸnik na kwadrykê
    gluQuadricNormals(quadratic, GLU_SMOOTH);         // wyg³adzanie kwadryk
    gluQuadricTexture(quadratic, GL_TRUE);         // Tworzy wspó³rzêdne tekstury 
	return TRUE;
}

int DrawGLScene(GLvoid)         // funkcja rysuj¹ca
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Wyczyœæ ekran
	glLoadIdentity();         // Zresetuj aktualn¹ macierz
	glTranslatef(0.0f,-1.0f,z);         // Przesuñ ekran 
	glRotatef(xrot,0.1f,0.0f,0.0f);         // Obróæ na osi X
	glRotatef(yrot,0.0f,0.1f,0.0f);         // Obróæ na osi Y
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glCallList(sky);
		glBindTexture(GL_TEXTURE_2D, texture[0]);          
		glCallList(table);         // narusuj stó³
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTranslatef(LCOORD+0.5f, 0.0f, FGCOORD-0.5f);         // Przesuñ w ekran 5 jednostek
		leg();
		glTranslatef(5.0f, 0.0f, 0.0f);         // Przesuñ w ekran 5 jednostek
		leg();
		glTranslatef(0.0f, 0.0f, -11.0f);         // Przesuñ w ekran 5 jednostek
		leg();
		glTranslatef(-5.0f, 0.0f, 0.0f);         // Przesuñ w ekran 5 jednostek
		leg();	
		glBindTexture(GL_TEXTURE_2D, texture[4]); // tekstura dla kuli 1
		hit(ballXPos[0], ballZPos[0], ballXSpeed[0], ballZSpeed[0]);
		hitBall(ballXPos, ballZPos, ballXSpeed, ballZSpeed);
		motion(ballXPos[0], ballZPos[0], ballXSpeed[0], ballZSpeed[0]);
		glPushMatrix();
			glTranslatef(ballXPos[0], 0.59f, ballZPos[0]);
			glRotatef((fabs(ballZPos[0])*250.0f)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			glRotatef((fabs(ballXPos[0])*250.0f)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kulê 1		
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, texture[5]);	// tekstura dla kuli 2
		hit(ballXPos[1], ballZPos[1], ballXSpeed[1], ballZSpeed[1]);
		motion(ballXPos[1], ballZPos[1], ballXSpeed[1], ballZSpeed[1]);
		glPushMatrix();
			glTranslatef(ballXPos[1], 0.59f, ballZPos[1]);
			//glRotatef((ballZPos[1]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[1]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kulê 2
		glPopMatrix();
		hit(ballXPos[2], ballZPos[2], ballXSpeed[2], ballZSpeed[2]);
		motion(ballXPos[2], ballZPos[2], ballXSpeed[2], ballZSpeed[2]);
		glPushMatrix();
			glTranslatef(ballXPos[2], 0.59f, ballZPos[2]);
			//glRotatef((ballZPos[2]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[2]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kulê 3
		glPopMatrix();
		hit(ballXPos[3], ballZPos[3], ballXSpeed[3], ballZSpeed[3]);
		motion(ballXPos[3], ballZPos[3], ballXSpeed[3], ballZSpeed[3]);
		glPushMatrix();
			glTranslatef(ballXPos[3], 0.59f, ballZPos[3]);
			//glRotatef((ballZPos[3]*250)/(2*3.1415f*0.4f), 1.0f, 0.0f, 0.0f);
			//glRotatef((ballXPos[3]*250)/(2*3.1415f*0.4f), 0.0f, 1.0f, 0.0f);
			gluSphere(quadratic,0.4f,32,32);         // Rysujemy kulê 4
		glPopMatrix();
	glPopMatrix();
	
    
	if (xrot < -5.0f || xrot > 185.0f) 
		xspeed = -xspeed;
	xrot+=xspeed;         // Obrót na osi X
    yrot+=yspeed;         // Obrót na osi Y
  	return TRUE;         // wszystko jest ok
} 


GLvoid KillGLWindow(GLvoid)         // zamknij okno
{ 
	gluDeleteQuadric(quadratic);         // Usuwamy kwadrykê - zwalniamy zasoby
	if (fullscreen) { 
		ChangeDisplaySettings(NULL,0);         // przywróc rozdzielczoœæ
		ShowCursor(TRUE);         // poka¿ kursor
	} 
	if (hRC) {         // mamy kontekst renderu? 
		if (!wglMakeCurrent(NULL,NULL)) {        // Czy jesteœmy w stanie zwolniæ go 
			MessageBox(NULL,"Nie mo¿na zwolniæ hDC lub hRC!","B£¥D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC)) {        // Czy mo¿emy usun¹æ kontekst renderu?
			MessageBox(NULL,"Nie mo¿na usun¹æ kontekstu renderowania","B£¥D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}
	if (hDC && !ReleaseDC(hWnd,hDC)) {        // Czy mo¿emy zwolniæ hDC
		MessageBox(NULL,"Nie mo¿na zwolniæ kontekstu urz¹dzenia (DC)","B£¥D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;         // Ustawiamy DC na NULL
	} 
	if (hWnd && !DestroyWindow(hWnd)) {        // Czy mo¿emy zwolniæ uchwyt okna?	
		MessageBox(NULL,"Nie mo¿na zwolniæ hWnd","B£¥D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;         // Ustaw hWnd na Null
	} 
	if (!UnregisterClass("OpenGL",hInstance)) {        // Czy mo¿emy wyrejstrowaæ klasê okna?
		MessageBox(NULL,"Nie mo¿na wyrejstrowaæ klasy okna","B£¥D ZAMYKANIA",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;         // Ustawiamy instancjê na NULL
	}
}


bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag){
	GLuint PixelFormat;         // Bêdzie przechowywaæ znaleziony format piksela 
	WNDCLASS wc;         // Struktura klasy okna 
	DWORD dwExStyle;         // Rozszerzony styl okna
	DWORD dwStyle;         // Normalny styl okna
	RECT WindowRect;         // Tu bêd¹ rozmiary okna
	WindowRect.left=(long)0;         // Pocz¹tek szerokoœci (od lewej) ma 0
	WindowRect.right=(long)width;         // Szerokoœc bierzemy z parametru naszej funkcji
	WindowRect.top=(long)0;         // Wysokoœæ te¿ zaczynamy od 0 (od góry)
	WindowRect.bottom=(long)height;         // Ustawiamy wysokoœæ z parametru naszej funkcji
	fullscreen=fullscreenflag;         // Ustawiamy globaln¹ zmienn¹ 
	hInstance = GetModuleHandle(NULL);         // Pobieramy instancjê dla okna
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;         // Ustawiamy odmalowywanie
	wc.lpfnWndProc = (WNDPROC) WndProc;         // WndProc bêdzie obs³ugiwaæ komunikaty
	wc.cbClsExtra = 0;         // nie wa¿ne
	wc.cbWndExtra = 0;         // nie wa¿ne
	wc.hInstance = hInstance;         // Ustawiamy instancjê
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);         // Ikona domyœlna
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);         // Kursor - strza³ka
	wc.hbrBackground = NULL;         // T³o
	wc.lpszMenuName = NULL;         // Nie chcemy menu
	wc.lpszClassName = "OpenGL";         // Nazwa klasy okna
	if (!RegisterClass(&wc)) {        // Spróbuj zarejstrowaæ klasê okna
		MessageBox(NULL,"Nie uda³o siê zarejstrowaæ klasy okna","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zakoñcz i zwróæ fa³sz.
	} 
	if (fullscreen){         // Czy ma byæ pe³ny ekran ?
		DEVMODE dmScreenSettings;         // Tryb karty graficznej
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));         // Wyczyœæ pamiêæ
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);         // Ustaw rozmiar tej struktury
		dmScreenSettings.dmPelsWidth = width;         // Wybierz ¿¹dan¹ szerokoœæ
		dmScreenSettings.dmPelsHeight = height;         // Wybierz ¿¹dan¹ wysokoœæ
		dmScreenSettings.dmBitsPerPel = bits;         // Wybierz g³êbie kolorów
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT; 
	       // Spróbuj ustawiæ pe³ny ekran. CDS_FULLSCREEN usuwa pasek start.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) { 
			        // Jeœli siê nie uda, przejdŸ do okna lub zakoñcz program
			if(MessageBox(NULL,"Tryb graficzny nie jest obs³ugiwany przez twoj¹ kartê graf. Czy u¿yæ zamiast niego okna?","",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {
				fullscreen=FALSE;         // Tryb okienkowy. fullscreen na false.
			}
			else {
				// Uwaga o zamykaniu
				MessageBox(NULL,"Program teraz siê zamknie.","B£¥D",MB_OK|MB_ICONSTOP);
				return FALSE;         // Zakoñcz i zwróæ false
			}
		}
	}
	if(fullscreen) {         // Wci¹¿ jesteœmy w trybie pe³noekranowym
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
		title,         // Tytu³ okna
		WS_CLIPSIBLINGS |         // Wymagane style okna
		WS_CLIPCHILDREN |         // Wymagane style okna
		dwStyle,         // Wybrane style okna
		0, 0,         // Pozycja okna
		WindowRect.right-WindowRect.left,         // Szerokoœæ
		WindowRect.bottom-WindowRect.top,         // Wysokoœæ
		NULL,         // Nie u¿ywamy okna potomnego
		NULL,         // ¯adnego menu
		hInstance,         // Instancja
		NULL))) {         // Nie dawaj nic do WM_CREAT
			KillGLWindow();         // Zresetuj tryb ekranu
			MessageBox(NULL,"Nie mo¿na stworzyæ okna.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;         // zwróæ false
	}  
	static PIXELFORMATDESCRIPTOR pfd =         // pfd mówi oknu co chcemy
	{
		sizeof(PIXELFORMATDESCRIPTOR),         // Rozmiar opisu piksela
		1,         // Numer wersji
		PFD_DRAW_TO_WINDOW |         // Format musi obs³ugiwaæ okno
		PFD_SUPPORT_OPENGL |         // Format musi obs³ugiwaæ OpenGL
		PFD_DOUBLEBUFFER,         // Musi obs³ugiwaæ Podwójne buforowanie
		PFD_TYPE_RGBA,         // i format RGBA
		bits,         // Wybieramy g³êbie kolorów
		0, 0, 0, 0, 0, 0,         // ignorujemy
		0,         // Bez bufora alpha
		0,         // Bit ignorujemy
		0,         // ignorujemy
		0, 0, 0, 0,         // ignorujemy
		16,         // 16 bitowy bufor Z
		0,         // ignorujemy
		0,         // ignorujemy
		PFD_MAIN_PLANE,         // G³ówna warstwa rysowania
		0,         // zarezerwowane
		0, 0, 0         // ignorujemy maski warstw
	}; 
	if (!(hDC=GetDC(hWnd))) {        // Mamy kontekst urz¹dzenia?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo¿na stworzyæ kontekstu urz¹dzenia.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) {        // Czy windows znajdzie taki format pixela?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo¿na znaleŸæ ¿¹danego formatu piksela.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!SetPixelFormat(hDC,PixelFormat,&pfd)) {        // Czy mo¿emy ustawiæ taki format
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo¿na ustawiæ ¿¹danego formatu piksela.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	if (!(hRC=wglCreateContext(hDC))) {         // Czy mo¿emy pobraæ hRC
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo¿na stworzyæ kontekstu renderowania.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	}
	if (!wglMakeCurrent(hDC,hRC)) {         // Czy mo¿emy aktywowaæ kontekst renderowania?
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Nie mo¿na aktywowaæ kontekstu renderowania.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	ShowWindow(hWnd,SW_SHOW);         // Pokazujemy okno
	SetForegroundWindow(hWnd);         // Ustawiamy wy¿szy priorytet
	SetFocus(hWnd);         // Dzia³anie klawiatury skierowujemy na okno
	ReSizeGLScene(width, height);         // Ustawiamy perspektywê
	if (!InitGL())         // Czy GL zanicjowa³ siê ?
	{
		KillGLWindow();         // Resetujemy ekran
		MessageBox(NULL,"Inicjacja GL niepomyœlna.","B£¥D",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;         // zwracamy false
	} 
	return TRUE;
} 


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {         
	/*
		hWnd uchwyt do okna
		uMsg Wiadomoœæ dla okna
		wParam Dodatkowe informajce wiadomoœci
		lParam Dodatkowe informajce wiadomoœci

	*/
	
	switch (uMsg) {         // SprawdŸ komunikaty okna
		case WM_ACTIVATE:         // Czy to wiadomoœæ aktywowania?
		{
			if (!HIWORD(wParam))         // Czy program jest aktywowany
			{
				active=TRUE;         // Program jest aktywny
			}
			else {
				active=FALSE;         // Program nie jest aktywny
			}
			return 0;         // Powróæ do pêtli wiadomoœci
		} 
		case WM_SYSCOMMAND:         // Czy to komenda systemowa?
		{
			switch (wParam) {        // Sprawdzimy typ
			case SC_SCREENSAVE:         // wygaszacz ekranu chce siê w³¹czyæ
			case SC_MONITORPOWER:         // Monitor chce siê wy³¹czyæ
			return 0;         // Anulujemy wygaszacze itp.
			}
			break;         // koniec
		}
		case WM_CLOSE:         // Czy to rozkaz zamkniêcia?
		{
			PostQuitMessage(0);         // Wyœlij wiadomoœæ zamkniêcia
			return 0;         // skocz dalej
		} 
		case WM_KEYDOWN:         // Czy klawisz zosta³ wciœniêty
		{
			keys[wParam]=TRUE;         // Odpowiednie pole zostaje ustawione
			return 0;         // skocz dalej
		} 
		case WM_KEYUP:         // Czy klawisz zosta³ podniesiony
		{
			keys[wParam]=FALSE;         // Odpowiednie pole zostaje ustawione na false
			return 0;         // skocz dalej
		} 
		case WM_SIZE:         // Czy okno siê zmieni³o ?
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));         // Zmieniamy scene OpenGL
			return 0;         // skocz dalej
		}
	} 
	 // Reszta wiadomoœci idzie do windows
	return DefWindowProc(hWnd,uMsg,wParam,lParam);

} 

void keyboard() {
	if (keys[VK_PRIOR])         // Page Up zosta³o naciœniête?
	{	
		if (z > -40.0f)
		z-=0.05f;         // przesuñ w g³¹b ekranu
	} 
	if (keys[VK_NEXT])         // Page Down zosta³o naciœniête?
	{
		if (z < 0.0f)
		z+=0.05f;         // przesuñ w kierunku widza
	} 
	if (keys[VK_UP])         // Strza³ka w górê zosta³a wciœniêta?
	{
		xspeed+=0.01f;         // Jeœli tak, zmniejsz xspeed
	}
	if (keys[VK_DOWN])         // Strza³ka w dó³ zosta³a wciœniêta?
	{
		xspeed-=0.01f;         // Jeœli tak, zwiêksz xspeed
	}
	if (keys[VK_LEFT])         // Strza³ka w prawo zosta³a wciœniêta?
	{
		yspeed+=0.01f;         // Jeœli tak, zwiêksz yspeed
	}
	if (keys[VK_RIGHT])         // Strza³ka w lewo zosta³a wciœniêta?
	{
		yspeed-=0.01f;         // Jeœli tak, zmniejsz yspeed
	} 
	if (keys[VK_SPACE])			// Zatrzymaj ruch kamery
	{
		xspeed = 0.0f;
		yspeed = 0.0f;
	}
	if (keys['B'] && !bp)         // Czy klawisz B jest wciœniêty i bp jest równe FALSE?
    {
        bp=TRUE;         // Je¿eli tak to bp = TRUE
        blend = !blend;         // Prze³¹cz mieszanie kolorów TRUE/FALSE
        if(blend)         // Czy mieszanie kolorów jest TRUE?
        {
            glEnable(GL_BLEND);         // W³¹cz mieszanie kolorów
            glDisable(GL_DEPTH_TEST);         // Wy³¹cz testowanie g³êbokoœci
        }
        else         // W przeciwnym wypadku
        {
            glDisable(GL_BLEND);         // Wy³¹cz mieszanie kolorów
            glEnable(GL_DEPTH_TEST);         // W³¹cz testowanie g³êbokoœci
        }
    }
    if (!keys['B'])         // Czy klawisz B zosta³ puszczony
    {
        bp=FALSE;         // Jeœli tak to bp=FALSE
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
	MSG msg;         // Struktura przechowuj¹ca komunikaty okna
	BOOL done=FALSE;         // Stan dzia³ania programu
	fullscreen=FALSE;         // tryb okienkowy
	
	char* message = "Sterowanie kulk¹: W,S,A,D\n\n"
					"Obroty kamery:\n"
					"wokó³ osi x - strza³ki góra, dó³\n"
					"wokó³ osi y - strza³ki lewo,prawo\n"
					"zatrzymanie kamery - spacja\n\n"
					"zoom in/out - page up/page down\n"
					"przezroczystoœæ (on/off) - B\n"
					"fullscreen - F1\n"
					"pomoc - F2";
	MessageBox(NULL,message,"Sterowanie",MB_OK | MB_ICONINFORMATION);	
	if (!CreateGLWindow("Adam Nowik Bilard ver. 0.9",640,480,32,fullscreen)) {
		return 0;         // zakoñcz program
	} 
	while (!done) { 
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {        // czy s¹ jakieœ wiadomoœci ? 
			if(msg.message==WM_QUIT)         // czy otrzymano wiadomoœæ zamkniêcia ?
			{
				done=TRUE;         // skoro tak, to done = TRUE
			}
			else         // nie otrzymano wiadomoœci zamkniêcia ?
			{ 
				TranslateMessage(&msg);         // wyt³umacz wiadomoœæ
				DispatchMessage(&msg);         // wyœlij j¹
			}
		}
		else         // nie ma ¿adnych komunikatów
		{ 
			        // Rysuj scenê OpenGL
			if(active)         // program jest aktywny ?
			{
				if(keys[VK_ESCAPE])         // czy wciœniêty jest ESC ?
				{
					done=TRUE;         // przerwanie warunku pêtli
				}
				else         // nie ma czasu na zamkniêcie, rysujemy scene
				{ 
					DrawGLScene();         // Rysuj scenê
					SwapBuffers(hDC);         // Zamieñ bufory (ekrany)
					keyboard();
				}
			}	 
			if(keys[VK_F1])         // czy F1 jest wciœniête
			{
				keys[VK_F1]=FALSE;         // ustaw go na false, bo zosta³ u¿yty
				KillGLWindow();         // Zamknij okno
				fullscreen=!fullscreen;         // Zamieñ pe³ny ekran)
					// Stwórz nowe okno
				if(!CreateGLWindow("Adam Nowik Bilard ver. 0.9",640,480,32,fullscreen))
				{
					return 0;         // Wyst¹pi³ b³¹d
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