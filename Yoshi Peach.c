

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define PASSO        5
#define NUM_TEXTURAS 10

struct tipo_camera {
    GLfloat posx;               // posição x da camera
    GLfloat posy;               // posição y da camera
    GLfloat posz;               // posição z da camera
    GLfloat alvox;              // alvo x da visualização
    GLfloat alvoy;              // alvo y da visualização
    GLfloat alvoz;              // alvo z da visualização
    GLfloat inicio;             // início da área de visualização em profundidade
    GLfloat fim;                // fim da área de visualização em profundidade
    GLfloat ang;                // abertura da 'lente' - efeito de zoom
};

struct tipo_luz {
    GLfloat posicao[ 4 ];
    GLfloat ambiente[ 4 ];
    GLfloat difusa[ 4  ];
    GLfloat especular[ 4 ];
    GLfloat especularidade[ 4 ];
};

/* Camera vai conter as definições da camera sintética */
struct tipo_camera camera;

/* Luz vai conter as informações da iluminação */
struct tipo_luz luz;

GLfloat aspecto;
GLint rotacao_y = 0 , rotacao = 0, rotacao_l = 0, rotacao_yoshi = 0;
GLint rotacao_yoshicx = 0 , rotacao_yoshicy = 0, rotacao_lbrd = 0,rotacao_lbre = 0;
GLint rotacao_lcy = 0, rotacao_lcx = 0, rotacao_yoshibrd = 0 , rotacao_yoshibre = 0, rotacao_cogumelo = 0;
GLint aa=0,bb=0,cc=0;
GLuint  texture_id[ NUM_TEXTURAS ];  /* Identificador da textura */

GLfloat rotX_ini, rotY_ini;
GLfloat obsX_ini, obsY_ini, obsZ_ini;


GLint x_ini,y_ini,bot;

GLfloat angle, fAspect;
GLfloat rotaciona_x, rotY;
GLfloat obsX, obsY, obsZ;


char *texture_name[ NUM_TEXTURAS ] = {
    "azulejo.bmp", "fundo.bmp", "face.bmp",
    "texturayoshi.bmp", "cogumelo.bmp",
    "peach.bmp", "loiro.bmp", "caixa.bmp"
};

// Funão que carrega uma texuta no formato BMP 
int  LoadBMP(char *filename, bool mipmap){
    #define SAIR        {fclose(fp_arquivo); return -1;}
    #define CTOI(C)     (*(int*)&C)

    GLubyte     *image;
    GLubyte     Header[0x54];
    GLuint      DataPos, imageSize;
    GLsizei     Width,Height;

    int nb = 0;


    // Abre o arquivo e efetua a leitura do Header do arquivo BMP
    FILE * fp_arquivo = fopen(filename,"rb");
    if (!fp_arquivo)
        return -1;
    if (fread(Header,1,0x36,fp_arquivo)!=0x36)
        SAIR;
    if (Header[0]!='B' || Header[1]!='M')
        SAIR;
    if (CTOI(Header[0x1E])!=0)
        SAIR;
    if (CTOI(Header[0x1C])!=24)
        SAIR;

    // Recupera a informação dos atributos de
    // altura e largura da imagem

    Width   = CTOI(Header[0x12]);
    Height  = CTOI(Header[0x16]);
    ( CTOI(Header[0x0A]) == 0 ) ? ( DataPos=0x36 ) : ( DataPos = CTOI(Header[0x0A]) );

    imageSize=Width*Height*3;

    // Efetura a Carga da Imagem
    image = (GLubyte *) malloc ( imageSize );
    int retorno;
    retorno = fread(image,1,imageSize,fp_arquivo);

    if (retorno !=imageSize)
     {
        free (image);
        SAIR;
     }

    // Inverte os valores de R e B
    int t, i;

    for ( i = 0; i < imageSize; i += 3 )
     {
        t = image[i];
        image[i] = image[i+2];
        image[i+2] = t;
     }

    
    if(mipmap){
		// Cria mipmaps para obter maior qualidade
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, Width, Height,	GL_RGB, 
						  GL_UNSIGNED_BYTE, image);
		// Ajusta os filtros iniciais para a textura
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	
	else{
		// Tratamento da textura para o OpenGL

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S    ,GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T    ,GL_REPEAT);
	
	    glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
	    // Faz a geraçao da textura na memória
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
    
	
	
	
	fclose (fp_arquivo);
    free (image);
    return 1;
}

// Funções desenha personagens da cena
void DesenhaCaixas(); 
void DesenhaYoshi(GLUquadricObj *quadObj);
void DesenhaPeach(GLUquadricObj *quadObj);
void DesenhaCogumelo(GLUquadricObj *quadObj);


/* Faz o carregamento */
void Texturizacao( void ) {
    int  i;

    glEnable(GL_TEXTURE_2D);
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures ( 10, texture_id );

    for(i=0; i < NUM_TEXTURAS; i++) {
        glBindTexture ( GL_TEXTURE_2D, texture_id[ i ] );
        LoadBMP ( texture_name[ i ], false );
    }

		// Ajusta os filtros
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP);
	

}

/* Funcão que define a iluminação da cena */
void Define_Iluminacao( void ) {
    /* Modelo de preenchimento dos objetos */
    glShadeModel( GL_SMOOTH );

    /* Habilita iluminação */
    glEnable( GL_LIGHTING );

    /* Ativa o uso da luz ambiente */
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , luz.ambiente );

    /* Habilita e configura a luz 0 (podem ter 8. de 0 a 7)
    posição da luz no universo */
    glLightfv( GL_LIGHT0 , GL_POSITION , luz.posicao );

    /* Configura a luz ambiente */
    glLightfv( GL_LIGHT0 , GL_AMBIENT  , luz.ambiente  );

    /* Configura a luz difusa */
    glLightfv( GL_LIGHT0 , GL_DIFFUSE  , luz.difusa );

    /* Configura a luz especular */
    glLightfv( GL_LIGHT0 , GL_SPECULAR , luz.especular );

    /* Habilita a luz 0 */
    glEnable ( GL_LIGHT0 );

    /* Habilita a cor do material no cálculo das cores do objeto */
    glEnable( GL_COLOR_MATERIAL );

    /* Define a refletância do material */
    glMaterialfv( GL_FRONT , GL_SPECULAR  , luz.especularidade );

    /* Define o brilho do material */
    glMateriali ( GL_FRONT , GL_SHININESS , 10 );
}

/* Função callback chamada para fazer o desenho */
void Desenha(void) {

    /* Limpa a janela de visualização com a cor de fundo especificada */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Define_Iluminacao();
	
	glRotatef( rotacao_y , 0 , 1 , 0 );
   
    /* Desenha a luz */
    glPushMatrix();
    	glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
    	glTranslatef( luz.posicao[ 0 ] , luz.posicao[ 1 ] , luz.posicao[ 2 ]  );
    	glutSolidSphere( 5 , 10 , 10 );
    glPopMatrix();

    Texturizacao();
	
    /* Chão */
    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, texture_id[0] );
    
    glBegin( GL_QUADS );
	    glNormal3f( 0.0, -1.0, 0.0 );
		glTexCoord2f( 10.0, 0.0 );		glVertex3f(  1000, -50, 500 );
	    glTexCoord2f( 10.0, 10.0 );		glVertex3f( 1000, -50, -2000 );
	    glTexCoord2f( 0.0, 10.0 );	    glVertex3f( -1000, -50, -2000 );
	    glTexCoord2f( 0.0, 0.0 );	    glVertex3f( -1000, -50, 500 );
    glEnd();

	glDisable(GL_TEXTURE_2D);
	
    /* Fundo Mario Bros */
    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, texture_id[1] );
	
    glBegin( GL_QUADS );
	    glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f( 1.0 , 0.0 );	    glVertex3i(  1000 , -100 ,   -2000 );
	    glTexCoord2f( 1.0 , 1.0 );	    glVertex3i(  1000 , 1100 , -2000 );
	    glTexCoord2f( 0.0 , 1.0 );	    glVertex3i( -1000 , 1100 , -2000 );
	    glTexCoord2f( 0.0 , 0.0 );	    glVertex3i( -1000 , -100 ,   -2000 );
    glEnd();

    /* Frente */
    glBegin( GL_QUADS );
	    glNormal3f(   0.0 ,   0.0 ,  -1.0 );
		glTexCoord2f( 0.0 , 0.0 );		glVertex3f(  300 , -100 ,   500 );
	    glTexCoord2f( 1.0 , 0.0 );	    glVertex3f(  -300 , -100 , 500 );
	    glTexCoord2f( 1.0 , 1.0 );	    glVertex3f( -300 , 1100 , 500 );
	    glTexCoord2f( 0.0 , 1.0 );	    glVertex3f( 300 , 1100 , 500 );
    glEnd();

    /* Lado Direito */
    glBegin( GL_QUADS );
    	glNormal3f(   -1.0 ,   0.0 ,  0.0 );
		glTexCoord2f( 1.0 , 0.0 );    	glVertex3f(  300 , -100 ,  500 );
	    glTexCoord2f( 1.0 , 1.0 );    	glVertex3f(  300 , 1100 , 500 );
	    glTexCoord2f( 0.0 , 1.0 );    	glVertex3f( 1000 , 1100 , -2000 );
	    glTexCoord2f( 0.0 , 0.0 );    	glVertex3f( 1000 , -100 ,   -2000 );
    glEnd();


    /* Lado Esquerdo */
    glBegin( GL_QUADS );
	    glNormal3f(   1.0 ,   0.0 ,  0.0 );
		glTexCoord2f( 1.0 , 0.0 );		glVertex3f(  -1000 , -100 ,  -2000 );
	    glTexCoord2f( 1.0 , 1.0 );	    glVertex3f(  -1000 , 1100 , -2000 );
	    glTexCoord2f( 0.0 , 1.0 );	    glVertex3f( -300 , 1100 , 500 );
	    glTexCoord2f( 0.0 , 0.0 );	    glVertex3f( -300 , -100 ,   500 );
    glEnd();

	
	glDisable(GL_TEXTURE_2D);
    
	GLUquadricObj *quadObj; // um objeto é criado
    quadObj = gluNewQuadric();
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    
    //DesenhaCaixas(); 
	DesenhaYoshi(quadObj);
    DesenhaCogumelo(quadObj);
    DesenhaPeach(quadObj);
	
	glPushMatrix();
		glTranslatef(-500,-20,-400);
		glRotatef(30,0,1,0);
		DesenhaCaixas(); 
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(400,-20,-400);
		glRotatef(-30,0,1,0);
		DesenhaCaixas(); 
	glPopMatrix();
    /* Executa os comandos OpenGL */

    glutSwapBuffers();

}

/* Inicializa parâmetros de rendering */
void Inicializa (void) {
    /* Cor de fundo da janela (RGBA) */
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    /* Posição x da câmera no universo */
    camera.posx   = 0;

    /* Posição y da câmera no universo */
    camera.posy   = 60;

    /* Posição z da câmera no universo */
    camera.posz   = 400;

    /* Posição x do alvo da câmera no universo - para onde a cÂmera 'olha' */
    camera.alvox  = 0;

    /* Posição y do alvo da câmera no universo - para onde a cÂmera 'olha' */
    camera.alvoy  = 0;

    /* Posição z do alvo da câmera no universo - para onde a cÂmera 'olha' */
    camera.alvoz  = 0;

    /* A partir de que distância da câmera ela começa a 'enxergar' os objetos */
    camera.inicio = 0.1;

    /* Até que distância da câmera ela consegue 'enxergar' os objetos */
    camera.fim    = 5000.0;

    /* Ângulo da câmera - define o zoom */
    camera.ang    = 65;

    /* Posição da fonte de luz */
    luz.posicao[ 0 ] = 100.0;
    luz.posicao[ 1 ] = 100.0;
    luz.posicao[ 2 ] = 100.0;
    luz.posicao[ 3 ] = 1.0;

    /* Cor e intensidade da luz ambiente */
    luz.ambiente[ 0 ] = 0.2;
    luz.ambiente[ 1 ] = 0.2;
    luz.ambiente[ 2 ] = 0.2;
    luz.ambiente[ 3 ] = 1.0;

    /* Cor e intensidade da luz difusa */
    luz.difusa[ 0 ] = 0.5;
    luz.difusa[ 1 ] = 0.5;
    luz.difusa[ 2 ] = 0.5;
    luz.difusa[ 3 ] = 1.0;

    /* Cor e intensidade da luz especular */
    luz.especular[ 0 ] = 0.9;
    luz.especular[ 1 ] = 0.9;
    luz.especular[ 2 ] = 0.9;
    luz.especular[ 3 ] = 1.0;

    /* Cor e intensidade da especularidade */
    luz.especularidade[ 0 ] = 0.9;
    luz.especularidade[ 1 ] = 0.9;
    luz.especularidade[ 2 ] = 0.9;
    luz.especularidade[ 3 ] = 1.0;

    /* Rotacao do objeto e da camera */
    rotacao_y = rotacao = rotacao_yoshi = rotacao_yoshicx = rotacao_yoshicy = 0;
    rotacao_l =  rotacao_lbrd = rotacao_lbre = rotacao_lcx = 0;
    rotacao_lcy = 0;

    /* Ativa a possibilidade de transparência dos objetos - canal alfa */
    glEnable( GL_BLEND );

    /* Define a forma de cálculo da transparência */
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    /* Ativa a remoção das faces ocultas */
    glEnable( GL_CULL_FACE );

    /* Ativa o cálculo de profundidade z-buffer */
    glEnable( GL_DEPTH_TEST );
    
}

/* Função usada para especificar o volume de visualização */
void EspecificaParametrosVisualizacao( void ) {
    // seleciona o tipo de matriz para a projeção
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a projeção perspectiva
    gluPerspective( camera.ang , aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // rotaciona a camera
    glRotatef( rotacao , 0 , 1 , 0 );

    // Especifica posição da camera (o observador) e do alvo
    gluLookAt( camera.posx , camera.posy , camera.posz ,
               camera.alvox , camera.alvoy , camera.alvoz ,
               0 , 1 , 0 );

    // Posiciona e orienta o observador
    glTranslatef(-obsX,-obsY,-obsZ);
    glRotatef(rotaciona_x,1,0,0);
    glRotatef(rotY,0,1,0);
}

/* Função callback chamada quando o tamanho da janela é alterado */
void AlteraTamanhoJanela( GLsizei largura , GLsizei altura ) {

    // Para previnir uma divisão por zero
    if ( altura == 0 ) altura = 1;

    // Especifica as dimensões da viewport
    glViewport( 0 , 0 , largura , altura );

    // Calcula a correção de aspecto
    aspecto = ( GLfloat )largura / ( GLfloat )altura;
    //printf("\nAspecto (relação entre a altura e a largura) atual : %f", aspecto );
    EspecificaParametrosVisualizacao();
}

/* Função callback chamada para gerenciar eventos do mouse */
void GerenciaMouse( GLint button , GLint state , GLint x , GLint y ) {
    // botão esquerdo do mouse
    if ( button == GLUT_LEFT_BUTTON )
        if ( state == GLUT_DOWN ) {
            // Salva os parâmetros atuais
            x_ini = x;
            y_ini = y;
            obsX_ini = obsX;
            obsY_ini = obsY;
            obsZ_ini = obsZ;
            rotX_ini = rotaciona_x;
            rotY_ini = rotY;
            bot = button;
        }
    // botão direito do mouse
    if ( button == GLUT_RIGHT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-out
            if ( camera.ang <= 174 ) camera.ang += 5;

    EspecificaParametrosVisualizacao();

    // obriga redesenhar
    glutPostRedisplay();
}

void GerenciaMovim(int x, int y) {
    // variaveis de calibração mouse.
    GLfloat cal_sens_rota = 10.0;
    GLfloat	cal_sens_obs = 20.0;
    GLfloat	cal_sens_trans = 40.0;


    // Botão esquerdo ?
    if(bot==GLUT_LEFT_BUTTON) {
        // Calcula diferenças
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        // E modifica ângulos
        rotY = rotY_ini - deltax/cal_sens_rota;
        rotaciona_x = rotX_ini - deltay/cal_sens_rota;
    }
    // Botão direito ?

    // Botão do meio ?
    if(bot==GLUT_MIDDLE_BUTTON) {
        // Calcula diferenças
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        // E modifica posições
        obsX = obsX_ini + deltax/cal_sens_trans;
        obsY = obsY_ini - deltay/cal_sens_trans;
    }
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}

/* Tratamento do teclado */
void GerenciaTeclado( GLubyte key , GLint x , GLint y ) {
    if ( key == ',' )
        aa=1;
    if ( key == '.' )
        bb=1;
    if ( key == ';' )
        cc=1;
    if ( key == '<' )
        aa=0;
    if ( key == '>' )
        bb=0;
    if ( key == ':' )
        cc=0;



    /* ESC : sai do programa */
    if ( key == 27 )
        exit( 0 );

    /* Rotaciona o objeto */
    if( key == 'r' )
        rotacao_y -= PASSO;

    if( key == 'R' )
        rotacao_y += PASSO;

    /* Rotaciona a camera */
    if ( key == 'G' )
        rotacao += PASSO;

    if ( key == 'g' )
        rotacao -= PASSO;

    /* ROTAÇÕES DA PEACH */
    /* Rotação geral */
    if ( key == '1' )
        rotacao_l += PASSO;

    if ( key == '!' )
        rotacao_l -= PASSO;

    /* Rotação braço direito */
    if ( key == '2' )
        rotacao_lbrd += PASSO;

    if ( key == '@' )
        rotacao_lbrd -= PASSO;

    /* Rotação braço esquerdo */
    if ( key == '3' )
        rotacao_lbre += PASSO;

    if ( key == '#' )
        rotacao_lbre -= PASSO;

    /* Rotação cabeça em x */
    if ( key == '4' )
        rotacao_lcx += PASSO;

    if ( key == '$' )
        rotacao_lcx -= PASSO;

    /* Rotação cabeça em y */
    if ( key == '5' )
        rotacao_lcy += PASSO;

    if ( key == '%' )
        rotacao_lcy -= PASSO;


    /* ROTAÇÕES DO YOSHI */
    /* Rotação geral */
    if ( key == '-' )
        rotacao_yoshi += PASSO;

    if ( key == '_' )
        rotacao_yoshi -= PASSO;

    /* Rotação da cabeça em x */
    if ( key == '9' )
        rotacao_yoshicx += PASSO;

    if ( key == '(' )
        rotacao_yoshicx -= PASSO;

    /* Rotação da cabeça em y */
    if ( key == '0' )
        rotacao_yoshicy += PASSO;

    if ( key == ')' )
        rotacao_yoshicy -= PASSO;

    /* Rotação do braço direito */
    if ( key == '7' )
        rotacao_yoshibrd += PASSO;

    if ( key == '&' )
        rotacao_yoshibrd -= PASSO;

    /* Rotação do braço esquerdo */
    if ( key == '8' )
        rotacao_yoshibre += PASSO;

    if ( key == '*' )
        rotacao_yoshibre -= PASSO;

    /* Rotação da base do COGUMELO */
    if ( key == '=' )
        rotacao_cogumelo += PASSO;

    if ( key == '+' )
        rotacao_cogumelo -= PASSO;

    /* Leva a câmera para a direita */
    if ( key == 'X' ) {
        camera.posx += PASSO;
        camera.alvox += PASSO; /* Leva o alvo junto */
    }

    /* Leva a câmera para a esquerda */
    if ( key == 'x' ) {
        camera.posx -= PASSO;
        camera.alvox -= PASSO; /* Leva o alvo junto */
    }

    /* Leva a câmera para cima */
    if ( key == 'Y' ) {
        camera.posy += PASSO;
        camera.alvoy += PASSO; /* Leva o alvo junto */
    }

    /* Leva a câmera para baixo */
    if ( key == 'y' ) {
        camera.posy -= PASSO;
        camera.alvoy -= PASSO; /* Leva o alvo junto */
    }

    /* Leva a câmera para trás */
    if ( key == 'Z' ) {
        camera.posz += PASSO;
        camera.alvoz += PASSO; /* Leva o alvo junto */
    }

    /* Leva a câmera para frente */
    if ( key == 'z' ) {
        camera.posz -= PASSO;
        camera.alvoz -= PASSO; /* Leva o alvo junto */
    }

    /* Leva o ponto para onde a câmera 'olha' para a direita */
    if ( key == 'Q' )
        camera.alvox += PASSO;

    /* Leva o ponto para onde a câmera 'olha' para a esquerda */
    if ( key == 'q' )
        camera.alvox -= PASSO;

    /* Leva o ponto para onde a câmera 'olha' para cima */
    if ( key == 'W' )
        camera.alvoy += PASSO;

    /* Leva o ponto para onde a câmera 'olha' para baixo */
    if ( key == 'w' )
        camera.alvoy -= PASSO;

    /* Leva o ponto para onde a câmera 'olha' para a frente */
    if ( key == 'E' )
        camera.alvoz += PASSO;

    /* Leva o ponto para onde a câmera 'olha' para trás */
    if ( key == 'e' )
        camera.alvoz -= PASSO;

    /* Aumenta o ponto inicial de visualização do objetos pela câmetra */
    if ( key == 'K' && camera.inicio + PASSO < camera.fim )
        camera.inicio += PASSO;

    /* Diminui o ponto inicial de visualização do objetos pela câmetra */
    if ( key == 'k' && camera.inicio - PASSO > 0 )
        camera.inicio -= PASSO;

    /* Aumenta o ponto final de visualização do objetos pela câmetra */
    if ( key == 'L' )
        camera.fim += PASSO;

    /* Diminiu o ponto final de visualização do objetos pela câmetra */
    if ( key == 'l' && camera.fim - PASSO > camera.inicio )
        camera.fim -= PASSO;

    /* Aumenta o angulo de visão da câmera (zoom-out) */
    if ( key == 'A' && camera.ang + PASSO < 180 )
        camera.ang += PASSO;

    /* Diminui o angulo de visão da câmera (zoom-in) */
    if ( key == 'a' && camera.ang - PASSO > 0 )
        camera.ang -= PASSO;

    /* Inicializa Tudo */
    if ( key == 'I'  || key == 'i' ) {
        Inicializa();
    }

    EspecificaParametrosVisualizacao();

    /* Obriga Redesenhar */
    glutPostRedisplay();
}

/* Tratamento das teclas especiais (teclas de função e de navegação). os parâmetros que recebe são a tecla pressionada e a posição x e y */
void TeclasEspeciais( GLint key , GLint x , GLint y ) {
    // movimenta a posição da luz
    if ( key == GLUT_KEY_LEFT )
        luz.posicao[ 0 ] -= PASSO;

    // movimenta a posição da luz
    if ( key == GLUT_KEY_RIGHT )
        luz.posicao[ 0 ] += PASSO;

    // movimenta a posição da luz
    if ( key == GLUT_KEY_UP )
        luz.posicao[ 1 ] += PASSO;

    // movimenta a posição da luz
    if ( key == GLUT_KEY_DOWN )
        luz.posicao[ 1 ] -= PASSO;

    // movimenta a posição da luz
    if ( key == GLUT_KEY_PAGE_UP )
        luz.posicao[ 2 ] += PASSO;

    // movimenta a posição da luz
    if ( key == GLUT_KEY_PAGE_DOWN )
        luz.posicao[ 2 ] -= PASSO;

	if( key == GLUT_KEY_F1){
		luz.especular[ 0 ] = 0.8;
    	luz.especular[ 1 ] = 0.0;
    	luz.especular[ 2 ] = 0.0;
    	luz.especular[ 3 ] = 1.0;
	}
	
	if( key == GLUT_KEY_F2){
		luz.especular[ 0 ] = 0.0;
    	luz.especular[ 1 ] = 0.8;
    	luz.especular[ 2 ] = 0.0;
    	luz.especular[ 3 ] = 1.0;
		
	}
	
	if( key == GLUT_KEY_F3){
		luz.especular[ 0 ] = 0.0;
    	luz.especular[ 1 ] = 0.0;
    	luz.especular[ 2 ] = 0.8;
    	luz.especular[ 3 ] = 1.0;
		
	}
	
	if( key == GLUT_KEY_F12){
		Inicializa();
		EspecificaParametrosVisualizacao();
		
	}
	
		
    // como foi mudado dados que influenciam na visualização dos objetos, este comando obriga a executar a função de desenho (Desenha)
    glutPostRedisplay();
}

/* Programa Principal */
int main(void) {
    // especifica a utilização dos buffers duplos e indica o sistema de cores
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

    // especifica as dimensões da janela na tela, em pixels
    glutInitWindowSize( 900 , 600 );

    // especifica a coordenada superior esquerda da janela na tela de vídeo
    glutInitWindowPosition( 0 , 0 );

    // cria a janela com o titulo especificado
    glutCreateWindow( "Trabalho de Computação Gráfica" );

    // determina qual a função do programa que desenhará os objetos
    glutDisplayFunc( Desenha );

    // determina qual a função do programa que será executada a cada alteração do tamanho da janela no video
    glutReshapeFunc( AlteraTamanhoJanela );

    // determina qual a função do programa tratará o uso do mouse
    glutMouseFunc( GerenciaMouse );

    // Registra a função callback para eventos de movimento do mouse
    glutMotionFunc(GerenciaMovim);

    // determina qual a função do programa tratará o teclado
    glutKeyboardFunc( GerenciaTeclado );

    // Determina qual a função do programa que tratará as teclas especiais digitadas ( teclas de função e de navegação)
    glutSpecialFunc( TeclasEspeciais );

    // mostra na console um help sobre as teclas que o programa usa e o que cada uma faz
    printf("\nControles dos Objetos \n\n");
    printf("\nI   => Inicializacao");
    printf("\nX x => muda a posicao x da camera");
    printf("\nY y => muda a posicao y da camera");
    printf("\nZ z => muda a posicao z da camera");
    printf("\nQ q => muda o alvo x");
    printf("\nW w => muda o alvo y");
    printf("\nE e => muda o alvo z");
    printf("\nR r => rotaciona o objeto");
    printf("\nG G => rotaciona a camera");
    printf("\nK k => muda inicio da profundidade de visualizacao");
    printf("\nL l => muda fim da profundidade de visualizacao");
    printf("\nA a => muda o angulo de abertura da lente");
    printf("\nSETAS DE DIRECAO, PAGEUP E PAGE DOWN => movimenta a fonte de luz");

    printf("\n\n Controle do Yoshi \n\n");
    printf("\n- _ => Rotacao da Barriga do Yoshi em Y");
    printf("\n7 & => Rotacao do Braco Direito do Yoshi");
    printf("\n8 * => Rotacao do Braco Esquerdo do Yoshi");
    printf("\n9 ( => Rotacao da Cabeca do Yoshi em X");
    printf("\n0 ) => Rotacao da Cabeca do Yoshi em Y");

    printf("\n\n Controle da Peach \n\n");
    printf("\n1 ! => Rotacao da Barriga do Peach em X");
    printf("\n2 @ => Rotacao do Braco Direito do Peach");
    printf("\n3 # => Rotacao do Braco Esquerdo do Peach");
    printf("\n4 $ => Rotacao da Cabeca do Peach em X");
    printf("\n5 % => Rotacao da Cabeca do Peach em Y");

    printf("\n\n Controle do Cogumelo \n\n");
    printf("\n= + => Rotacao da Base do Cogumelo");

    /* Função simples que inicializa os parâmetros da câmera e da projeção a ser utilizada */
    Inicializa();

    /* Executa */
    glutMainLoop();
}


void DesenhaYoshi(GLUquadricObj *quadObj) {

    /* Yoshi */
    glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		glBindTexture ( GL_TEXTURE_2D, texture_id[3] );
		glTranslatef( -120  , 75 , -50);
		glScalef( 1 , 1 , 1 );
	
	    glPushMatrix(); /* Barriga */
		    glTranslatef( 0  , -40 , 0 );
		    glRotatef( rotacao_yoshi , 0 , 1 , 0 );
		    glScalef( 1 , 1 , 1.3 );
		    gluSphere( quadObj, 50 , 200 , 200 );
		
		    glPushMatrix(); /* Barriga branca */
			    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
			
			    glColor3f( 1.0 , 1.0 , 1.0 );
			    glTranslatef( 0  , 0 , 20 );
			    gluSphere( quadObj, 40 , 200 , 200 );
			
			
			    glColor3f( 1.0 , 1.0 , 1.0 );
		    glPopMatrix(); /* Fecha Barriga branca */
		
		    glEnable(GL_TEXTURE_2D); /* Habilita textura */
		
		    glPushMatrix(); /* Rabo */
			    glTranslatef( 0  , 0 , -20 );
			    glRotatef( -160 , 1 , 0 , 0 );
			    gluCylinder(quadObj, 30, 1 , 100, 40, 20);
		    glPopMatrix(); /* Fecha rabo */
		
		    glPushMatrix(); /* Perna Esquerda */
			    glTranslatef( 15 , -68 , 0 );
			    glRotatef( -90 , 1 , 0 , 0 );
			    gluCylinder(quadObj, 10, 10 , 50, 40, 20);
			
			    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
			
			    glPushMatrix(); /* Pé Esquerdo */
				    glColor3f( 0.528 , 0.464 , 0.0 );
				    glTranslatef( 0 , -5 , -8 );
				    glScalef( 1 , 1.5 , 0.8 );
				    gluSphere( quadObj, 15 , 200 , 200 );
			    glPopMatrix(); /* Fecha Pé Esquerdo */
		
		    glPopMatrix(); /* Fecha Perna Esquerda */
		
		    glEnable(GL_TEXTURE_2D); /* Habilita textura */
		
		    glPushMatrix(); /* Perna Direita */
			    glTranslatef( -15 , -68 , 0 );
			    glRotatef( -90 , 1 , 0 , 0 );
			    gluCylinder(quadObj, 10, 10 , 50, 40, 20);
			
			    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
			
			    glPushMatrix(); /* Pé Direito */
				    glColor3f( 0.528 , 0.464 , 0.0 );
				    glTranslatef( 0 , -5 , -8 );
				    glScalef( 1 , 1.5 , 0.8 );
				    gluSphere( quadObj, 15 , 200 , 200 );
			    glPopMatrix(); /* Fecha Pé Direito */
		
		    glPopMatrix(); /* Fecha Perna Direita */
		
		    glEnable(GL_TEXTURE_2D); /* Habilita textura */
		
		    glPushMatrix(); /* Braço esquerdo */
			    glTranslatef( 30 , 23 , 20 );
			    glRotatef( 30 , -90 , -1 , 0 );
			    glRotatef( rotacao_yoshibre+45 , 1 , 0 , 0 );
			    gluCylinder(quadObj, 8, 5 , 50, 40, 20);
			
			    glPushMatrix(); /* Mão Esquerda */
			    glTranslatef( 0  , 0 , 50 );
			    glScalef( 1 , 1 , 1 );
			    gluSphere( quadObj, 10 , 200 , 200 );
		    glPopMatrix(); /* Fecha Mão Esquerda */
		
		    glPopMatrix(); /* Fecha Braço Esquerdo */
		
		    glPushMatrix(); /* Braço Direito */
		    glTranslatef( -30 , 23 , 20 );
		    glRotatef( -60 , -90 , -1 , 0 );
		    glRotatef( -40 , 1 , 0 , 0 );
		    glRotatef( rotacao_yoshibrd-10 , 1 , 0 , 0 );
		    gluCylinder(quadObj, 8, 5 , 50, 40, 20);
		
		    glPushMatrix(); /* Mão Direita */
		    glTranslatef( 0  , 0 , 50 );
		    glScalef( 1 , 1 , 1 );
		    gluSphere( quadObj, 10 , 200 , 200 );
		    glPopMatrix(); /* Fecha Mão Direita */
		
		    glPopMatrix(); /* Fecha Braço Direito */
		
		    glPushMatrix(); /* Pescoço */
		    glTranslatef( 0  , 30 , 20 );
		    glRotatef( -90 , 1 , 0 , 0 );
		    gluCylinder(quadObj, 20, 12 , 80, 40, 20);
		
		
		    glPushMatrix(); /* Pescoço branco */
		    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
		
		    glColor3f( 1.0 , 1.0 , 1.0 );
		    glTranslatef( 0  , -5 , 0 );
		    gluCylinder(quadObj, 18, 10 , 80, 40, 20);
		
		
		    glColor3f( 1.0 , 1.0 , 1.0 );
		    glPopMatrix(); /* Fecha pescoço branco */
		
		
		    glPushMatrix(); /* Espinhos */
		
		    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
		    glColor3f( 1.0 , 0 , 0 );
		    glTranslatef( 0  , 28 , 25 );
		    glRotatef( 90 , 1 , 0 , 0 );
		    gluCylinder(quadObj, 1, 9 , 15, 40, 20);
		
		    glTranslatef( 0  , 18, 0 );
		    gluCylinder(quadObj, 1, 9 , 15, 40, 20);
		
		    glTranslatef( 0  , 18, 0 );
		    gluCylinder(quadObj, 1, 9 , 15, 40, 20);
		
		    glTranslatef( 0  , 18, 0 );
		    gluCylinder(quadObj, 1, 9 , 15, 40, 20);
		    glColor3f( 1 , 1 , 1 );
		    glEnable(GL_TEXTURE_2D); /* Habilita textura */
		    glPopMatrix(); /* Fecha espinhos */
		
		
		    glPushMatrix(); /* Cabeça */
		    glRotatef( 90 , 1 , 0 , 0 );
		    glTranslatef( 0  , 100 , 0 );
		    glRotatef( rotacao_yoshicx, 0, 1,0);
		    glRotatef( rotacao_yoshicy, 1, 0, 0);
		    glScalef( 0.7 , 0.7 , 0.6 );
		    gluSphere( quadObj, 45 , 200 , 200 );
		
		    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
		
		    glPushMatrix(); /* Olho Direito */
		    glColor3f( 1.0 , 1.0 , 1.0 );
		    glTranslatef( 20 , 55 , 0 );
		    glScalef( 3 , 8 , 3 );
		    glutSolidSphere( 6, 200, 200 );
		
		    glPushMatrix(); /* Olho */
		    glColor3f( 0.5 , 0.5 , 1.0 );
		    glTranslatef( 1 , -0.3 , 5 );
		    glutSolidSphere( 2, 200, 200 );
		    glPopMatrix(); /* Fecha Olho */
		
		    glPopMatrix(); /* Fecha Olho Direito */
		
		    glPushMatrix(); /* Olho Esquerdo */
		    glColor3f( 1.0 , 1.0 , 1.0 );
		    glTranslatef( -10 , 55 , 0 );
		    glScalef( 3 , 8 , 3 );
		    glutSolidSphere( 6, 200, 200 );
		
		    glPushMatrix(); /* Olho */
		    glColor3f( 0.5 , 0.5 , 1.0 );
		    glTranslatef( 0 , -0.3 , 5 );
		    glutSolidSphere( 2, 200, 200 );
		    glPopMatrix(); /* Fecha olho */
		
		    glPopMatrix(); /* Fecha Olho Esquerdo */
		
		    glEnable(GL_TEXTURE_2D); /* Habilita textura */
		
		    glPushMatrix(); /* Boca - Nariz */
		    glTranslatef( 0  , -20 , 70 );
		    glScalef( 1 , 1 , 1 );
		    gluSphere( quadObj, 45 , 200 , 200 );
		    glPopMatrix(); /* Fecha Boca - Nariz */
		
		    glPopMatrix(); /* Fecha cabeça */
		    glPopMatrix(); /* Fecha pescoço */
	    glPopMatrix(); /* Fecha barriga */
    glPopMatrix(); /* Fecha Yoshi */


}

void DesenhaPeach(GLUquadricObj *quadObj) {

    /*Peach*/
    glPushMatrix();{
		
    	glTranslatef( 20  , 47, -50);
    	glScalef( 0.5 , 0.5 , 0.5 );
		
    	/* Abre Tronco */
    	glPushMatrix();{
    		glBindTexture ( GL_TEXTURE_2D, texture_id[5] );
    		glRotatef( rotacao_l , 0 , 1 , 0 );
    		glRotatef( -90 , 1 , 0 , 0 );
    		gluCylinder(quadObj, 40, 45 , 100, 40, 50);
		
    		/* Peito Direito */
			glPushMatrix(); {
				    glTranslatef( -18  , -35 , 80);
				    glScalef( 1.5 , 2 , 1.5 );
				    gluSphere( quadObj, 12 , 200 , 200 );
			}glPopMatrix(); /* Fecha Peito Direito */
			
			/* Peito Esquerdo */
    		glPushMatrix();{ 
				    glTranslatef( 21  , -35 , 80);
				    glScalef( 1.5 , 2 , 1.5 );
				    gluSphere( quadObj, 12 , 200 , 200 );
			}glPopMatrix(); /* Fecha Peito Esquerdo*/
		
    		/* Abre Saia */
    		glPushMatrix();{
	    		glTranslatef( 0  , 0 , -45 );
				glScalef( 1.5 , 2 , 1.5 );
				gluSphere( quadObj, 50 , 200 , 200 );
				
				glPushMatrix(); /* Barra da saia */
	    			glTranslatef( 0 , 0 , -95 );
  					glRotatef( 0 , 0 , 0 , 0 );
	    			gluCylinder(quadObj, 70, 50 , 100, 40, 20);
	    		glPopMatrix(); /* Barra da saia */
			
			}glPopMatrix(); //Fecha Saia
		
			/* Abre Ombro Esquerdo */
    		glPushMatrix();{
			    glTranslatef( 70 , 10 , 80 );
			    glScalef( 1 , 1.5 , 0.8 );
			    glRotatef( rotacao_lbre-10 , 1 , 0, 0 );
			    gluSphere( quadObj, 35 , 10 , 10 );
			
			
			    glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
			
			    glPushMatrix(); /* Braço Esquerdo */
				    glTranslatef( 0 , 0 , -30 );
				    glRotatef( 180 , 1 , 0 , 0 );
				    gluCylinder(quadObj, 10, 10 , 75, 40, 20);
				
				    glPushMatrix(); /* Mão Esquerda */
					    glTranslatef( 0  , 0 , 70 );
					    glScalef( 1 , 1 , 1 );
					    gluSphere( quadObj, 18 , 200 , 200 );
			    	glPopMatrix(); /* Fecha Mão Esquerda */
			
				glPopMatrix(); /* Fecha Braço Esquerdo */
			
			
			}glPopMatrix(); /* Fecha Ombro Esquerdo */
		
    		/* Abre Ombro Direito */
    		glPushMatrix();{
	    		glBindTexture ( GL_TEXTURE_2D, texture_id[5] );
				glTranslatef( -70 , 10 , 80 );
				glScalef( 1 , 1.5 , 0.8 );
				glRotatef( rotacao_lbrd-10 , 1 , 0 , 0 );
				gluSphere( quadObj, 35 , 10 , 10 );				
				
				    glPushMatrix(); /* Braço Direito */
					    glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
					    glTranslatef( 0 , 0 , -30 );
					    glRotatef( 180 , 1 , 0 , 0 );
					
					    gluCylinder(quadObj, 10, 10 , 75, 40, 20);
					
					    glPushMatrix(); /* Mão Direita */
						    glTranslatef( 0  , 0 , 70 );
						    glScalef( 1 , 1 , 1 );
						    gluSphere( quadObj, 18 , 200 , 200 );
					    glPopMatrix(); /* Fecha Mão Direita */
				
				    glPopMatrix(); /* Fecha Braço Esquerdo */
			
			}glPopMatrix(); /* Fecha Ombro Direito */
		
		    /* Abre Pescoço */
		    glPushMatrix();{
	    		glTranslatef( 0  , 0 , 95 );
				glScalef( 1 , 1 , 0.5 );
	    		gluSphere( quadObj, 44 , 200 , 200 );
	    		glTranslatef( 0 , 0 , -10 );
  				glRotatef( 0 , 0 , 0 , 0 );
	    		glRotatef( rotacao_lcy, 0, 0, 1);
    			gluCylinder(quadObj, 10, 10 , 75, 40, 20);
			
	    		/* Cabeça */
	    		glPushMatrix();{
				    glTranslatef( 0  , 0 ,130);
				    glRotatef( 0 , 0 , 0 , 0 );
				    glScalef( 1 , 1 , 1.3 );
				    glRotatef( rotacao_lcx , 1 , 0 , 0 );
				    gluSphere( quadObj, 50 , 200 , 200 );
				
				    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
				
				    /* Boca */
				    glPushMatrix();
    					glColor3f( 1 , 0 , 0 );
			    		glTranslatef( 0  , -35 , -40);
			    		gluCylinder(quadObj, 2, 15 , 15, 40, 50);
					
			    	glPopMatrix(); /* Fecha Boca */
			
				    /* Olho Direito */
				    glPushMatrix();
				    	glColor3f( 1.0 , 1.0 , 1.0 );
				    	glTranslatef( -15 , -48 , 0 );
				    	glScalef( 1.5 , 1 , 2.5 );
				    	glutSolidSphere( 6, 200, 200 );
			
					    glPushMatrix(); /* Olho */
						    glColor3f( 0 , 0 , 1 );
						    glTranslatef( 2 , -6 , 0 );
						    glutSolidSphere( 3, 200, 200 );
					    glPopMatrix(); /* Fecha Olho */
				
				    glPopMatrix(); /* Fecha Olho Direito */
				
				    /* Olho Esquerdo */
		    		glPushMatrix();
					    glColor3f( 1.0 , 1.0 , 1.0 );
					    glTranslatef( 15 , -48 , 0 );
					    glScalef( 1.5 , 1 , 2.5 );
					    glutSolidSphere( 6, 200, 200 );
					
					    glPushMatrix(); /* Olho */
						    glColor3f( 0 , 0 , 1 );
						    glTranslatef( 2 , -6 , 0 );
						    glutSolidSphere( 3, 200, 200 );
		    			glPopMatrix();/* Fecha Olho */
				
				    glPopMatrix(); /* Fecha Olho Esquerdo */
				
				    glEnable(GL_TEXTURE_2D); /* Habilita textura */
				
				    /* Cabelo */
				    glPushMatrix();
					    glEnable(GL_TEXTURE_2D);
					    glBindTexture ( GL_TEXTURE_2D, texture_id[6] );
					    glColor3f( 1.0 , 1.5 , 0 );
					    glTranslatef( 0  , 0 , 55);
					    glRotatef( 0 , 0 , 0 , 0 );
					    glScalef( 1 , 1 , 1.3 );
					    gluSphere( quadObj, 50 , 200 , 200 );
					
					
					    glColor3f( 1.0 , 1.5 , 0 );
					    glTranslatef( 0  , 35 , -60 );
					    glRotatef( 0 , 0 , 0 , 0 );
					    glScalef( 1.3 , 0.8 , 2.4 );
					    gluSphere( quadObj, 50 , 200 , 200 );
					    glDisable(GL_TEXTURE_2D); /* Desabilita textura */
				    glPopMatrix(); /* Fecha cabelo*/
			
			
				}glPopMatrix(); /* Fecha Cabeça */
		
		
			}glPopMatrix(); /* Fecha Pescoço */
		
		}
		glPopMatrix(); /* Fecha Tronco */
	}
    glPopMatrix(); /* Fecha todo o personagem da Peach */

}

void DesenhaCogumelo(GLUquadricObj *quadObj) {

    /* Cogumelo */
    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
    
	glPushMatrix();
		glTranslatef( 180  , -50 , -100);
	    glScalef( 0.8 , 0.8 , 0.8 );
	
	    glPushMatrix(); /* Base */
		    glTranslatef( 0 , 0 , 20 );
		    glRotatef( -90 , 1 , 0 , 0 );
		    glRotatef( rotacao_cogumelo , 0 , 0 , 1 );
		    gluCylinder(quadObj, 30, 30 , 40, 40, 20);
	
		    glPushMatrix(); /* Topo */
			    glBindTexture ( GL_TEXTURE_2D, texture_id[4] );
			    glRotatef( 90 , 1 , 0 , 0 );
			    glTranslatef(  0 , 55 , 0 );
			    glScalef( 1 , 0.8 , 1 );
			    gluSphere( quadObj, 50 , 200 , 200 );
	
	    	glPopMatrix(); /* Fecha Topo */
	    glPopMatrix(); /* Fecha Base */
    glPopMatrix(); /* Fecha Cogumelo */


}

void DesenhaCaixas(){
	
    glEnable(GL_TEXTURE_2D);
    glBindTexture ( GL_TEXTURE_2D, texture_id[7] );
	
	glPushMatrix(); 
		glScalef(30,30,30);		
		glBegin ( GL_QUADS );
			// Front Face
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			// Back Face
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			// Top Face
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			// Bottom Face
			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			// Right face
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			// Left Face
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
	glPopMatrix();
	
	glTranslatef(63,0,0);
	
	glPushMatrix();
		glScalef(30,30,30);			
		glBegin ( GL_QUADS );
			// Front Face
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			// Back Face
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			// Top Face
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			// Bottom Face
			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			// Right face
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			// Left Face
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
	
	glPopMatrix(); 
	
	glTranslatef(-31,60,0);
	
	glPushMatrix();
		glScalef(30,30,30);			
		glBegin ( GL_QUADS );
			// Front Face
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			// Back Face
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			// Top Face
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			// Bottom Face
			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			// Right face
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			// Left Face
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
	
	glPopMatrix();
	
	
	glDisable(GL_TEXTURE_2D);
	
}

