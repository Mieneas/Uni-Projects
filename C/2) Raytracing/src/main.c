#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "simple_ray/ray_default_scene.h"


int main(int argc, char** argv) {
  /* TODO: check and parse command line arguments */
  int anzahl_prozess = 0;
  int bild_breite = 0;//pixel
  int samples_anzahl = 0;
  
  int i = 0;//für Schleifen
  bmp_Rect *rechteck = malloc(sizeof(bmp_Rect));//Temporary für rechteck.
  
  //Alle Argumente:
  if (argc < 5) {
    if (atoi(argv[1]) >= 1 && atoi(argv[1]) <= 128) {
      anzahl_prozess = atoi(argv[1]);//Anzahle der Prozessoren.
    } else {
      perror("Number of processes can only be from 1-128.\n");
      exit(1);
    }
    
    if (1 <= atoi(argv[2]) && atoi(argv[2]) <= 10000) {
      bild_breite = atoi(argv[2]);//Bildmaßen = bild_breite x bild_breite.
    } else {
      perror("Number of pixels can only be from 1-10000.\n");
      exit(1);
    }
    
    if (atoi(argv[3]) >= 1) {
      samples_anzahl = atoi(argv[3]);//Anzahl der Samples pro Pixel.
    } else {
      perror("Number of samples can only be greater than or equal to 1.\n");
      exit(1);
    }
  } else {

    perror("Number of parameters is not equal to 3.\n");
    exit(1);
  }
  printf("Anzahl der Prozesse: %d\n"
           "Auflösung: %d\n"
           "Samples: %d\n", anzahl_prozess, bild_breite, samples_anzahl);
  
  /* initialize scene */
  ray_Scene *scene = ray_createDefaultScene();
  
  /* TODO: divide work load and create n child processes */
  int bild_bereich[anzahl_prozess];//Array, wo die Maßen des Bildbereiches gespeichert wird.
  int corner[anzahl_prozess];//Array, wo die Koordinaten gespeichert werden.
  pid_t pid = 0;
  pid_t p_pid[anzahl_prozess];//Array, wo die pids gespeichert werden
  pid_t father = getpid();//Speichere id von dem Hauptprozess.
  bmp_Image *kbild = bmp_loadFromData(bild_breite, bild_breite, NULL);//erstelle das gesamte Bild.
  char *p_name = (char *) malloc(sizeof(char) * 8);//prozessname.

  
  //Bildbereich aufteilen:
  int div = bild_breite / anzahl_prozess;
  int mod = bild_breite % anzahl_prozess;
  
  for (i = 0; i < anzahl_prozess; i++) {
    bild_bereich[i] = div;
  }
  for(i = 0; i < mod; i++){
      bild_bereich[i%anzahl_prozess]++;
  }



  //Ecken berechnen:
  int temp = 0;
  for (i = 0; i < anzahl_prozess; i++) {
    corner[i] = temp;
    temp += bild_bereich[i];
  }
  
  //für Prozessnamen:
  char name[9];
  
  //für das generiertes Bild:
  bmp_Image *generiertes_bild;
  
  //um zu erkenen, ob das Bild richtig gespeichert wurde:
  size_t byte_anzahl = 0;

  
  //Prozesse erstellen und Bildbereiche speichern.
  for (i = 0; i < anzahl_prozess; i++) {
    pid = fork();

    if(pid > 0){//father == getpid()){
        p_pid[i] = pid;
    }
    if (father != getppid() && father != getpid()) {
        ray_freeScene(scene);
        free(p_name);
        free(rechteck);
        bmp_free(kbild);
        exit(0);
    }

    if(pid == 0) {
        rechteck->x = 0;
        rechteck->y = corner[i];
        rechteck->w = bild_breite;
        rechteck->h = bild_bereich[i];
    }
  }

    /* TODO: each child process renders it's part of the image and saves it to an individual file;
        the parent process meanwhile waits for children to finish their work
    */
  //father muss warten.
  if (father == getpid()) {
      for(i = 0; i < anzahl_prozess; i++){
          waitpid(p_pid[i], NULL, 0);
      }
  }

  //Nur die Kinder sind erlaubt dieser Block auszuführen.
  if (father != getpid()) {
    //Bildname
    snprintf(name, 9, "%d", getpid());
    //der angegebene Teilbereich generieren.
    generiertes_bild = ray_renderScene(rechteck, bild_breite, bild_breite, samples_anzahl, scene, name);
    //der angegebene generierter Teilbereich speichern und den Speicher dafür freigeben.
    byte_anzahl = bmp_saveToFile(generiertes_bild, strcat(name, ".bmp"));
    bmp_free(generiertes_bild);
    //prüf, ob das Bild richtig gespeichert wurde.
    if (byte_anzahl == 0) {
      perror("image has been not saved.\n");
      exit(1);
    }
    //gebe alle kopierte Daten frei.
      ray_freeScene(scene);
      free(p_name);
      free(rechteck);
      bmp_free(kbild);

    //Prozess wird geendet.
    exit(0);
  }

  /* TODO: parent process loads all files and merges them into a single image */

  //Alle Bildteile sammeln und in kbild speichern.
  for(i = 0; i < anzahl_prozess; i++)
  {
      snprintf(name, 9, "%d", p_pid[i]);

    generiertes_bild = bmp_loadFromFile(strcat(name, ".bmp"));
    bmp_stamp(generiertes_bild, kbild, 0, corner[i]);
    bmp_free(generiertes_bild);
  }
  
  /* TODO: save final image to file "final.bmp" */
  byte_anzahl = bmp_saveToFile(kbild, "final.bmp");
  if(byte_anzahl == 0){
    perror("The image has been not saved\n");
  }
  
  /* free memory */
  ray_freeScene(scene);
  /* TODO: make sure to free all allocated memory */
  free(p_name);
  free(rechteck);
  bmp_free(kbild);
  
  return 0;
}
