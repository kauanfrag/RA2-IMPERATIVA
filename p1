#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_DESC 150

enum Categoria {
 CEREAIS=0, VERDURAS, FRUTAS, GORDURAS, PESCADOS,
 CARNES, LEITE, BEBIDAS, OVOS, ACUCARADOS, MISCELANEAS,
 CATEGORIA_INVALID
};

struct Alimento {
 int codigo;
 char descricao[TAM_DESC];
 float energia;
 float proteina;
 int categoria;
};

static void remove_nl(char *s)
{
 size_t L = strlen(s);
 while (L>0) {
  char c = s[L-1];
  if (c=='\n' || c=='\r') { s[L-1]='\0'; L--; }
  else { break; }
 }
}

static float tofloat_or_minus(const char *s)
{
 float out = -1.0f;
 if (s==NULL) return out;
 while (*s==' ') s++;
 if (*s=='\0') return out;
 char *e = NULL;
 out = strtof(s,&e);
 if (e==s) out = -1.0f;
 return out;
}

static int csv_to_bin_file(const char *csv, const char *bin)
{
 int ok = 0;
 FILE *f = fopen(csv,"r");
 if (f==NULL) return ok;
 FILE *o = fopen(bin,"wb");
 if (o==NULL) { fclose(f); return ok; }
 char line[512];
 if (fgets(line,sizeof(line),f)==NULL) { fclose(f); fclose(o); return ok; }
 int cnt = 0;
 if (fwrite(&cnt,sizeof(int),1,o) != 1) { fclose(f); fclose(o); return ok; }
 int read_more = 1;
 while (read_more==1) {
  char *r = fgets(line,sizeof(line),f);
  if (r==NULL) read_more = 0;
  if (read_more==1) {
   struct Alimento a;
   char *p = line;
   char *t;
   t = strtok(p,",");
   if (t!=NULL) {
    a.codigo = atoi(t);
    t = strtok(NULL,",");
    if (t!=NULL) {
     strncpy(a.descricao,t,TAM_DESC-1);
     a.descricao[TAM_DESC-1]='\0';
     remove_nl(a.descricao);
     t = strtok(NULL,",");
     t = strtok(NULL,",");
     a.energia = tofloat_or_minus(t);
     t = strtok(NULL,",");
     a.proteina = tofloat_or_minus(t);
     t = strtok(NULL,",");
     t = strtok(NULL,",");
     if (t!=NULL) {
      int cid = atoi(t);
      if (cid < CEREAIS || cid > MISCELANEAS) a.categoria = CATEGORIA_INVALID;
      else a.categoria = cid;
     } else { a.categoria = CATEGORIA_INVALID; }
     if (fwrite(&a,sizeof(struct Alimento),1,o) == 1) cnt++;
    }
   }
  }
 }
 if (fseek(o,0,SEEK_SET) == 0) { fwrite(&cnt,sizeof(int),1,o); }
 fclose(f);
 fclose(o);
 ok = 1;
 return ok;
}

int main(void)
{
 int ok = csv_to_bin_file("alimentos.csv","dados.bin");
 if (ok==1) printf("OK\n"); else printf("ERRO\n");
 return 0;
}
