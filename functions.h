#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Δομή προϊόντος
typedef struct {
    char name[50];        // Όνομα προϊόντος
    float cost;           // Κόστος προϊόντος
    int stock;            // Απόθεμα προϊόντος
    int order_count;      // Αιτήματα παραγγελίας
    int sold_count;       // Πωλήσεις
} Item;

// Δηλώσεις συναρτήσεων
void setup_items(Item items[]);           // Ρύθμιση δεδομένων προϊόντων
void customer_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]);  // Επεξεργασία πελάτη
void shop_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]);  // Επεξεργασία καταστήματος
void print_report(Item items[]);          // Εκτύπωση αναφοράς για τα προϊόντα

#endif

