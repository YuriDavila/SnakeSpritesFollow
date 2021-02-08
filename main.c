#include <genesis.h>
#include "sprites.h"

//***** DEFINE CONSTANTES *****//

//Limites da Tela
#define TELA_ESQUERDA		FIX32(0)	//= 0 + 20;
#define TELA_DIREITA		FIX32(320)	//= 320 - 20;
#define TELA_CIMA			FIX32(0)	//= 0 + 20;
#define TELA_BAIXO			FIX32(224)	//= 240; OU FIX32(screenHeight)

#define MAX_PARTS			12	//era pra ser 12

//***** DEFINE FUNÇÕES ESTATICAS *****//

static void ConfiguraSnake();
static void MoveSnakeHead();
static void MoveSnakeBody();

//***** DEFINE PONTEIROS DAS SPRITES *****// 

Sprite* inimSprite[MAX_PARTS];

//***** DEFINE VARIÁVEIS *****//

//BOSS SNAKE CONFIG
u16 updateFrame = 0;
u16 maxUpdateFrames = 2;	//era pra ser 2

u16 maxParts = MAX_PARTS;
u16 headPositionIndex = 0;

fix32 headX[MAX_PARTS];
fix32 headY[MAX_PARTS];

//VARS do corpo da Snake
fix32 posX;
fix32 posY;
fix32 movX;
fix32 movY;
int w;
int h;
s16 xOrder;
s16 yOrder;


//DEBUG
void Debugar() {
	//VAR para exibir os dados
	char _string_debug[32];

	//Debug do JOGO

	//POSX
	sprintf(_string_debug, "%d", fix32ToInt(posX)); //convertemos a string

	VDP_drawText("PosX: ", 2, 12);
	VDP_drawText(_string_debug, 2, 13);

	//POSY
	sprintf(_string_debug, "%d", fix32ToInt(posY)); //convertemos a string

	VDP_drawText("PosY: ", 2, 15);
	VDP_drawText(_string_debug, 2, 16);

}

//MAIN
int main(u16 hard) {
	u16 paleta64[64];
	//u16 ind;
	//u16 numTile;

	// Inicia Processador de Video VDP, e estabelece resolução da tela 320x224
	VDP_init(); //(Best Practice)
	VDP_setScreenWidth320();
	//VDP_setScreenHeight224();

	//Apaga a tela e as Sprites - CLS e FreeAllSprites do BASIC
	VDP_resetScreen();	//(Best Practice)
	VDP_resetSprites(); //Depreciado (Best Practice)

	//VDP_setPlanSize(64, 64);

	//Inicia driver de Som
	//Z80_init();

	//Carrega Font png
	//VDP_doVRamDMA((u32)tile_font8bits, 46080, 1536); //0x05A0 1536 = largura das MINHAS fonts / Exemplos = Metade /41984=posDefault

	//inicializa motor de sprites
	SPR_init();

	//Seta todas as paletas para Black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);
	
	//***** MONTA TELA JOGO
	//ind = 1; //Reinicia indice de tileset

	ConfiguraSnake();

	//Cores
	memcpy(&paleta64[0], spr_starblade.palette->data, 16 * 2);

	//fade in
	VDP_fadeIn(0, (4 * 16) - 1, paleta64, 20, FALSE);

	while (TRUE) {

		Debugar();

		MoveSnakeHead();
		MoveSnakeBody();

		//update sprites
		SPR_update();

		//sync frame and do vblank process
		SYS_doVBlankProcess();

		VDP_clearTextLine(13);
		VDP_clearTextLine(16);
	}

	return 0;
}

static void ConfiguraSnake() {
		
	for (int i = 0; i < (maxParts - 1); i++) {
		headX[i] = FIX32(-100);
		headY[i] = FIX32(-100);
	}

	xOrder = 1;
	yOrder = 1;
	
	posX = FIX32(-100);
	posY = FIX32(-100);

	movX = FIX32(3);
	movY = FIX32(3);
	
	h = 32;
	w = 32;
	
	//SPRITES

	//Head Sprite / Position
	inimSprite[0] = SPR_addSprite(&spr_starbladeHead, fix32ToInt(posX), fix32ToInt(posY), TILE_ATTR(PAL0, 0, FALSE, FALSE));
	
	//Body Sprites
	for (int i = 1; i < maxParts - 1; i++) {
		if (i < maxParts - 5) {
			//Corpo Grande
			inimSprite[i] = SPR_addSprite(&spr_starblade, -100, -100, TILE_ATTR(PAL0, 0, FALSE, FALSE));
		} else if (i < maxParts - 3) {
			//Corpo Medio
			inimSprite[i] = SPR_addSprite(&spr_starblade, -100, -100, TILE_ATTR(PAL0, 0, FALSE, FALSE));
		} else {
			//Corpo Pequeno
			inimSprite[i] = SPR_addSprite(&spr_starblade, -100, -100, TILE_ATTR(PAL0, 0, FALSE, FALSE));
		}
	}

	//Sprite do Rabo
	inimSprite[maxParts - 1] = SPR_addSprite(&spr_starbladeHead, -100, -100, TILE_ATTR(PAL0, 0, FALSE, FALSE));
		
	SPR_update();
	
}

static void MoveSnakeHead() {
	
	//Movimenta
	posX += movX * xOrder;
	posY += movY * yOrder;

	//Quica nas laterais da tela
	if (posX < TELA_ESQUERDA) {
		posX = TELA_ESQUERDA;
		xOrder = -xOrder;
	} else if (posX + FIX32(w) > TELA_DIREITA) {
		posX = TELA_DIREITA - FIX32(w);
		xOrder = -xOrder;
	}

	//Quica embaixo e em cima da tela
	if (posY < TELA_CIMA) {
		posY = TELA_CIMA;
		yOrder = -yOrder;
	} else if (posY + FIX32(h) > TELA_BAIXO) {
		posY = TELA_BAIXO - FIX32(h);
		yOrder = -yOrder;
	}

	//Posiciona a cabeça
	SPR_setPosition(inimSprite[0], fix32ToInt(posX), fix32ToInt(posY));
}

static void MoveSnakeBody() {
	//update Frame
	updateFrame = updateFrame ? updateFrame - 1 : maxUpdateFrames;

	//Last Position
	if (updateFrame == maxUpdateFrames) {
		//next Position
		headPositionIndex = headPositionIndex ? headPositionIndex - 1 : maxParts - 2; // era pra ser maxParts-2

		//save Head position
		headX[headPositionIndex] = posX;
		headY[headPositionIndex] = posY;

		//body position
		int _bodyPosIndex;

		for (int i = 0; i < maxParts - 1; i++) {
			_bodyPosIndex = headPositionIndex + i;
			if (_bodyPosIndex >= maxParts - 1) {
				_bodyPosIndex = _bodyPosIndex - (maxParts - 1);
			}
			
			//Posiciona as partes do corpo
			SPR_setPosition(inimSprite[i+1], fix32ToInt(headX[_bodyPosIndex]), fix32ToInt(headY[_bodyPosIndex]));
		}

	}
}
