#ifndef _FISICA_H
#define _FISICA_H

#include "config.h"

#define CONSTANTE_GRAVITACIONAL 6.6743e-20f
#define DISTANCIA_TERRA_SOL 149597870.7f // 1 AU (Unidade Astronómica) em km
#define ESCALA_VISUAL_BASE 60.0f
#define SIZE_RATIO 12742000.0f

void atualizarFisica(CorpoCeleste *corpo, float deltaTime);
float calcularRaio(CorpoCeleste *corpo);
void definirPosicao(CorpoCeleste *corpo, float distancia);
CorpoCeleste criarCorpoCeleste(float massa, float densidade, vector posicao,
                                float distanciaOrbital, float velAngular, float velRotacao,
                                funcaoRenderizacao r, void* dados, CorpoCeleste *orbita);
float calcularRaioVisual(CorpoCeleste *corpo);
vector calcularPosicaoVisual(CorpoCeleste *corpo);

#endif