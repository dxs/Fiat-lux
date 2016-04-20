/*
    Fichier:    reflecteur.h
    Auteur:     Alix Nepveux & Sven Borden
    Date :      16 mars 2016
    Version:    1.0
    Description:Module reflecteur qui gere la structure 
*/

#ifndef REFLECTEUR_H
#define REFLECTEUR_H

#include <stdio.h>
#include "utilitaire.h"
//verifie les valeurs et les enregistre dans la structure
int reflecteurSet(char[]);/*ajoute un photon a partir d'une ligne*/
int addReflecteur(POINT, POINT);/*ajoute un photon dans la liste*/
int delReflecteur(int);/*supprime un photon de la liste*/
void writeReflecteur(FILE *);
void drawRefl(void);
void printListReflecteur(void);/*imprime la liste des reflecteurs*/
void delListReflecteur(void);/*nettoir la memoire en supprimant refle*/
int nbRefl(void);
#endif
