/*
    Fichier:    photon.c
    Auteur:     Alix Nepveux & Sven Borden
    Date :      20 mai 2016
    Version:    1.1
    Description:Module photon qui gere la structure photon
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "absorbeur.h"
#include "constantes.h"
#include "error.h"
#include "graphic.h"
#include "reflecteur.h"
#include "utilitaire.h"
#include "photon.h"

#define OK      1
#define NO     	0
#define NB_ELEM 3

typedef struct Photon PHOTON;

struct Photon
{
	POINT pos;	/*position du photon*/
	double alpha;	/*angle du photon*/
	unsigned int id;
	PHOTON * next;	/*Pointeur sur le suivant*/
};

static int n = 0;
static unsigned int lastId = 0;
static PHOTON * list = NULL;

void check(PHOTON *, VECTOR);

int photonSet(char line[MAX_LINE])
{
	POINT _pos; double _alpha;
	if(sscanf(line, "%lf %lf %lf", 
		&_pos.x, &_pos.y, &_alpha) != NB_ELEM)
	{
		error_lecture_elements(ERR_PHOTON, ERR_PAS_ASSEZ);
		return NO;
	}

	addPhoton(_pos, _alpha);
	return OK;
}

int addPhoton(POINT _pt, double _alpha)
{
	/*Crée un nouveau photon*/
	PHOTON *p = (PHOTON *) malloc(sizeof(PHOTON));
	if(p != NULL)
	{
		p->pos.x = _pt.x;
		p->pos.y = _pt.y;
		p->alpha = _alpha;
		p->id = lastId;
		if(list != NULL)
			p->next = list;
		else
			p->next = NULL;
		list = p;
		n++;
		lastId++;
	}
	else
	{
		free(p);
		p = NULL;
	}
	return (p != NULL);//retourne 1 (vrai) si est différent de NULL
} 

int delPhoton(int _id)
{
	PHOTON *p = list;
	
	if(p)
	{
		if(p->id == _id)
		{
			list = list->next;
			free(p);
		}
		else
		{
			PHOTON *prec = p;
			while(prec->next && prec->next->id != _id)
				prec = prec->next;
			if(prec->next)
			{
				PHOTON * del = prec->next;
				prec->next = prec->next->next;
				free(del);
				del = NULL;
			}
		}
	}
	n--;
	return OK;
}

void writePhoton(FILE * file)
{
	PHOTON *p = list;
	fprintf(file, "#photon\n%d\n", n);
	while(p != NULL)
	{
		fprintf(file, "%lf %lf %lf\n", p->pos.x, p->pos.y, p->alpha);
		p = p->next;
	}
	fprintf(file, "FIN_LISTE\n\n");
}

void drawPhot(void)
{
	PHOTON *p = list;
	while(p != NULL)
	{
		graphicDrawPhoton(p->pos, p->alpha);
		p = p->next;
	}
}

void printListPhoton(void)
{
	PHOTON *p = list;
	int i = 1;
	while(p)
	{
		printf("Photon %d\tX = %lf\t Y = %lf\tAlpha = %lf\n", 
			i, p->pos.x, p->pos.y, p->alpha);
		p = p->next;
		i++;
	}
	return ;
}

void delOutside(double xmin, double xmax, double ymin, double ymax)
{
	PHOTON *p = list;
	while(p)
	{
		if(p->pos.x < xmin || p->pos.x > xmax
			||p->pos.y < ymin || p->pos.y > ymax)
			delPhoton(p->id);
			p = p->next;
	}
}

void delListPhoton(void)
{
	n = 0;
	while(list)
	{
		PHOTON *p = list;
		list = p->next;
		free(p);
	}
	return ;
}

int nbPhot(void) { return n; }

void reflection(VECTOR v, PHOTON *p)
{
	double vx = v.ptFin.x - v.ptDeb.x;
	double vy = v.ptFin.y - v.ptDeb.y;
	double normev = utilitaireNormeVector(v);
	double uvx = vx/normev;
	double uvy = vy/normev;
	VECTOR r = getReflecteur(getLastId());
	POINT * tmp = utilitaireIntersection(r, v);
	POINT ptTmp; ptTmp.x = tmp->x; ptTmp.y = tmp->y;
	double normeL1 = utilitaireDistance2Points(v.ptDeb, ptTmp);
	double normeL2 = normev - normeL1;
	double rx = r.ptFin.x - r.ptDeb.x;
	double ry = r.ptFin.y - r.ptDeb.y;
	double normer = utilitaireNormeVector(r);
	double nx1, ny1;
	double nx2, ny2;
	//vecteurs normaux 
	nx1 = -ry/normer;	ny1 = rx/normer;
	nx2 = ry/normer;	ny2 = -rx/normer;
	//on peut avoir deux reflections possible
	double wx, wy;
	if(uvx*nx1+uvy*ny1 > 0.)
	{
		nx1 = nx2;	ny1 = ny2;
	}
	wx = normeL2*(uvx-2*(uvx*nx1+uvy*ny1)*nx1);
	wy = normeL2*(uvy-2*(uvx*nx1+uvy*ny1)*ny1);
	
	v.ptDeb.x = ptTmp.x;	v.ptDeb.y = ptTmp.y;
	v.ptFin.x = v.ptDeb.x + wx;
	v.ptFin.y = v.ptDeb.y + wy;	
	p->alpha = atan2(wy, wx);
	p->pos.x = v.ptFin.x;
	p->pos.y = v.ptFin.y;
}

void check(PHOTON *p, VECTOR v)
{
	POINT *prefl = NULL, *pabso = NULL;
	POINT refl, abso;
	prefl = reflProche(v);
	pabso = absoProche(v);
	if(prefl == NULL && pabso == NULL)//pas d'intersection
	{
		p->pos.x = v.ptFin.x;
		p->pos.y = v.ptFin.y;
		return;
	}
	if(prefl == NULL)//seulement un abso
	{
		delPhoton(p->id);
		return;
	}
	if(pabso == NULL)//seulement refl
	{
		reflection(v, p);
		return;
	}
	//les deux
	refl.x = prefl->x;
	refl.y = prefl->y;
	abso.x = pabso->x;
	abso.y = pabso->y;
	
	double dRefl = fabs(utilitaireDistance2Points(refl, v.ptDeb));
	double dAbso = fabs(utilitaireDistance2Points(abso, v.ptDeb));

	dRefl < dAbso ? reflection(v, p) : delPhoton(p->id);
	
	return;
}

void updatePhoton()
{
	PHOTON *p = list;
	double futureX = 0;
	double futureY = 0;/*les deux futures positions*/
	VECTOR v;
	while(p)
	{
		futureX = p->pos.x + cos(p->alpha)*DELTA_T*VPHOT;
		futureY = p->pos.y + sin(p->alpha)*DELTA_T*VPHOT;
		v.ptDeb.x = p->pos.x;
		v.ptDeb.y = p->pos.y;
		v.ptFin.x = futureX;
		v.ptFin.y = futureY;
		check(p, v);
		resetLastReflect();
		p = p->next;
	}
}
