#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Εδώ δηλώνουμε όλες τις συναρτήσεις που χρησιμοποιεί το πρόγραμμα. 
// Σκεφτείτε το σαν έναν "οδηγό" για το τι κάνει κάθε κομμάτι του κώδικα.

#include "items.h" // Χρησιμοποιούμε τη δομή Item από το αρχείο items.h

// Φτιάχνει τον κατάλογο προϊόντων (τα ονόματα, τις τιμές, τα αποθέματα κ.λπ.).
void setup_items(Item items[]);

// Εδώ είναι η δουλειά του πελάτη: 
// Στέλνει παραγγελίες και περιμένει να δει αν θα γίνουν δεκτές.
void customer_process(Item items[], int client_num, int pipe_to_shop[], int pipe_from_shop[]);

// Εδώ είναι η δουλειά του καταστήματος:
// Παίρνει παραγγελίες, ελέγχει το απόθεμα και απαντάει αν μπορεί να πουλήσει ή όχι.
void shop_process(Item items[], int client_num, int pipe_to_shop[], int pipe_from_shop[]);

#endif 

