/* Infographie, labo 9
 *
 * Nom     : Chien de l'espace
 * Fichier : spaceDog.c
 * Auteur  : Nicolas Seriot, EIVD, EI4b
 * Date    : début 2004
 */

// disable a harmless warning on Win32
// see http://www.opengl.org/resources/faq/technical/gettingstarted.htm
#ifdef WIN32
#pragma warning(disable:4305)
#endif

#include <stdlib.h>    /* pour la fonction exit()  */
#include <string.h>    /* pour les fonctions relatives aux chaînes de caractères */
#include <stdio.h>     /* pour les entrées-sorties */
#include <math.h>      /* pour les fonctions trigonométriques */
#include "materials.h" /* pour les différentes matières */
#include "chien.h"     /* pour les objets qui composent le chien */

#include <jpeglib.h>

/* inclusion de la GLUT */
#ifdef __APPLE__
#include <GLUT/glut.h>   /* pour Mac OS X            */
#include <jerror.h>
#else
#include <GL/glut.h>     /* pour les autres systèmes */
#endif

#define NB_TEXTURES 4

#define BOIS_ID     0
#define EIVD_ID     1
#define CODE_ID     2
#define PHOTO_ID	3

void loadJpegImage(char *fichier);

//tableaux associés à l'utilisation de textures 
//contient le numéro associé à chaque texture
GLuint texId[NB_TEXTURES];
//sert à stocker la texture sous forme de tableau à 3 dimensions
unsigned char texture[256][256][3];
//sert à stocker la texture sous forme d'un tableau à 1 dimension
unsigned char image[256*256*3];

/* switch globaux */
GLboolean light_0_Switch   = GL_TRUE;
GLboolean light_1_Switch   = GL_TRUE;
GLboolean light_2_Switch   = GL_FALSE;
GLboolean light_3_Switch   = GL_FALSE;
GLboolean diviserFenetre   = GL_TRUE;
GLboolean fullscreen       = GL_FALSE;
GLboolean mouvement        = GL_FALSE;
GLboolean affichageAxes    = GL_TRUE;
GLboolean shadeModelSmooth = GL_TRUE;
GLboolean brouillard       = GL_FALSE;

GLint xOld, yOld;

/* couleurs */
static GLfloat rouge[]  = {1.0, 0.0, 0.0, 1.0};
static GLfloat vert[]   = {0.0, 1.0, 0.0, 1.0};
static GLfloat bleu[]   = {0.0, 0.0, 1.0, 1.0};
static GLfloat noir[]   = {0.0, 0.0, 0.0, 1.0};
static GLfloat gris03[] = {0.3, 0.3, 0.3, 1.0};
static GLfloat gris08[] = {0.8, 0.8, 0.8, 1.0};

/* les dimensions de la fenêtre */
GLsizei fenetreLargeur;
GLsizei fenetreHauteur;

float rapport = 1.0;   /* rapport entre la largeur et la hauteur de la fenêtre */

int   roll    = -20;   /* angle roulis  - rotation autour de l'axe Z */
int   pitch   = -42;   /* angle tangage - rotation autour de l'axe Y */
int   heading =  10;   /* angle cap     - rotation autour de l'axe X */

GLfloat rotationX = 30;
GLfloat rotationZ = 0;
GLfloat rotationY = 0;

/* position de l'observateur */
float X = 4.0;
float Y = 1.2;
float Z = 3.9;

/* ouverture dans la direction de y */
int ouverture = 45; 

/* conserve la position de la souris */
int mouseX = 0;
int mouseY = 0;

/* définition des menus */
GLint menuPrincipal, menuMatiere, menuLumiere;

/* fenêtre Glut */
GLuint principale, infos;

static char label[100]; /* pour stocker les caractères à afficher */

// les lumières

GLfloat light0_pos[]={5, -3, -5, 1};
GLfloat light0_Ka[]={0, 0, 0, 1};
GLfloat light0_Kd[]={1, 1, 1, 1};
GLfloat light0_Ks[]={1, 1, 1, 1};

GLfloat light1_pos[]={-3, 5, -7, 1}; 
GLfloat light1_Ka[]={0, 0, 0, 1};
GLfloat light1_Kd[]={0,0, 0.8, 1};
GLfloat light1_Ks[]={0,0, 1, 1};

GLfloat light2_pos[]={6, -4, 5, 1};  
GLfloat light2_Ka[]={0.1, 0, 0, 1};
GLfloat light2_Kd[]={0.8, 0.2, 0.2, 1};
GLfloat light2_Ks[]={1, 1, 1, 1};

GLfloat light3_pos[]={4, 4, 4, 1};  
GLfloat light3_Ka[]={0.0, 0.0, 0.0, 1.0};
GLfloat light3_Kd[]={0.3, 1.0, 1.0, 1.0};
GLfloat light3_Ks[]={0.0, 0.3, 1.0, 1.0};

GLfloat spot3_direction[] = { -1.0, -1.0, -1.0 };

/* la matière courante */
GLfloat material_Ka[4];
GLfloat material_Kd[4];
GLfloat material_Ks[4];
GLfloat material_Ke[4];

GLfloat material_Se;

/* la matière de l'objet */

GLfloat obj_mat_Ka[4];
GLfloat obj_mat_Kd[4];
GLfloat obj_mat_Ks[4];
GLfloat obj_mat_Ke[4];

GLfloat obj_mat_Se;

void dessinerCube(float c);

void loadJpegImage(char *fichier) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *file; 
    unsigned char *ligne;
    int i,j;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    if ((file=fopen(fichier,"rb"))==NULL)
    {
        fprintf(stderr,"Erreur : impossible d'ouvrir le fichier de texture\n");
        exit(1);
    }

    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);
    
    if ((cinfo.image_width!=256)||(cinfo.image_height!=256)) {
        printf("%s %d %c %d \n","Erreur : l''image doit etre de taille ",256,'x',256);
        exit(1);
    }
    
    if (cinfo.jpeg_color_space==JCS_GRAYSCALE) 
    {
        fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
        exit(1);
    }

    jpeg_start_decompress(&cinfo);
    ligne=image;
    while (cinfo.output_scanline<cinfo.output_height)
    {
        ligne=image+3*256*cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo,&ligne,1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
  
    for (i=0;i<256;i++)
        for (j=0;j<256;j++) 
        {
            texture[i][j][0]=image[i*256*3+j*3];
            texture[i][j][1]=image[i*256*3+j*3+1];
            texture[i][j][2]=image[i*256*3+j*3+2];
        }
}

/* fonction permettant de modifier la matière courante
 */
void new_material(double* material) {
    material_Ka[0] = material[0];
    material_Ka[1] = material[1];
    material_Ka[2] = material[2];
    material_Ka[3] = material[3];
    material_Kd[0] = material[4];
    material_Kd[1] = material[5];
    material_Kd[2] = material[6];
    material_Kd[3] = material[7];
    material_Ks[0] = material[8];
    material_Ks[1] = material[9];
    material_Ks[2] = material[10];
    material_Ks[3] = material[11];
    material_Ke[0] = 0;
    material_Ke[1] = 0;
    material_Ke[2] = 0;
    material_Ke[3] = 0;
    material_Se = material[12];
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
    glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
    glMaterialfv(GL_FRONT, GL_SHININESS, &material_Se);
}

/* fonction permettant de déterminer la matière de l'objet
 */
void new_obj_mat(double* material) {
    obj_mat_Ka[0] = material[0];
    obj_mat_Ka[1] = material[1];
    obj_mat_Ka[2] = material[2];
    obj_mat_Ka[3] = material[3];
    obj_mat_Kd[0] = material[4];
    obj_mat_Kd[1] = material[5];
    obj_mat_Kd[2] = material[6];
    obj_mat_Kd[3] = material[7];
    obj_mat_Ks[0] = material[8];
    obj_mat_Ks[1] = material[9];
    obj_mat_Ks[2] = material[10];
    obj_mat_Ks[3] = material[11];
    obj_mat_Ke[0] = 0;
    obj_mat_Ke[1] = 0;
    obj_mat_Ke[2] = 0;
    obj_mat_Ke[3] = 0;
    obj_mat_Se = material[12];
}

/* dessiner les axes du repaire orthonormée */
void axes(float longueur, float largeur) {

    /* Ox rouge */
    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, rouge);
    glMaterialfv(GL_FRONT, GL_AMBIENT, rouge);
    glMaterialfv(GL_FRONT, GL_SPECULAR, rouge);
    glMaterialfv(GL_FRONT, GL_EMISSION, rouge);
    
    glScaled(longueur,largeur,largeur);
    glTranslatef(1.0,0.0,0.0);
    
    glutWireCube (1.0);
    glPopMatrix();
    
    /* Oy vert */
    glPushMatrix();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, vert);
    glMaterialfv(GL_FRONT, GL_AMBIENT, vert);
    glMaterialfv(GL_FRONT, GL_SPECULAR, vert);
    glMaterialfv(GL_FRONT, GL_EMISSION, vert);
        
    glScaled(largeur,longueur,largeur);
    glTranslatef(0.0,1.0,0.0);
    
    glutWireCube (1.0);  
    glPopMatrix();
    
    /* Oz bleu */
    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, bleu);  
    glMaterialfv(GL_FRONT, GL_AMBIENT, bleu);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bleu);
    glMaterialfv(GL_FRONT, GL_EMISSION, bleu);
    
    glScaled(largeur,largeur,longueur);
    glTranslatef(0.0,0.0,1.0);
    
    glutWireCube (1.0);  
    glPopMatrix();
}

/* charge les textures en mémoire
 */
void loadTextures() {
	glGenTextures(NB_TEXTURES, &texId[0]);
	
    loadJpegImage("bois.jpg"); // chargement

	glBindTexture(GL_TEXTURE_2D, texId[BOIS_ID]); // affectation dans le tableau
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	// construction du mipmap
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, texture); 

    loadJpegImage("eivd.jpg");
	
	glBindTexture(GL_TEXTURE_2D, texId[EIVD_ID]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, texture); 

    loadJpegImage("code.jpg");
	
	glBindTexture(GL_TEXTURE_2D, texId[CODE_ID]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, texture);

    loadJpegImage("jicube.jpg");
	
	glBindTexture(GL_TEXTURE_2D, texId[PHOTO_ID]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, texture);
}

/* fonction d'initialisation
 */
void init(void) {
    
	loadTextures(); // charger les textures
	
    /* couleur du fond */
    glClearColor(noir[0], noir[1], noir[2], 0.0);
    
    glClear(GL_COLOR_BUFFER_BIT);

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_Ka);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_Kd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_Ks);
    
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_Ka);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_Kd);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_Ks);
    
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_Ka);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_Kd);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_Ks);

    glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
    glLightfv(GL_LIGHT3, GL_AMBIENT,  light3_Ka);
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  light3_Kd);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light3_Ks);
    
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.0);
    
    glLightf(GL_LIGHT3,  GL_SPOT_CUTOFF, 45.0);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot3_direction);
    glLightf(GL_LIGHT3,  GL_SPOT_EXPONENT, 2.0);

    /* initialiser la matière de l'objet */
	 new_obj_mat(Gold);

    if(shadeModelSmooth)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);  /* utile sur les petites config */
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

    glEnable(GL_DEPTH_TEST); /* autorise l'activation du tampon de profondeur */
    
    /* pour rendre les normales unitaires après les transformations d'échelle */
    glEnable(GL_NORMALIZE);    

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // pour ajouter du brouillard
    glFogi(GL_FOG_MODE,GL_EXP);
    glFogfv(GL_FOG_COLOR,noir);
    glFogf(GL_FOG_START,1);
    glFogf(GL_FOG_END,15);
    glFogf(GL_FOG_DENSITY,0.15);

    //glEnable(GL_FOG);
}

/*
// pour normaliser un vecteur
float normaliser(float* v)
{
    float length;
    
    length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    
    return length;
}
*/

/* affiche une chaîne de caractères au format bitmap (Helvetica 12)
 * Ã  une position donnée et dans une couleur donnée
 */
void afficherTexte (char *s, float X, float Y, float Z, GLfloat * couleur) {
    unsigned int i;
    
    glColor3f (couleur[0], couleur[1], couleur[2]);
    glRasterPos3f (X, Y, Z);
    for (i = 0; i < strlen(s); i++)
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, s[i]);
}

/* fonction de rappel pour la souris (avec clic)
 */
void souris_clic(int button, int state, int x, int y) {
    xOld = x;
    yOld = y;
}

/* fonction de rappel pour la souris (sans clic)
 */
void souris_sans_clic(int x, int y) {

	 //rotationY += (float)(x-yOld)*0.1;
    rotationX += (float)(x-xOld)*0.1;
    rotationZ += (float)(y-yOld)*0.1;
    
    xOld = x;
    yOld = y;
    
    glutPostRedisplay();
}

/* avancer de 'f' unités en direction du point visé
 */
void avancer(float f) {
    GLfloat matrix[4][4];
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glLoadIdentity();
    glRotatef(roll, 0.0, 0.0, 1.0);
    glRotatef(pitch, 0.0, 1.0, 0.0);
    glRotatef(heading, 1.0, 0.0, 0.0);
    glTranslatef(-X, -Y, -Z);
    
    /* stocke la matrice qui se trouve au sommet de la pile */
    /* elle contient le vecteur de l'observateur au point visé */
    glGetFloatv(GL_MODELVIEW_MATRIX, &matrix[0][0]);
    glPopMatrix();
    
    /* se rapprocher du point visé */
    X -= matrix[0][2]*f;
    Y -= matrix[1][2]*f;
    Z -= matrix[2][2]*f;
}

/* fonction de rappel pour les touches spéciales
 */
void keyboard_special(int special_key, int x, int y) {
    switch (special_key) {    
        case GLUT_KEY_UP: avancer(0.1); break;
        case GLUT_KEY_DOWN: avancer(-0.1); break;
        case GLUT_KEY_RIGHT: pitch++; break;
        case GLUT_KEY_LEFT: pitch--; break;
        default:
            printf ("La touche speciale %d n'est pas active.\n", special_key);
            break;
    }
    glutPostRedisplay();
}

/* affichage de la fenêtre d'information
 */
void infos_display(void) {
    
    /* pour des angles entre -179 et 180 degrés */
    if(roll < -179)
        roll = 360 + roll;
    else if(roll > 180)
        roll = -(360 - roll);
    
    if(pitch < -179)
        pitch = 360 + pitch;
    else if(pitch > 180)
        pitch = -(360 - pitch);
    
    if(heading < -179)
        heading = 360 + heading;
    else if(heading > 180)
        heading = -(360 - heading);
    
    /* contrÃ´le de la cohérence de l'ouverture */
    if (ouverture > 180)
        ouverture = 180;
    else if (ouverture < 0)
        ouverture = 0;
    
    glClearColor(gris03[0], gris03[1], gris03[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* afficher les paramètres courants */
    sprintf (label, "X: %f, heading: %d", X, heading);
    afficherTexte (label,
                   -0.9, 0.6, 0.0, // position
                   rouge);         // couleur
        
    sprintf (label, "Y: %f, pitch: %d", Y, pitch);
    afficherTexte (label,
                   -0.9, 0.1, 0.0, // position
                   vert);          // couleur
    
    sprintf (label, "Z: %f, roll: %d", Z, roll);   
    afficherTexte (label,
                   -0.9, -0.4, 0.0, // position
                   bleu);           // couleur
                
    sprintf (label, "Ouverture : %d degres", ouverture);
    afficherTexte (label,
                   -0.9, -0.9, 0.0, // position
                   gris08);         // couleur
    
    glutSwapBuffers();    
}

/* fonction de rappel pour la gestion du clavier
 */
void keyboard(unsigned char key, int x, int y) {
    
    switch (key) {
        /* pour (dés)activer l'affichages des facettes */
        case ' ': mouvement = !mouvement; break;
        case 'X': X += 0.1; break; /* avancer sur l'axe X */
        case 'x': X -= 0.1; break; /* reculer sur l'axe X */
        case 'Y': Y += 0.1; break; /* avancer sur l'axe Y */
        case 'y': Y -= 0.1; break; /* reculer sur l'axe Y */
        case 'Z': Z += 0.1; break; /* avancer sur l'axe Z */
        case 'z': Z -= 0.1; break; /* reculer sur l'axe Z */
        case 'q': case 'Q': exit(0); break; /* pour quitter */
        case 'O': ouverture++; break;       /* augmente l'ouverture */
        case 'o': ouverture--; break;       /* diminue l'ouverture  */
        case 'H': heading++; break;         /* augmente le cap      */
        case 'h': heading--; break;         /* diminue le cap       */
        case 'P': pitch++; break;           /* augmente le tangage  */
        case 'p': pitch--; break;           /* diminue le tangage   */
        case 'R': roll++; break;            /* augmente le roulis   */
        case 'r': roll--; break;            /* diminue le roulis    */
		  
		  case 'b': /* interrupteur brouillard */
            brouillard = !brouillard;
            brouillard ? glEnable(GL_FOG) : glDisable(GL_FOG);
				break;
		  
        case '0': /* interrupteur lumière 0 */
            light_0_Switch = !light_0_Switch;
            light_0_Switch ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);
            break;
        case '1': /* interrupteur lumière 1 */
            light_1_Switch = !light_1_Switch;
            light_1_Switch ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
            break;
        case '2': /* interrupteur lumière 2 */
            light_2_Switch = !light_2_Switch;
            light_2_Switch ? glEnable(GL_LIGHT2) : glDisable(GL_LIGHT2);
            break;
        case '3': /* interrupteur lumière 3 (spot) */
            light_3_Switch = !light_3_Switch;
            light_3_Switch ? glEnable(GL_LIGHT3) : glDisable(GL_LIGHT3);
            break;
        case 'a': /* affichage des axes */
            affichageAxes = !affichageAxes;
            break;
        case 'f': /* mode plein écran */
            fullscreen = !fullscreen;
            if(fullscreen)
                glutFullScreen();
            else
                glutPositionWindow(0, 0);
            break;
        case 'd': case 'D': /* division de la fenêtre en quatre parties */
            diviserFenetre = !diviserFenetre;
            break;
        case 's': case 'S': /* changement de modèle de lissage */
            shadeModelSmooth = !shadeModelSmooth;
            if(shadeModelSmooth)
                glShadeModel(GL_SMOOTH);
            else
                glShadeModel(GL_FLAT);
            break;
        default: /* signaler les touches pressées inactives */
            printf ("La touche %d n'est pas active.\n", key);
            break;
    }
    glutPostRedisplay();
}

/* fonction de gestion du menu principal
 */
void gestionMenuPrincipal(int value) {
    
    switch (value) {
        case 0: keyboard(' ',0,0); break;  /* indique si l'on veut du mouvement */
        case 1: keyboard('X',0,0); break;  /* avancer sur l'axe X */
        case 2: keyboard('x',0,0); break;  /* reculer sur l'axe X */
        case 3: keyboard('Y',0,0); break;  /* avancer sur l'axe X */
        case 4: keyboard('y',0,0); break;  /* reculer sur l'axe Y */
        case 5: keyboard('Z',0,0); break;  /* avancer sur l'axe X */ 
        case 6: keyboard('z',0,0); break;  /* reculer sur l'axe Z */
        case 7: keyboard('P',0,0); break;  /* tanguer dans le sens positif */     
        case 8: keyboard('p',0,0); break;  /* tanguer dans le sens négatif */
        case 9: keyboard('R',0,0); break;  /* roulis dans le sens positif  */
        case 10: keyboard('r',0,0); break; /* roulis dans le sens négatif  */
        case 11: keyboard('H',0,0); break; /* changer le cap dans le sens positif */
        case 12: keyboard('h',0,0); break; /* changer le cap dans le sens négatif */
        case 13: keyboard('O',0,0); break; /* ouvrir l'angle de vue */
        case 14: keyboard('o',0,0); break; /* fermer l'angle de vue */
        case 15: keyboard_special(GLUT_KEY_UP,0,0); break;   /* se rapprocher */
        case 16: keyboard_special(GLUT_KEY_DOWN,0,0); break; /* s'éloigner */
        case 17: keyboard('a',0,0); break; /* affichage des axes */
        case 18: keyboard('f',0,0); break; /* mode plein écran */
        case 19: keyboard('d',0,0); break; /* diviser la fenêtre */
        case 20: keyboard('s',0,0); break; /* changer de ShadeModel */
        case 21: keyboard('b',0,0); break; /* brouillard */
        case 22: keyboard('q',0,0); break; /* fin du programme */
    }
	 
	glutPostRedisplay();
}

void dessinerChien() {

    if(affichageAxes) {
        axes(3.0, 1.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
		glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
		glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
		glMaterialfv(GL_FRONT, GL_SHININESS, &material_Se);
    }

	glEnable(GL_TEXTURE_2D);

	// pour obtenir une lumière adaptée
	new_material(Special);

	// disposer le chien par rapport aux axes
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glRotatef(60.0, 1.0, 0.0, 0.0);
	glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);

	// dessin de plusieurs cubes texturés
	glBindTexture(GL_TEXTURE_2D, texId[BOIS_ID]);
	glPushMatrix();
	glTranslatef(-1.0,-2.0,-1.0);
	dessinerCube(1.0);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[EIVD_ID]);
	glPushMatrix();
	glTranslatef(2.0,1.0,-2.0);
	dessinerCube(0.7);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[CODE_ID]);
	glPushMatrix();
	glTranslatef(1.0,2.0,1.0);
	dessinerCube(1.0);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[PHOTO_ID]);
	glPushMatrix();
	glTranslatef(1.5,-1.5,0.0);
	dessinerCube(1.0);
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);

	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	/* le nez */
	
	new_material(Black_Plastic);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,truffe_vertex);
	glDrawElements(GL_TRIANGLES,truffe_polygoncount*3,GL_UNSIGNED_INT,truffe_index);

	/* le corps et la tête */

	new_material(Gold);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, obj_mat_Ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, obj_mat_Kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, obj_mat_Ks);
    glMaterialfv(GL_FRONT, GL_EMISSION, obj_mat_Ke);
    glMaterialfv(GL_FRONT, GL_SHININESS, &obj_mat_Se);
	
	glInterleavedArrays(GL_T2F_N3F_V3F,0,head_vertex);
	glDrawElements(GL_TRIANGLES,head_polygoncount*3,GL_UNSIGNED_INT,head_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,body_vertex);
	glDrawElements(GL_TRIANGLES,body_polygoncount*3,GL_UNSIGNED_INT,body_index);

	/* le cou et la queue */

	new_material(Chrome);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,tail_vertex);
	glDrawElements(GL_TRIANGLES,tail_polygoncount*3,GL_UNSIGNED_INT,tail_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,neck_vertex);
	glDrawElements(GL_TRIANGLES,neck_polygoncount*3,GL_UNSIGNED_INT,neck_index);

	/* les oreilles */

	new_material(Bronze);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,tetrahedr_vertex);
	glDrawElements(GL_TRIANGLES,tetrahedr_polygoncount*3,GL_UNSIGNED_INT,tetrahedr_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,tetrahedr__vertex);
	glDrawElements(GL_TRIANGLES,tetrahedr__polygoncount*3,GL_UNSIGNED_INT,tetrahedr__index);

	/* les pattes et le bout de la queue */

	new_material(Rouge);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere4_vertex);
	glDrawElements(GL_TRIANGLES,sphere4_polygoncount*3,GL_UNSIGNED_INT,sphere4_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere4_c5_vertex);
	glDrawElements(GL_TRIANGLES,sphere4_c5_polygoncount*3,GL_UNSIGNED_INT,sphere4_c5_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere4_c6_vertex);
	glDrawElements(GL_TRIANGLES,sphere4_c6_polygoncount*3,GL_UNSIGNED_INT,sphere4_c6_index);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere4_c7_vertex);
	glDrawElements(GL_TRIANGLES,sphere4_c7_polygoncount*3,GL_UNSIGNED_INT,sphere4_c7_index);
	
	glInterleavedArrays(GL_T2F_N3F_V3F,0,bout_queue_vertex);
	glDrawElements(GL_TRIANGLES,bout_queue_polygoncount*3,GL_UNSIGNED_INT,bout_queue_index);
	
	/* l'os */
	
	new_material(Os);

	glInterleavedArrays(GL_T2F_N3F_V3F,0,cylinder14_vertex);
	glDrawElements(GL_TRIANGLES,cylinder14_polygoncount*3,GL_UNSIGNED_INT,cylinder14_index);	

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere15_8_vertex);
	glDrawElements(GL_TRIANGLES,sphere15_8_polygoncount*3,GL_UNSIGNED_INT,sphere15_8_index);	
	
	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere15_7_vertex);
	glDrawElements(GL_TRIANGLES,sphere15_7_polygoncount*3,GL_UNSIGNED_INT,sphere15_7_index);	

	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere15_6_vertex);
	glDrawElements(GL_TRIANGLES,sphere15_6_polygoncount*3,GL_UNSIGNED_INT,sphere15_6_index);	
	
	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere15_vertex);
	glDrawElements(GL_TRIANGLES,sphere15_polygoncount*3,GL_UNSIGNED_INT,sphere15_index);

	// le scaphandre
	
	new_material(Verre);
	
	glInterleavedArrays(GL_T2F_N3F_V3F,0,sphere13_vertex);
	glDrawElements(GL_TRIANGLES,sphere13_polygoncount*3,GL_UNSIGNED_INT,sphere13_index);	
}

/* dessine un cube de côté 
 */
void dessinerCube(float c) {

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0,0.0);   glVertex3f( 0.0, c  , c);
  glTexCoord2f(0.0,1.0);   glVertex3f( 0.0, 0.0, c);
  glTexCoord2f(1.0,1.0);   glVertex3f( c  , 0.0, c);
  glTexCoord2f(1.0,0.0);   glVertex3f( c  , c  , c);
  glEnd();

  glBegin(GL_POLYGON);  
  glTexCoord2f(0.0,0.0);   glVertex3f( c, c  , c);
  glTexCoord2f(0.0,1.0);   glVertex3f( c, 0.0, c);
  glTexCoord2f(1.0,1.0);   glVertex3f( c, 0.0, 0.0);
  glTexCoord2f(1.0,0.0);   glVertex3f( c, c  , 0.0);
  glEnd();

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0,0.0);   glVertex3f( c  , c  , 0.0);
  glTexCoord2f(0.0,1.0);   glVertex3f( c  , 0.0, 0.0);
  glTexCoord2f(1.0,1.0);   glVertex3f( 0.0, 0.0, 0.0);
  glTexCoord2f(1.0,0.0);   glVertex3f( 0.0, c  , 0.0);
  glEnd();

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0,0.0);   glVertex3f( 0.0, c  , 0.0);
  glTexCoord2f(0.0,1.0);   glVertex3f( 0.0, 0.0, 0.0);
  glTexCoord2f(1.0,1.0);   glVertex3f( 0.0, 0.0, c  );
  glTexCoord2f(1.0,0.0);   glVertex3f( 0.0, c  , c  );
  glEnd();

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0,0.0);   glVertex3f( 0.0, c  , 0.0);
  glTexCoord2f(0.0,1.0);   glVertex3f( 0.0, c  , c  );
  glTexCoord2f(1.0,1.0);   glVertex3f( c  , c  , c  );
  glTexCoord2f(1.0,0.0);   glVertex3f( c  , c  , 0.0);
  glEnd();
  
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0,0.0);   glVertex3f( 0.0, 0.0, 0.0);
  glTexCoord2f(0.0,1.0);   glVertex3f( 0.0, 0.0, c  );
  glTexCoord2f(1.0,1.0);   glVertex3f( c  , 0.0, c  );
  glTexCoord2f(1.0,0.0);   glVertex3f( c  , 0.0, 0.0);
  glEnd();
  
}

/* dessine le viewport nord-ouest
 */
void NO(void) {
    
    /*  +---+---+
     *  | X |   |  Projection perspective, en haut à gauche,
     *  +---+---+  ou en plein écran !
     *  |   |   |
     *  +---+---+
     */
    
    /* spécifie la clÃ´ture */
    if(diviserFenetre) {
        glViewport (0,     /* origine x */
        fenetreHauteur/2,  /* origine y */
        fenetreLargeur/2,  /* largeur   */
        fenetreHauteur/2); /* hauteur   */
    } else {
        glViewport (0, /* origine x */
        0,             /* origine y */
        fenetreLargeur,  /* largeur   */
        fenetreHauteur); /* hauteur   */
    }
    
    /* indique que la matrice courante est la matrice de projection */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    /* Projection perspective */
    gluPerspective(ouverture,  /* ouverture dans la direction de y */
                   rapport,    /* rapport                          */
                   1.0,        /* distance au plan de devant       */
                   100.0);     /* distance au plan de derrière     */
    
    /* indique que la matrice courante est la matrice de modelview */
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    
    glLoadIdentity();
    
    glRotatef(roll, 0.0, 0.0, 1.0);
    glRotatef(pitch, 0.0, 1.0, 0.0);
    glRotatef(heading, 1.0, 0.0, 0.0);
    glTranslatef(-X, -Y, -Z);
    
    // les sources de lumière sont aussi transformées !
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
    
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot3_direction);

	dessinerChien();
	
    glPopMatrix();
}

/* dessine le viewport nord-est
 */
void NE(void) {
    
    /*  +---+---+
     *  |   | X |  Projection orthogonale sur Oyz, en haut à droite
     *  +---+---+ 
     *  |   |   |
     *  +---+---+
     */
    
    /* spécifie la clôture */
    glViewport (fenetreLargeur/2,  /* origine x */
    fenetreHauteur/2,  /* origine y */
    fenetreLargeur/2,  /* largeur   */
    fenetreHauteur/2); /* hauteur   */
    
    /* indique que la matrice courante est la matrice de projection */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    
    /* Projection orthogonale */
    glOrtho(-2.0,       /* x min */
        2.0,            /* x max */
        -2.0 / rapport, /* y min */
        2.0 / rapport,  /* y max */
        0.0,            /* distance plan avant   */
        10.0);          /* distance plan arrière */
    
    /* indique que la matrice courante est la matrice de modelview */
    glMatrixMode (GL_MODELVIEW);
    
    glPushMatrix();
    
    glLoadIdentity ();   

    /* position de observateur */
    gluLookAt (5.0, 0.0, 0.0,  /* depuis Ox       */
               0.0, 0.0, 0.0,  /* vers l'origine  */
               0.0, 0.0, 1.0); /* Oz vers le haut */
    
    /* les sources de lumière sont aussi transformées ! */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
    
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot3_direction);
    
	 dessinerChien();
    
    glPopMatrix();
}

/* dessine le viewport sud-ouest */
void SO(void) {
    
    /*  +---+---+
     *  |   |   |
     *  +---+---+ 
     *  | X |   |  Projection orthogonale sur Oxz, en bas à gauche
     *  +---+---+
     */
    
    /* spécifie la clÃ´ture */
    glViewport (0,                 /* origine x */
                0,                 /* origine y */
                fenetreLargeur/2,  /* largeur   */
                fenetreHauteur/2); /* hauteur   */
    
    /* indique que la matrice courante est la matrice de projection */
    glMatrixMode (GL_PROJECTION);
    
    glPushMatrix();
    
    glLoadIdentity ();
    
    /* Projection orthogonale */
    glOrtho(-2.0,       /* x min */
        2.0,            /* x max */
        -2.0 / rapport, /* y min */
        2.0 / rapport,  /* y max */
        0.0,            /* distance plan avant   */
        10.0);          /* distance plan arrière */
        
    /* indique que la matrice courante est la matrice de modelview */
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();   
    
    /* position de observateur */
    gluLookAt (0.0, 5.0, 0.0,  /* depuis Oy       */
               0.0, 0.0, 0.0,  /* vers origine    */
               0.0, 0.0, 1.0); /* Oz vers le haut */
    
    /* les sources de lumière sont aussi transformées ! */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
    
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot3_direction);
    
    //dessinerReseauArches();
	 dessinerChien();
    
    glPopMatrix();
}

/* dessine le viewport sud-est */
void SE(void) {
    /*  +---+---+
     *  |   |   |
     *  +---+---+ 
     *  |   | X |  Projection orthogonale sur Oxy, en bas Ã  droite
     *  +---+---+
     */
    
    /* spécifie la clÃ´ture */
    glViewport (fenetreLargeur/2,  /* origine x */
                0,                 /* origine y */
                fenetreLargeur/2,  /* largeur   */
                fenetreHauteur/2); /* hauteur   */
    
    /* indique que la matrice courante est la matrice de projection */
    glMatrixMode (GL_PROJECTION);
    
    glPushMatrix();
    
    glLoadIdentity ();
    
    /* Projection orthogonale */
    glOrtho(-2.0,           /* x min */
            2.0,            /* x max */
            -2.0 / rapport, /* y min */
            2.0 / rapport,  /* y max */
            0.0,            /* distance plan avant   */
            10.0);          /* distance plan arrière */
    
    /* indique que la matrice courante est la matrice de modelview */
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();   
    
    /* position de observateur */
    gluLookAt (0.0, 0.0, 5.0,  /* depuis Oz      */
               0.0, 0.0, 0.0,  /* vers origine   */
              -1.0, 0.0, 0.0); /* Ox vers le bas */
    
    /* les sources de lumière sont aussi transforméesÂ ! */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT3, GL_POSITION, light3_pos);
    
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot3_direction);
    
    //dessinerReseauArches();
	 dessinerChien();
    
    glPopMatrix();
}

/* fonction d'affichage
 */
void display(void) {
    
    if (rotationX > 360)
        rotationX = rotationX - 360;
    if (rotationY > 360)
        rotationY = rotationY - 360;
    if (rotationZ > 360)
        rotationZ = rotationZ - 360;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!diviserFenetre) {
        NO();
    } else {
        NO();
        NE();
        SO();
        SE();
    }
    
    /* réafficher les infos */
    glutSetWindow(infos);
    glutPostRedisplay();
    
    /* revenir Ã  la fenêtre principale */
    glutSetWindow(principale);
    glutPostRedisplay();
    
    glFlush();
    
    glutSwapBuffers(); /* échanger les tampons */ 
    
}

/* redimensionnement de la fenêtre */
void reshape (int w, int h) {
    fenetreLargeur = w;
    fenetreHauteur = h;
    
    /* pour pouvoir conserver les proportions quand la fenêtre est redimensionnée */
    rapport = (float)w / (float)h;    
}

/* fonction de rappel en cas d'inaction,
 * effectue éventuellement une lente rotation
 */
void idle(void) {
    if(mouvement) {
		  rotationX += 0.1;        
        rotationZ += 0.1;
        //rotationY += 0.1;
		  
        glutPostRedisplay();
    }
}

/* fonction permettant de choisir la matière utilisée
 */
void gestionMenuMatiere(int value) {
    switch (value) {
        case 0: new_obj_mat(RevetementSpatial); break;
        case 1: new_obj_mat(Brass); break;
        case 2: new_obj_mat(Bronze); break;
        case 3: new_obj_mat(Polished_Bronze); break;
        case 4: new_obj_mat(Chrome); break;
        case 5: new_obj_mat(Copper); break;
        case 6: new_obj_mat(Polished_Copper); break;
        case 7: new_obj_mat(Gold); break;
        case 8: new_obj_mat(Polished_Gold); break;
        case 9: new_obj_mat(Pewter); break;
        case 10: new_obj_mat(Silver); break;
        case 11: new_obj_mat(Polished_Silver); break;
        case 12: new_obj_mat(Emerald); break;
        case 13: new_obj_mat(Jade); break;
        case 14: new_obj_mat(Obsidian); break;
        case 15: new_obj_mat(Pearl); break;
        case 16: new_obj_mat(Ruby); break;
        case 17: new_obj_mat(Turquoise); break;
        case 18: new_obj_mat(Black_Plastic); break;
        case 19: new_obj_mat(Black_Rubber); break;
        /**/
        case 20: new_obj_mat(Plastic1); break;
        case 21: new_obj_mat(Plastic2); break;
        case 22: new_obj_mat(Plastic3); break;
        case 23: new_obj_mat(Plastic4); break;
        case 24: new_obj_mat(Plastic5); break;
        case 25: new_obj_mat(Plastic6); break;
        /**/
        case 26: new_obj_mat(Rubber1); break;
        case 27: new_obj_mat(Rubber2); break;
        case 28: new_obj_mat(Rubber3); break;
        case 29: new_obj_mat(Rubber4); break;
        case 30: new_obj_mat(Rubber5); break;
        case 31: new_obj_mat(Rubber6); break;
    }
    
    glutPostRedisplay();
}

/* fonction permettant d'allumer et d'éteindre des sources de lumière
 */
void gestionMenuLumiere(int value) {
    switch (value) {
        case 0:
            light_0_Switch = !light_0_Switch;
            light_0_Switch ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);
            break;
        case 1:
            light_1_Switch = !light_1_Switch;
            light_1_Switch ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
            break;
        case 2:
            light_2_Switch = !light_2_Switch;
            light_2_Switch ? glEnable(GL_LIGHT2) : glDisable(GL_LIGHT2);
            break;
        case 3:
            light_3_Switch = !light_3_Switch;
            light_3_Switch ? glEnable(GL_LIGHT3) : glDisable(GL_LIGHT3);
            break;
    }
    
    glutPostRedisplay();
}

/* fonction principale
 */
int main(int argc, char** argv) {
    glutInit(&argc, argv); /* initialisation de la GLUT          */
    glutInitDisplayMode    /* initialisation du mode d'affichage */
        (GLUT_DOUBLE |     /* tampon double */
         GLUT_RGB    |     /* couleur RGB   */
         GLUT_DEPTH);      /* zbuffer       */
    
    glutInitWindowSize (1100, 700);        /* taille de la fenêtre                          */
    glutInitWindowPosition (30, 30);       /* position de la fenêtre                        */
	
    principale = glutCreateWindow ("Chien de l'espace"); /* titre de la fenêtre                           */
    init();                                /* initialisation du contenu de la fenêtre       */
    glutDisplayFunc(display);              /* fonction de rappel pour l'affichage           */
    glutMouseFunc(souris_clic);            /* fonction de rappel pour la souris (avec clic) */
    glutMotionFunc(souris_sans_clic);      /* fonction de rappel pour la souris (sans clic) */
    glutReshapeFunc(reshape);              /* fonction de rappel pour le redimensionnement  */
    glutKeyboardFunc(keyboard);            /* fonction de rappel pour la gestion du clavier */
    glutSpecialFunc(keyboard_special);     /* fonction de rappel pour les touches spéciales */
    glutIdleFunc(idle);
                                   
    /* sous-menu Matiere */
    menuMatiere = glutCreateMenu(gestionMenuMatiere);
    glutAddMenuEntry("RevetementSpatial", 0);
    glutAddMenuEntry("Brass", 1);
    glutAddMenuEntry("Bronze", 2);
    glutAddMenuEntry("Polished_Bronze", 3);
    glutAddMenuEntry("Chrome", 4);
    glutAddMenuEntry("Copper", 5);
    glutAddMenuEntry("Polished_Copper", 6);
    glutAddMenuEntry("* Gold (defaut)", 7);
    glutAddMenuEntry("Polished_Gold", 8);
    glutAddMenuEntry("Pewter", 9);
    glutAddMenuEntry("Silver", 10);
    glutAddMenuEntry("Polished_Silver", 11);
    glutAddMenuEntry("Emerald", 12);
    glutAddMenuEntry("Jade", 13);
    glutAddMenuEntry("Obsidian", 14);
    glutAddMenuEntry("Pearl", 15);
    glutAddMenuEntry("Ruby", 16);
    glutAddMenuEntry("Turquoise", 17);
    glutAddMenuEntry("Black_Plastic", 18);
    glutAddMenuEntry("Black_Rubber", 19);
    glutAddMenuEntry("Plastic1", 20);
    glutAddMenuEntry("Plastic2", 21);
    glutAddMenuEntry("Plastic3", 22);
    glutAddMenuEntry("Plastic4", 23);
    glutAddMenuEntry("Plastic5", 24);
    glutAddMenuEntry("Plastic6", 25);
    glutAddMenuEntry("Rubber1", 26);
    glutAddMenuEntry("Rubber2", 27);
    glutAddMenuEntry("Rubber3", 28);
    glutAddMenuEntry("Rubber4", 29);
    glutAddMenuEntry("Rubber5", 30);
    glutAddMenuEntry("Rubber6", 31);                                   
                                   
    /* sous-menu Lumiere */
    menuLumiere = glutCreateMenu(gestionMenuLumiere);
    glutAddMenuEntry("Lumiere blanche \t [0]", 0);
    glutAddMenuEntry("Lumiere bleue   \t [1]", 1);
    glutAddMenuEntry("Lumiere rouge   \t [2]", 2);
    glutAddMenuEntry("Spot vert       \t [3]", 3);
                                   
    /* menu principal */
    menuPrincipal = glutCreateMenu(gestionMenuPrincipal);
    glutAddSubMenu("Matiere", menuMatiere);
    glutAddSubMenu("Lumiere", menuLumiere);
    glutAddMenuEntry("Mouvement rotatif \t [ESPACE]", 0);
    glutAddMenuEntry("Avancer sur X \t [X]", 1);
    glutAddMenuEntry("Reculer sur X \t [x]", 2);
    glutAddMenuEntry("Avancer sur Y \t [Y]", 3);
    glutAddMenuEntry("Reculer sur Y \t [y]", 4);
    glutAddMenuEntry("Avancer sur Z \t [Z]", 5);
    glutAddMenuEntry("Reculer sur Z \t [z]", 6);
    glutAddMenuEntry("Pitch   + \t [P, RIGHT]",      7);
    glutAddMenuEntry("Pitch   - \t [p, LEFT]",       8);
    glutAddMenuEntry("Roll    + \t [R]",    9);
    glutAddMenuEntry("Roll    - \t [r]", 10);
    glutAddMenuEntry("Heading       + \t [C]",        11);
    glutAddMenuEntry("Heading       - \t [c]",      12);
    glutAddMenuEntry("Ouverture + \t [O]", 13);   
    glutAddMenuEntry("Ouverture - \t [o]", 14);
    glutAddMenuEntry("Avancer \t [UP]", 15);   
    glutAddMenuEntry("Reculer \t [DOWN]", 16);
    glutAddMenuEntry("Affichage des axes \t [A]", 17);
    glutAddMenuEntry("Mode plein ecran \t [F]", 18);
    glutAddMenuEntry("Diviser la fenetre \t [D]", 19);
    glutAddMenuEntry("ShadeModel \t [S]", 20);
    glutAddMenuEntry("Brouillard \t [b]", 21);
    glutAddMenuEntry("Quitter \t [Q]", 22);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
                                                          
    infos = glutCreateSubWindow(principale, 0, 0, 160, 60);
	
    glutDisplayFunc(infos_display);

    glutMainLoop(); /* gestion des événements */
                                   
    return 0; /* pour respecter la norme ANSI */
}
