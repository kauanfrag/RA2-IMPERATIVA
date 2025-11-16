#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_DESC 150

enum Categoria {
    CEREAIS = 0,
    VERDURAS,
    FRUTAS,
    GORDURAS,
    PESCADOS,
    CARNES,
    LEITE,
    BEBIDAS,
    OVOS,
    ACUCARADOS,
    MISCELANEAS,
    INDUSTRIALIZADOS,
    PREPARADOS,
    LEGUMINOSAS,
    NOZES_SEMENTES,
    CATEGORIA_INVALID
};

typedef struct Comidinha {
    int codigo;
    char descricao[TAM_DESC];
    float energia;
    float proteina;
    int categoria;
} Comidinha;

static void corta_nl(char *s)
{
    size_t L = strlen(s);
    while (L > 0) {
        char c = s[L-1];
        if (c == '\n' || c == '\r') { s[L-1] = '\0'; L--; } else break;
    }
}

static int faz_bin(const char *csv, const char *bin)
{
    int ok = 0;
    FILE *f = fopen(csv, "r");
    if (!f) return ok;
    FILE *o = fopen(bin, "wb");
    if (!o) { fclose(f); return ok; }
    char linha[1024];
    if (fgets(linha, sizeof(linha), f) == NULL) { fclose(f); fclose(o); return ok; }
    int cont = 0;
    if (fwrite(&cont, sizeof(int), 1, o) != 1) { fclose(f); fclose(o); return ok; }
    while (fgets(linha, sizeof(linha), f) != NULL) {
        Comidinha x;
        char *p = linha;
        char *t;
        t = strtok(p, ",");
        if (!t) continue;
        x.codigo = atoi(t);
        t = strtok(NULL, ",");
        if (!t) continue;
        strncpy(x.descricao, t, TAM_DESC-1);
        x.descricao[TAM_DESC-1] = '\0';
        corta_nl(x.descricao);
        t = strtok(NULL, ",");
        t = strtok(NULL, ",");
        if (t) x.energia = strtof(t, NULL); else x.energia = 0.0f;
        t = strtok(NULL, ",");
        if (t) x.proteina = strtof(t, NULL); else x.proteina = 0.0f;
        t = strtok(NULL, ",");
        t = strtok(NULL, ",");
        if (t) {
            int cid = atoi(t);
            if (cid >= CEREAIS && cid <= NOZES_SEMENTES) x.categoria = cid;
            else x.categoria = CATEGORIA_INVALID;
        } else x.categoria = CATEGORIA_INVALID;
        if (fwrite(&x, sizeof(Comidinha), 1, o) == 1) cont++;
    }
    if (fseek(o, 0, SEEK_SET) == 0) fwrite(&cont, sizeof(int), 1, o);
    fclose(f);
    fclose(o);
    ok = 1;
    return ok;
}

int main(void)
{
    int ok = faz_bin("alimentos.csv", "dados.bin");
    if (ok) printf("OK\n"); else printf("ERRO\n");
    return 0;
}
