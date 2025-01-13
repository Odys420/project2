#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"

// Αρχικοποιεί τα προϊόντα με δεδομένα
void setup_items(Item items[]) {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        snprintf(items[i].name, sizeof(items[i].name), "Προϊόν %d", i + 1);
        items[i].cost = (i + 1) * 5;  // Τιμή προϊόντος: 5, 10, 15 κ.λπ.
        items[i].stock = 10;          // Αρχικό απόθεμα 10 για κάθε προϊόν
        items[i].order_count = 0;     // Αρχικοί μετρητές παραγγελιών
        items[i].sold_count = 0;      // Αρχικοί μετρητές πωλήσεων
    }
}

// Επεξεργασία του πελάτη (αποστολή παραγγελιών στο κατάστημα)
void customer_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]) {
    for (int i = 0; i < ORDERS_PER_CLIENT; i++) {
        int product_id = rand() % NUM_PRODUCTS; // Επιλέγουμε τυχαίο προϊόν
        write(pipe_to_shop[1], &product_id, sizeof(product_id));  // Στέλνουμε παραγγελία στο κατάστημα

        int response;
        read(pipe_from_shop[0], &response, sizeof(response));  // Διαβάζουμε την απάντηση από το κατάστημα

        if (response == 1) {
            printf("Πελάτης %d: Παραγγελία %d για το Προϊόν %d ολοκληρώθηκε\n", client_id + 1, i + 1, product_id + 1);
        } else {
            printf("Πελάτης %d: Παραγγελία %d για το Προϊόν %d απέτυχε\n", client_id + 1, i + 1, product_id + 1);
        }
    }
}

// Επεξεργασία του καταστήματος (λήψη παραγγελιών από πελάτες)
void shop_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]) {
    for (int i = 0; i < ORDERS_PER_CLIENT; i++) {
        int product_id;
        read(pipe_to_shop[0], &product_id, sizeof(product_id));  // Λήψη παραγγελίας από πελάτη
        items[product_id].order_count++;  // Αύξηση αιτημάτων για το προϊόν

        int response;
        if (items[product_id].stock > 0) {
            items[product_id].stock--;  // Μείωση αποθέματος
            items[product_id].sold_count++;  // Αύξηση πωλήσεων
            response = 1;  // Επιτυχής παραγγελία
        } else {
            response = 0;  // Αποτυχημένη παραγγελία (εξάντληση αποθέματος)
        }

        write(pipe_from_shop[1], &response, sizeof(response));  // Αποστολή απάντησης στον πελάτη
    }
}
