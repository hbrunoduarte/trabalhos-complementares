#include "libs/config.h"
#include "libs/fisica.h"

CorpoCeleste criarCorpoCeleste(float massa, float densidade, vector posicao,
                                float distanciaOrbital, float velAngular, float velRotacao,
                                funcaoRenderizacao r, void* dados, CorpoCeleste *orbita) {

    CorpoCeleste corpo;

    corpo.massa = massa;
    corpo.densidade = densidade;
    
    if (orbita == NULL) {
        corpo.posicao = posicao;
    } else {
        corpo.posicao = orbita->posicao;
        corpo.posicao.x += distanciaOrbital + orbita->raio;
    }
    corpo.distanciaOrbital = distanciaOrbital;
    corpo.anguloAtual = 0.0f;
    corpo.velocidadeAngular = velAngular;

    corpo.raio = calcularRaio(&corpo);
    corpo.velocidadeRotacao = velRotacao;
    corpo.renderizar = r;
    corpo.dadosVisuais = dados;
    corpo.orbita = orbita;

    vector p = calcularPosicaoVisual(&corpo);
    printf("posicao: (%f, %f, %f) | posicaoVisual: (%f, %f, %f) | raio: %f | raioVisual: %f\n", 
        corpo.posicao.x, corpo.posicao.y, corpo.posicao.z,
        p.x, p.y, p.z,
        corpo.raio, corpo.raio/SIZE_RATIO);

    return corpo;
}

float magnitude(vector u) {
    return (float) sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

void definirPosicao(CorpoCeleste *corpo, float distancia) {

    if (!corpo->orbita) return;

    corpo->posicao = corpo->orbita->posicao;
    corpo->posicao.x += distancia;

}

float calcularRaio(CorpoCeleste *corpo) {
    return (float) pow(3 * corpo->massa / (4 * M_PI * corpo->densidade), 1.0/3.0);
}


void atualizarFisica(CorpoCeleste *corpo, float deltaTime) {

    if (!corpo->orbita) return;

    corpo->anguloAtual += corpo->velocidadeAngular * deltaTime;

    if (corpo->anguloAtual > 2 * M_PI) {
        corpo->anguloAtual -= 2 * M_PI;
    }

    float xRelativo = corpo->distanciaOrbital * cos(corpo->anguloAtual);
    float zRelativo = corpo->distanciaOrbital * sin(corpo->anguloAtual);

    corpo->posicao.x = corpo->orbita->posicao.x + xRelativo;
    corpo->posicao.y = corpo->orbita->posicao.y;
    corpo->posicao.z = corpo->orbita->posicao.z + zRelativo;
}

float calcularRaioVisual(CorpoCeleste *corpo) {
    if (!corpo) return 0.0f;
    return corpo->raio / SIZE_RATIO;
}

vector calcularPosicaoVisual(CorpoCeleste *corpo) {

    if (!corpo->orbita) return corpo->posicao;

    vector direcao = addVectors(corpo->posicao, mulVector(corpo->orbita->posicao, -1.0f));
    float distanciaReal = magnitude(direcao);

    if (distanciaReal <= 0.0001f) return calcularPosicaoVisual(corpo->orbita);

    vector direcaoNorm = mulVector(direcao, 1.0/distanciaReal);

    float distanciaEmAU = distanciaReal / DISTANCIA_TERRA_SOL;

    float distanciaVisualLog = ESCALA_VISUAL_BASE * log2(distanciaEmAU + 1.0f);

    float raioOrbitado = calcularRaioVisual(corpo->orbita);
    float distanciaFinalVisual = distanciaVisualLog + raioOrbitado;

    vector posicaoVisualRelativa = mulVector(direcaoNorm, distanciaFinalVisual);

    vector posicaoMundoOrbitado = calcularPosicaoVisual(corpo->orbita);

    return addVectors(posicaoMundoOrbitado, posicaoVisualRelativa);
}
