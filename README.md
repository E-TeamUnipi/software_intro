# Introduction to E-Team Embedded Software

## Descrizione
Il codice di esempio mostra dei firmware esemplificativi per la gestione
di acquisizione dati, invio e ricezione di messaggi sul CAN bus, scheduling
di task periodici.

Lo scenario di simulazione, descritto nelle slides, prevedere segnale
di acceleratore costante al 100% per 30 secondi, poi portato a zero.
Con la ventola disattivata, il sistema raggiunge in questo tempo la
temperatura di inverter di 91 gradi, partendo da 40 di temperatura ambiente.
A ventole sempre attiva alla velocità massima, lo stesso sistema arriverebbe
a regime a 61 gradi circa. Solo la temperatura dell'acqua di raffreddamento
è misurabile, ma non quella dell'inverter.

## Istruzioni per l'esecuzione

### Compilazione
```
meson compile -C build
```

### Creazione del virtual environment
```sh-session
cd bus
python -m venv venv
. venv/bin/activate
pip install -r requirements.txt
```

### (Ri)generazione dei bindings
```sh-session
cd bus
python generate.py
```

### Attivazione delle interfacce CAN virtuali
```
sudo ip link add vcan type vcan
sudo ip link add vcansim type vcan
sudo ip link set vcan up
sudo ip link set vcansim up
```
`vcansim` viene usato dal simulatore e dagli `hal` per lo scambio di messaggi
che normalmente corrispondono a segnali analogici letti dalle schede.

### Monitoring delle interfacce CAN
```sh-session
cd bus
. venv/bin/activate
python -m cantools monitor -c vcan dbc/main.dbc
python -m cantools monitor -c vcansim dbc/sim.dbc # in un altro terminale
```
### Esecuzione
Ogni firmware virtuale va eseguito in un proprio terminale:
```sh-session
./build/sim
./build/sb1
./build/sb2
./build/fan_ctrl
./build/vcu
```
## Struttura del codice
### Organizzazione
Ogni progetto è diviso in una parte portable e una parte architecture-dependent.
Ad esempio, `sb/sb.h`, `sb/sb.c`, `sb/hal.h` sono le parti portable della sensorboard,
mentre `sb/main.c`, `sb/hal.c` sono le parti che cambiano in base all'ambiente utilizzato.
In questo caso, `sb/hal.c` simula con un timer linux e socketcan la ricezione dei messaggi
CAN, la esecuzione periodica di un timer che gestisce i task periodici e la gestione di
messaggi analogici.
Su hardware queste funzioni si interfaccerebbero direttamente al CAN controller, ai timer
nativi del microcontrollore e alle varie periferiche.

È comunque utile guardare il `main.c` perché mostra la struttura di esecuzione che è identica
anche in un microcontrollore.

### SB
Il codice della sensorboard è pensato per essere identico per schede che avranno diversa configurazione hardware.
Attraverso `hal_board_id()` il firmware capisce quale configurazione sia stata scelta (in hardware questo viene
fatto attraverso dei jumper).

### Identificativi dei messaggi
Per convenzione interna, i messaggi CAN riservano una parte dei bit meno significativi all'identificativo della scheda
che invia il messaggio; per questo si vedono scritture del tipo:
```
hal_send_can_message(
    MAIN_IQ_CMD_FRAME_ID | MAIN_NODE_VCU,
```
In quanto `MAIN_IQ_CMD_FRAME_ID` contiene solo i bit relativi al messaggio e la parte del sender nulla; si potrebbe
discutere se questa convenzione abbia ancora senso e nel caso eliminarla.

## Esercitazione
Mancano due parti fondamentali del codice:
1) La lettura della temperatura e l'invio del relativo messaggio sulla sensorboard
   (funzioni `send_temp` e `handle_temp_sensor`)
2) Controllo della temperatura dell'inverter attraverso il controllo della ventola
   (funzione `temp_control_loop`)

La temperatura dell'inverter non è accessibile, ma è monitorabile dal `cansim` ed è
collegata alla temperatura dell'acqua misurata attraverso il modello termico.
Si consiglia di partire dalla lettura di `hal.h`, `sb.h` e `vcu.h`

### Misura della temperatura
Si suppone di utilizzare un sensore di tipo `a` e un partitore con resistenza di 1k.
Chiedere a un elettronico la relazione tra la temperatura e la tensione misurata, che
andrà poi invertita per ottenere la temperatura da inviare sul CAN.

### Controllo della temperatura
In prima battuta provare un controllo con ventola sempre accesa, ma un proporzionale
sarebbe migliore (la potenza dissipata dalla ventola cresce con il cubo della velocità).

## Segnali safety critical
Se si prova a terminare `sb1` mentre gli altri firmware funzionano, si noterà
che la VCU continua richiedere una corrente all'inverter pari a quella dell'ultimo
segnale di PPS letto. Come si potrebbe rendere safe questo codice?
Un discorso simile si potrebbe fare per il controllo di temperatura.

## Note di stile
### Uso degli interi nella programmazione embedded
Nella programmazione in C/C++ convenzionale è abbastanza comune
usare il tipo int/unsigned int per rappresentare gli interi.
Nella programmazione embedded, conoscere la grandezza dei tipi che
si usa è fondamentale per ragioni di correttezza e spazio.
Il tipo int non ha una grandezza definita da standard; ad esempio
su architettura AVR è di 2 byte, su ARM è di 4 byte, etc (questo
renderebbe anche lo stesso codice non molto portabile tra architetture
diverse). In generale è meglio utilizzare i tipi definiti in `<stdint.h>`,
e utilizzare `size_t` se servisse un unsigned int di grandezza nativa
dell'architettura.

### Utilizzo di float
I float in ambito embedded hanno come vantaggio di permettere calcoli a precisione
abbastanza alta, ma nelle architetture senza FPU (ad esempio gli atmel AVR) le
operazioni floating point vengono eseguite con emulazione software, impattando sulle
performance.
Bisogna quindi fare attenzione a usare i float solo quando necessario. In alternativa,
spesso si può rapprsentare una grandezza di e.g. 0.5V come 500mV e lavorare con interi
alla precisione che serve, tenendo a mente che le divisioni tra interi faranno perdere
precisione.
Una via intermedia consiste nell'usare la rappresentazione fixed-point, per cui esistono
varie librerie.
Il Cortex-M4 offre accelerazione sia per i fixed point che per i floating point
a singola precisione.

Nella scrittura dei float literal, è da preferire la forma `0.123f` a `0.123`, in quanto
la seconda viene interpretata come `double` dal compilatore e vengono implicitamente aggiunti
dei cast.

### Volatile
È abbastanza comune usare variabili globali confinate al singolo file (quindi `static`) per il
passaggio di valori dall'interrupt handler (ad esempio valori ricevuti da un messaggio CAN)
a codice che effettua i calcoli. Le variabile di questo tipo (che non siano puntatori/array)
vanno dichiarate come volatile, altrimenti il compilatore, non vedendo possibili scritture
alla variabile nel control-flow che parte dal main, assumerà che queste rimangano costanti
dentro le routine di calcolo anche se vengono in realtà scritte "esternamente".

### encode/decode dei campi dei CAN bindings
La libreria cantools genera per ogni segnale del messaggio CAN una coppia di funzioni con il
postfisso `encode` e `decode`, che convertono da rappresentazione floating point alla rappresentazione
binaria usata nel messaggio CAN.
Quando il segnale non ha offset e la scala è 1, si può evitare di utilizzarla e assegnare
direttamente il valore dal campo della struttura. Nel dubbio, aggiungere encode/decode
rende il codice corretto in tutti i casi, al costo della conversione (sarebbe da verificare
se il compilatore è abbastanza furbo da accorgersene).

## Miglioramenti ulteriori
È stato trascurato il setting dei filtri dei messaggi CAN, che su una scheda impedisce al CAN controller
di generare un interrupt quando vengono letti messaggi e non sono di interesse della scheda.

La VCU potrebbe generare messaggi di diagnostica (ad esempio in un heartbeat) per rendere più visibili gli
stati interni: se ad esempio il PPS è in stato di fault, comanderà sempre una corrente nulla, ma non sarà
distinguibile dal caso in cui il PPS è effettivamente 0 e in stato valido.
