#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_DESC 150

enum Categoria {
    CEREAIS=0, VERDURAS, FRUTAS, GORDURAS, PESCADOS,
    CARNES, LEITE, BEBIDAS, OVOS, ACUCARADOS, MISCELANEAS,
    CATEGORIA_INVALID
};

typedef struct Alimento {
    int codigo;
    char descricao[TAM_DESC];
    float energia;
    float proteina;
    int categoria;
} Alimento;

typedef struct NoItem {
    Alimento info;
    struct NoItem *prox;
} NoItem;

typedef struct NoCat {
    int id;
    NoItem *itens;
    struct NoCat *prox;
} NoCat;

typedef struct NoArv {
    float chave;
    NoItem *ref;
    struct NoArv *esq;
    struct NoArv *dir;
} NoArv;

typedef struct DuoArv {
    NoArv *porEnergia;
    NoArv *porProteina;
} DuoArv;

static const char *NOMES_CAT[] = {
    "Cereais","Verduras","Frutas","Gorduras","Pescados",
    "Carnes","Leite","Bebidas","Ovos","Produtos acucarados","Miscelaneas"
};

static int cmp_ci(const char *a, const char *b) {
    size_t i = 0;
    int r = 0;
    while (r == 0) {
        unsigned char ca = (unsigned char)a[i], cb = (unsigned char)b[i];
        if (ca == '\0' && cb == '\0') r = 0;
        else if (ca == '\0') r = -1;
        else if (cb == '\0') r = 1;
        else {
            char la = (char)tolower(ca), lb = (char)tolower(cb);
            if (la < lb) r = -1;
            else if (la > lb) r = 1;
            else i++;
        }
        if (r != 0 || (ca == '\0' && cb == '\0')) break;
    }
    return r;
}

static int ler_bin(const char *path, Alimento **out, int *qt) {
    int ok = 0, n = 0;
    FILE *f = fopen(path, "rb");
    if (f == NULL) return ok;
    if (fread(&n, sizeof(int), 1, f) != 1) { fclose(f); return ok; }
    if (n <= 0) { *out = NULL; *qt = 0; fclose(f); ok = 1; return ok; }
    Alimento *arr = (Alimento*)malloc((size_t)n * sizeof(Alimento));
    if (arr == NULL) { fclose(f); return ok; }
    size_t g = fread(arr, sizeof(Alimento), (size_t)n, f);
    if ((int)g != n) { free(arr); fclose(f); return ok; }
    fclose(f);
    *out = arr; *qt = n; ok = 1; return ok;
}

static NoCat* novo_cat(int id) {
    NoCat *n = (NoCat*)calloc(1, sizeof(NoCat));
    if (n != NULL) { n->id = id; n->itens = NULL; n->prox = NULL; }
    return n;
}

static NoItem* novo_item(const Alimento *a) {
    NoItem *n = (NoItem*)malloc(sizeof(NoItem));
    if (n != NULL) { n->info = *a; n->prox = NULL; }
    return n;
}

static void inserir_item_alf(NoItem **cab, NoItem *novo) {
    NoItem *cur = *cab, *ant = NULL; int feito = 0;
    while (feito == 0) {
        if (cur == NULL) {
            if (ant == NULL) *cab = novo; else ant->prox = novo;
            feito = 1;
        } else {
            int c = cmp_ci(novo->info.descricao, cur->info.descricao);
            if (c <= 0) {
                if (ant == NULL) { novo->prox = *cab; *cab = novo; }
                else { novo->prox = ant->prox; ant->prox = novo; }
                feito = 1;
            } else { ant = cur; cur = cur->prox; }
        }
    }
}

static void inserir_cat_alf(NoCat **cab, NoCat *novo) {
    NoCat *cur = *cab, *ant = NULL; int feito = 0;
    while (feito == 0) {
        if (cur == NULL) {
            if (ant == NULL) *cab = novo; else ant->prox = novo;
            feito = 1;
        } else {
            int c = strcmp(NOMES_CAT[novo->id], NOMES_CAT[cur->id]);
            if (c <= 0) {
                if (ant == NULL) { novo->prox = *cab; *cab = novo; }
                else { novo->prox = ant->prox; ant->prox = novo; }
                feito = 1;
            } else { ant = cur; cur = cur->prox; }
        }
    }
}

static NoCat* achar_cat(NoCat *cab, int id) {
    NoCat *cur = cab, *res = NULL;
    while (cur != NULL && res == NULL) {
        if (cur->id == id) res = cur; else cur = cur->prox;
    }
    return res;
}

static void inserir_item_por_cat(NoCat **cab, const Alimento *a) {
    if (a == NULL) return;
    NoCat *c = achar_cat(*cab, a->categoria);
    if (c == NULL) {
        NoCat *nc = novo_cat(a->categoria);
        if (nc == NULL) return;
        inserir_cat_alf(cab, nc);
        c = achar_cat(*cab, a->categoria);
        if (c == NULL) return;
    }
    NoItem *ni = novo_item(a);
    if (ni == NULL) return;
    inserir_item_alf(&c->itens, ni);
}

static NoArv* novo_no(float chave, NoItem *ref) {
    NoArv *n = (NoArv*)malloc(sizeof(NoArv));
    if (n != NULL) { n->chave = chave; n->ref = ref; n->esq = NULL; n->dir = NULL; }
    return n;
}

static NoArv* inserir_bst(NoArv *raiz, float chave, NoItem *ref) {
    if (raiz == NULL) return novo_no(chave, ref);
    NoArv *cur = raiz; int ok = 0;
    while (ok == 0) {
        if (chave < cur->chave) {
            if (cur->esq == NULL) { cur->esq = novo_no(chave, ref); ok = 1; } else cur = cur->esq;
        } else if (chave > cur->chave) {
            if (cur->dir == NULL) { cur->dir = novo_no(chave, ref); ok = 1; } else cur = cur->dir;
        } else {
            int c = strcmp(ref->info.descricao, cur->ref->info.descricao);
            if (c <= 0) {
                if (cur->esq == NULL) { cur->esq = novo_no(chave, ref); ok = 1; } else cur = cur->esq;
            } else {
                if (cur->dir == NULL) { cur->dir = novo_no(chave, ref); ok = 1; } else cur = cur->dir;
            }
        }
    }
    return raiz;
}

static DuoArv* montar_duo(NoCat *cat) {
    DuoArv *p = (DuoArv*)calloc(1, sizeof(DuoArv));
    if (p == NULL) return NULL;
    NoItem *f = cat->itens;
    while (f != NULL) {
        p->porEnergia   = inserir_bst(p->porEnergia,   f->info.energia,  f);
        p->porProteina  = inserir_bst(p->porProteina,  f->info.proteina, f);
        f = f->prox;
    }
    return p;
}

static void print_dec(NoArv *r) {
    if (r == NULL) return;
    print_dec(r->dir);
    printf("%d %s %.0f %.1f\n", r->ref->info.codigo, r->ref->info.descricao, r->ref->info.energia, r->ref->info.proteina);
    print_dec(r->esq);
}

static void soltar_arv(NoArv *r) {
    if (r == NULL) return;
    soltar_arv(r->esq);
    soltar_arv(r->dir);
    free(r);
}

static void ver_categorias(NoCat *cab) {
    NoCat *c = cab;
    while (c != NULL) { printf("%d %s\n", c->id, NOMES_CAT[c->id]); c = c->prox; }
}

static void ver_itens(NoCat *c) {
    if (c == NULL) return;
    NoItem *f = c->itens;
    while (f != NULL) {
        printf("%d %s %.0f %.1f\n", f->info.codigo, f->info.descricao, f->info.energia, f->info.proteina);
        f = f->prox;
    }
}

static int contar_cats(NoCat *cab) {
    int n = 0; NoCat *t = cab;
    while (t != NULL) { n++; t = t->prox; }
    return n;
}

static int pos_cat(NoCat *cab, NoCat *alvo) {
    int p = 0, ach = -1; NoCat *it = cab;
    while (it != NULL && ach == -1) { if (it == alvo) ach = p; else { p++; it = it->prox; } }
    return ach;
}

static void soltar_geral(NoCat *cab, DuoArv **pairs, int q) {
    if (pairs != NULL) {
        int i = 0;
        while (i < q) {
            if (pairs[i] != NULL) {
                if (pairs[i]->porEnergia != NULL)  soltar_arv(pairs[i]->porEnergia);
                if (pairs[i]->porProteina != NULL) soltar_arv(pairs[i]->porProteina);
                free(pairs[i]);
            }
            i++;
        }
        free(pairs);
    }
    NoCat *c = cab;
    while (c != NULL) {
        NoItem *f = c->itens;
        while (f != NULL) { NoItem *nx = f->prox; free(f); f = nx; }
        NoCat *nxg = c->prox; free(c); c = nxg;
    }
}

int main(void) {
    Alimento *arr = NULL; int total = 0;
    int ok = ler_bin("dados.bin", &arr, &total);
    if (ok != 1) { printf("Falha ao ler dados.bin\n"); return 0; }

    NoCat *listaCats = NULL;
    int i = 0;
    while (i < total) {
        if (arr[i].categoria >= CEREAIS && arr[i].categoria <= MISCELANEAS)
            inserir_item_por_cat(&listaCats, &arr[i]);
        i++;
    }
    free(arr);

    int nCats = contar_cats(listaCats);
    DuoArv **duos = NULL;
    if (nCats > 0) {
        duos = (DuoArv**)malloc((size_t)nCats * sizeof(DuoArv*));
        if (duos == NULL) { printf("Memória insuficiente\n"); soltar_geral(listaCats, NULL, 0); return 0; }
    }

    int k = 0; NoCat *g = listaCats;
    while (g != NULL) { duos[k] = montar_duo(g); k++; g = g->prox; }

    int escolha = -1; int loop = 1;
    while (loop == 1) {
        printf("\nMenu:\n");
        printf("1 - Ver categorias\n");
        printf("2 - Itens por categoria (alfabético)\n");
        printf("3 - Itens por energia (decrescente)\n");
        printf("4 - Itens por proteína (decrescente)\n");
        printf("0 - Sair\n");
        if (scanf("%d", &escolha) != 1) loop = 0;
        else {
            if (escolha == 0) loop = 0;
            else if (escolha == 1) {
                ver_categorias(listaCats);
            } else if (escolha == 2) {
                int cid = -1; printf("Categoria id: ");
                if (scanf("%d", &cid) == 1) {
                    NoCat *cat = achar_cat(listaCats, cid);
                    if (cat != NULL) ver_itens(cat);
                }
            } else if (escolha == 3) {
                int cid = -1; printf("Categoria id: ");
                if (scanf("%d", &cid) == 1) {
                    NoCat *cat = achar_cat(listaCats, cid);
                    int p = pos_cat(listaCats, cat);
                    if (p >= 0 && p < nCats && duos[p] != NULL) print_dec(duos[p]->porEnergia);
                }
            } else if (escolha == 4) {
                int cid = -1; printf("Categoria id: ");
                if (scanf("%d", &cid) == 1) {
                    NoCat *cat = achar_cat(listaCats, cid);
                    int p = pos_cat(listaCats, cat);
                    if (p >= 0 && p < nCats && duos[p] != NULL) print_dec(duos[p]->porProteina);
                }
            } else {
            }
        }
    }

    soltar_geral(listaCats, duos, nCats);
    return 0;
}

