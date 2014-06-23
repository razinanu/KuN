
allgemeines
-----------
- zum kompilieren der vorlage ist opencv nötig (paket libopencv-dev)
- wenn opencv nicht installiert werden kann, im Makefile bei CFLAGS das -DUSE_OPENCV entfernen
  und -lopencv... aus den linker-kommandos entfernen
  (dann wird aber auch das kamerabild nicht angezeigt)

v-rep (simulation)
------------------
- auf den laborrechnern ist v-rep schon installiert (/opt/V-REP_PRO_EDU_...)
- ansonsten vrep installieren (z.B. unter Linux entpacken in /home/<user>/opt)
- im Makefile anpassen: VREP_FOLDER
  z.B. VREP_FOLDER=/home/<user>/opt/V-REP_PRO_EDU_V3_0_5c_64_Linux
- make

- make startsim führt simulation aus

- ./server-vrep 5555                # startet das programm für den roboter
- ./client-keyboard localhost 5555  # startet das programm für die tastatursteuerung

hinweis: nutzer braucht schreibrechte für die vrep-installation (am besten im home installieren)

vbot (echter roboter)
---------------------
- make server-vbot
- server-vbot per ssh auf roboter schieben und starten
  scp ./server-vbot student@10.1.2.51:
  ssh student@10.1.2.51
- ./client-keyboard <10.1.2.51> <port>
