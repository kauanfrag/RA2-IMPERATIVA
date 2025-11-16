#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef struct NodoCom {
    Comidinha dado;
    struct NodoCom *prox;
} NodoCom;

typedef struct NodoCat {
    int id;
    NodoCom *itens;
    struct NodoCat *prox;
} NodoCat;

typedef struct NodoArv {
    float chave;
    NodoCom *ref;
    struct NodoArv *esq;
    struct NodoArv *dir;
} NodoArv;

typedef struct ParArv {
    NodoArv *porEnergia;
    NodoArv *porProteina;
} ParArv;

static const char *NOMES_CAT[] = {
    "Cereais e derivados",
    "Verduras, hortaliças e derivados",
    "Frutas e derivados",
    "Gorduras e óleos",
    "Pescados e frutos do mar",
    "Carnes e derivados",
    "Leite e derivados",
    "Bebidas",
    "Ovos e derivados",
    "Produtos açucarados",
    "Miscelaneas",
    "Outros alimentos industrializados",
    "Alimentos preparados",
    "Leguminosas e derivados",
    "Nozes e sementes"
};

static int ci_cmp(const char *a, const char *b)
{
    size_t i = 0;
    while (1) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca == '\0' && cb == '\0') return 0;
        if (ca == '\0') return -1;
        if (cb == '\0') return 1;
        char la = (char)tolower(ca), lb = (char)tolower(cb);
        if (la < lb) return -1;
        if (la > lb) return 1;
        i++;
    }
}

static int ler_bin(const char *path, Comidinha **out, int *qt)
{
    int ok = 0, n = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return ok;
    if (fread(&n, sizeof(int), 1, f) != 1) { fclose(f); return ok; }
    if (n <= 0) { *out = NULL; *qt = 0; fclose(f); ok = 1; return ok; }
    Comidinha *arr = (Comidinha*)malloc((size_t)n * sizeof(Comidinha));
    if (!arr) { fclose(f); return ok; }
    size_t g = fread(arr, sizeof(Comidinha), (size_t)n, f);
    if ((int)g != n) { free(arr); fclose(f); return ok; }
    fclose(f);
    *out = arr; *qt = n; ok = 1; return ok;
}

static NodoCat* novo_cat(int id)
{
    NodoCat *n = (NodoCat*)calloc(1, sizeof(NodoCat));
    if (n) { n->id = id; n->itens = NULL; n->prox = NULL; }
    return n;
}

static NodoCom* novo_com(const Comidinha *c)
{
    NodoCom *n = (NodoCom*)malloc(sizeof(NodoCom));
    if (n) { n->dado = *c; n->prox = NULL; }
    return n;
}

static void coloca_alf(NodoCom **cab, NodoCom *nv)
{
    NodoCom *cur = *cab, *ant = NULL; int feito = 0;
    while (!feito) {
        if (cur == NULL) {
            if (ant == NULL) *cab = nv; else ant->prox = nv;
            feito = 1;
        } else {
            int cmp = ci_cmp(nv->dado.descricao, cur->dado.descricao);
            if (cmp <= 0) {
                if (ant == NULL) { nv->prox = *cab; *cab = nv; }
                else { nv->prox = ant->prox; ant->prox = nv; }
                feito = 1;
            } else { ant = cur; cur = cur->prox; }
        }
    }
}

static void coloca_cat_alf(NodoCat **cab, NodoCat *nv)
{
    NodoCat *cur = *cab, *ant = NULL; int feito = 0;
    while (!feito) {
        if (cur == NULL) {
            if (ant == NULL) *cab = nv; else ant->prox = nv;
            feito = 1;
        } else {
            int cmp = strcmp(NOMES_CAT[nv->id], NOMES_CAT[cur->id]);
            if (cmp <= 0) {
                if (ant == NULL) { nv->prox = *cab; *cab = nv; }
                else { nv->prox = ant->prox; ant->prox = nv; }
                feito = 1;
            } else { ant = cur; cur = cur->prox; }
        }
    }
}

static NodoCat* acha_cat(NodoCat *cab, int id)
{
    NodoCat *cur = cab;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->prox;
    }
    return NULL;
}

static void joga_na_cat(NodoCat **cab, const Comidinha *c)
{
    if (!c) return;
    NodoCat *ct = acha_cat(*cab, c->categoria);
    if (!ct) {
        NodoCat *nc = novo_cat(c->categoria);
        if (!nc) return;
        coloca_cat_alf(cab, nc);
        ct = acha_cat(*cab, c->categoria);
        if (!ct) return;
    }
    NodoCom *nc = novo_com(c);
    if (!nc) return;
    coloca_alf(&ct->itens, nc);
}

static NodoArv* novo_no(float chave, NodoCom *ref)
{
    NodoArv *n = (NodoArv*)malloc(sizeof(NodoArv));
    if (n) { n->chave = chave; n->ref = ref; n->esq = NULL; n->dir = NULL; }
    return n;
}

static NodoArv* arv_insere(NodoArv *raiz, float chave, NodoCom *ref)
{
    if (!raiz) return novo_no(chave, ref);
    NodoArv *cur = raiz; int ok = 0;
    while (!ok) {
        if (chave < cur->chave) {
            if (!cur->esq) { cur->esq = novo_no(chave, ref); ok = 1; } else cur = cur->esq;
        } else if (chave > cur->chave) {
            if (!cur->dir) { cur->dir = novo_no(chave, ref); ok = 1; } else cur = cur->dir;
        } else {
            int cmp = strcmp(ref->dado.descricao, cur->ref->dado.descricao);
            if (cmp <= 0) {
                if (!cur->esq) { cur->esq = novo_no(chave, ref); ok = 1; } else cur = cur->esq;
            } else {
                if (!cur->dir) { cur->dir = novo_no(chave, ref); ok = 1; } else cur = cur->dir;
            }
        }
    }
    return raiz;
}

static ParArv* monta_par(NodoCat *cat)
{
    ParArv *p = (ParArv*)calloc(1, sizeof(ParArv));
    if (!p) return NULL;
    NodoCom *it = cat->itens;
    while (it) {
        p->porEnergia = arv_insere(p->porEnergia, it->dado.energia, it);
        p->porProteina = arv_insere(p->porProteina, it->dado.proteina, it);
        it = it->prox;
    }
    return p;
}

static void print_dec(NodoArv *r)
{
    if (!r) return;
    print_dec(r->dir);
    printf("%d %s %.0f %.1f\n", r->ref->dado.codigo, r->ref->dado.descricao, r->ref->dado.energia, r->ref->dado.proteina);
    print_dec(r->esq);
}

static void imprime_range_rec(NodoArv *r, float min, float max)
{
    if (!r) return;
    imprime_range_rec(r->dir, min, max);
    if (r->chave >= min && r->chave <= max) printf("%d %s %.0f %.1f\n", r->ref->dado.codigo, r->ref->dado.descricao, r->ref->dado.energia, r->ref->dado.proteina);
    imprime_range_rec(r->esq, min, max);
}

static void solta_arv(NodoArv *r)
{
    if (!r) return;
    solta_arv(r->esq);
    solta_arv(r->dir);
    free(r);
}

static int conta_cats(NodoCat *h)
{
    int c = 0; NodoCat *t = h;
    while (t) { c++; t = t->prox; }
    return c;
}

static int pos_cat(NodoCat *cab, NodoCat *alvo)
{
    int p = 0, ach = -1; NodoCat *it = cab;
    while (it && ach == -1) { if (it == alvo) ach = p; else { p++; it = it->prox; } }
    return ach;
}

static void solta_pares(ParArv **pairs, int q)
{
    if (!pairs) return;
    int i = 0;
    while (i < q) {
        if (pairs[i]) {
            if (pairs[i]->porEnergia) solta_arv(pairs[i]->porEnergia);
            if (pairs[i]->porProteina) solta_arv(pairs[i]->porProteina);
            free(pairs[i]);
        }
        i++;
    }
    free(pairs);
}

static ParArv **constroi_pares(NodoCat *cab, int *outN)
{
    int n = conta_cats(cab);
    *outN = n;
    if (n == 0) return NULL;
    ParArv **arr = (ParArv**)malloc((size_t)n * sizeof(ParArv*));
    if (!arr) return NULL;
    int k = 0;
    NodoCat *g = cab;
    while (g) { arr[k++] = monta_par(g); g = g->prox; }
    return arr;
}

static void soltar_geral(NodoCat *cab, ParArv **pairs, int q)
{
    solta_pares(pairs, q);
    NodoCat *c = cab;
    while (c) {
        NodoCom *f = c->itens;
        while (f) { NodoCom *nx = f->prox; free(f); f = nx; }
        NodoCat *nxg = c->prox; free(c); c = nxg;
    }
}

static int rebuild_pars(NodoCat *cab, ParArv ***ptr, int *nptr)
{
    if (*ptr != NULL) { solta_pares(*ptr, *nptr); *ptr = NULL; *nptr = 0; }
    ParArv **novo = constroi_pares(cab, nptr);
    *ptr = novo;
    return 1;
}

static int remove_categoria(NodoCat **lista, ParArv ***pars_ptr, int *nCats_ptr, int catId)
{
    int removed = 0;
    if (!lista || !*lista) return removed;
    NodoCat *cur = *lista, *ant = NULL;
    while (cur && !removed) {
        if (cur->id == catId) {
            if (!ant) *lista = cur->prox; else ant->prox = cur->prox;
            NodoCom *it = cur->itens;
            while (it) { NodoCom *nx = it->prox; free(it); it = nx; }
            free(cur);
            removed = 1;
        } else { ant = cur; cur = cur->prox; }
    }
    if (removed) rebuild_pars(*lista, pars_ptr, nCats_ptr);
    return removed;
}

static int remove_alimento(NodoCat *lista, ParArv **pars, int *nCats_ptr, int codigo)
{
    int removed = 0;
    if (!lista) return removed;
    NodoCat *c = lista;
    while (c && !removed) {
        NodoCom *it = c->itens, *ant = NULL;
        while (it && !removed) {
            if (it->dado.codigo == codigo) {
                if (!ant) c->itens = it->prox; else ant->prox = it->prox;
                free(it);
                int pos = pos_cat(lista, c);
                if (pos >= 0 && pos < *nCats_ptr && pars) {
                    if (pars[pos]) {
                        if (pars[pos]->porEnergia) solta_arv(pars[pos]->porEnergia);
                        if (pars[pos]->porProteina) solta_arv(pars[pos]->porProteina);
                        free(pars[pos]);
                    }
                    pars[pos] = monta_par(c);
                }
                removed = 1;
            } else { ant = it; it = it->prox; }
        }
        c = c->prox;
    }
    return removed;
}

static int salva_bin(const char *path, NodoCat *lista)
{
    int total = 0;
    NodoCat *c = lista;
    while (c) { NodoCom *it = c->itens; while (it) { total++; it = it->prox; } c = c->prox; }
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    if (fwrite(&total, sizeof(int), 1, f) != 1) { fclose(f); return 0; }
    if (total == 0) { fclose(f); return 1; }
    Comidinha *arr = (Comidinha*)malloc((size_t)total * sizeof(Comidinha));
    if (!arr) { fclose(f); return 0; }
    int idx = 0;
    c = lista;
    while (c) {
        NodoCom *it = c->itens;
        while (it) { arr[idx++] = it->dado; it = it->prox; }
        c = c->prox;
    }
    if ((int)fwrite(arr, sizeof(Comidinha), (size_t)total, f) != total) { free(arr); fclose(f); return 0; }
    free(arr);
    fclose(f);
    return 1;
}

static int modificado = 0;

int main(void)
{
    Comidinha *arr = NULL; int total = 0;
    int ok = ler_bin("dados.bin", &arr, &total);
    if (ok != 1) { printf("ERRO\n"); return 0; }
    NodoCat *lista = NULL;
    int i = 0;
    while (i < total) {
        if (arr[i].categoria >= CEREAIS && arr[i].categoria <= NOZES_SEMENTES) joga_na_cat(&lista, &arr[i]);
        i++;
    }
    free(arr);
    int nCats = conta_cats(lista);
    ParArv **pars = NULL;
    if (nCats > 0) {
        pars = (ParArv**)malloc((size_t)nCats * sizeof(ParArv*));
        if (!pars) { printf("ERRO\n"); soltar_geral(lista, NULL, 0); return 0; }
        int z = 0; while (z < nCats) { pars[z] = NULL; z++; }
    }
    int k = 0; NodoCat *g = lista;
    while (g) { pars[k] = monta_par(g); k++; g = g->prox; }
    int opt = -1;
    int running = 1;
    while (running) {
        printf("1 Listar categorias\n");
        printf("2 Listar itens de uma categoria em ordem alfabética\n");
        printf("3 Listar por energia\n");
        printf("4 Listar por proteína\n");
        printf("5 Listar por energia mediate a valores informados\n");
        printf("6 Listar por proteína mediante a valores informados\n");
        printf("7 Remover categoria\n");
        printf("8 Remover alimento\n");
        printf("9 Salvar em dados.bin\n");
        printf("0 Sair\n");
        int read = scanf("%d", &opt);
        if (read != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) ;
            printf("Inv\n");
        } else {
            if (opt == 0) {
                if (modificado) {
                    if (salva_bin("dados.bin", lista)) printf("OK\n"); else printf("ERRO\n");
                }
                running = 0;
            } else if (opt == 1) {
                NodoCat *t = lista;
                while (t) { printf("%d %s\n", t->id, NOMES_CAT[t->id]); t = t->prox; }
            } else if (opt == 2) {
                int cid; printf("Digite id da categoria (0..14): ");
                int r = scanf("%d", &cid);
                if (r != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("Inv\n"); }
                    else {
                        NodoCat *c = acha_cat(lista,cid);
                        if (c) { NodoCom *it = c->itens; while (it) { printf("%d %s %.0f %.1f\n", it->dado.codigo, it->dado.descricao, it->dado.energia, it->dado.proteina); it = it->prox; } }
                    }
                }
            } else if (opt == 3) {
                int cid; printf("Digite id da categoria (0..14): ");
                int r = scanf("%d", &cid);
                if (r != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("Inv\n"); }
                    else {
                        NodoCat *c = acha_cat(lista,cid);
                        int pos = pos_cat(lista,c);
                        if (pos >= 0 && pos < nCats && pars && pars[pos] && pars[pos]->porEnergia) print_dec(pars[pos]->porEnergia);
                    }
                }
            } else if (opt == 4) {
                int cid; printf("Digite id da categoria (0..14): ");
                int r = scanf("%d", &cid);
                if (r != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("Inv\n"); }
                    else {
                        NodoCat *c = acha_cat(lista,cid);
                        int pos = pos_cat(lista,c);
                        if (pos >= 0 && pos < nCats && pars && pars[pos] && pars[pos]->porProteina) print_dec(pars[pos]->porProteina);
                    }
                }
            } else if (opt == 5) {
                int cid; float a,b; printf("Digite: id min max -> ");
                int r = scanf("%d %f %f",&cid,&a,&b);
                if (r != 3) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("Inv\n"); }
                    else {
                        NodoCat *c = acha_cat(lista,cid);
                        int pos = pos_cat(lista,c);
                        if (pos >= 0 && pos < nCats && pars && pars[pos] && pars[pos]->porEnergia) {
                            if (a>b) { float t=a; a=b; b=t; }
                            imprime_range_rec(pars[pos]->porEnergia, a, b);
                        }
                    }
                }
            } else if (opt == 6) {
                int cid; float a,b; printf("Digite: id min max -> ");
                int r = scanf("%d %f %f",&cid,&a,&b);
                if (r != 3) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("Inv\n"); }
                    else {
                        NodoCat *c = acha_cat(lista,cid);
                        int pos = pos_cat(lista,c);
                        if (pos >= 0 && pos < nCats && pars && pars[pos] && pars[pos]->porProteina) {
                            if (a>b) { float t=a; a=b; b=t; }
                            imprime_range_rec(pars[pos]->porProteina, a, b);
                        }
                    }
                }
            } else if (opt == 7) {
                printf("Digite id para remover (0..14): ");
                int cid; int r = scanf("%d",&cid);
                if (r != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (cid < 0 || cid > 14) { printf("NF\n"); }
                    else {
                        if (remove_categoria(&lista, &pars, &nCats, cid)) { printf("OK\n"); modificado = 1; }
                        else printf("NF\n");
                    }
                }
            } else if (opt == 8) {
                printf("Digite código do alimento: ");
                int code; int r = scanf("%d",&code);
                if (r != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) ; printf("Inv\n"); }
                else {
                    if (remove_alimento(lista, pars, &nCats, code)) { printf("OK\n"); modificado = 1; }
                    else printf("NF\n");
                }
            } else if (opt == 9) {
                if (salva_bin("dados.bin", lista)) printf("OK\n"); else printf("ERRO\n");
            } else {
                printf("Inv\n");
            }
        }
    }

    soltar_geral(lista, pars, nCats);
    return 0;
}


