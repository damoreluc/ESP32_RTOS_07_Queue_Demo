/*
 * FreeRTOS Esempio 07: queue demo
 *
 * Comunicazione tra due task tramite una coda:
 *   la loop() genera dei dati numerici e li inserisce in una coda;
 *   un task legge i dati dalla coda e li stampa.
 * 
 * Provare a modificare la pausa nella loop come indicato nel commento.
 *
 * Nota: nel file soc.h sono definiti i riferimenti ai due core della ESP32:
 *   #define PRO_CPU_NUM (0)
 *   #define APP_CPU_NUM (1)
 *
 * Qui viene adoperata la APP_CPU
 *
 */

#include <Arduino.h>

// dimensione della coda
static const uint8_t msgQueueLen = 5;

// coda di comunicazione tra task, variabile globale
static QueueHandle_t msg_queue;

// Implementazione dei task
// task che attende un item nella coda e lo stampa
void printTask(void *parameters)
{
  // variabile locale col dato prelevato dalla coda
  int32_t item;

  // stampa sul terminale
  while (1)
  {
    // controlla se è presente un messaggio nella coda
    // e in caso affermativo lo preleva (non bloccante)
    if (xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE)
    {
      Serial.println(item);
    }
    // attendo un certo tempo prima di ripetere
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// configurazione del sistema
void setup()
{
  Serial.begin(115200);

  // breve pausa
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("FreeRTOS: queue demo");

  // creazione della coda
  msg_queue = xQueueCreate(msgQueueLen, sizeof(int32_t));

  // creazione ed avvio dell'unico task
  // DOMANDA: bastano 1024 byte di stack per questo task?
  xTaskCreatePinnedToCore(
      printTask,    // funzione da richiamare nel task
      "Print Task", // nome del task (etichetta utile per debug)
      1500,         // dimensione in byte dello stack per le variabili locali del task (minimo 768 byte)
      NULL,         // puntatore agli eventuali parametri da passare al task
      1,            // priorità del task
      NULL,         // eventuale task handle per gestire il task da un altro task
      APP_CPU_NUM   // core su cui far girare il task
  );
}

void loop()
{
  static int32_t num = 0;

  // prova ad inserire il dato nella coda
  // se la coda fosse piena per più di 10 ticks, stampa un messaggio di errore
  if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE)
  {
    Serial.println("La coda è piena");
  }

  // aggiorna il dato
  num++;

  // attende prima di ripetere un nuovo inserimento
  // provare con una attesa di 1000 ms e poi con 500 ms
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}