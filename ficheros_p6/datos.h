struct data {
    int servings;
    int cook_waiting; /* # de hilos bloqueados en cook_queue (0 o 1) */
    int nr_sav_waiting; /* # de salvajes bloqueados */  
};