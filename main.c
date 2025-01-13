#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Ορίζουμε σταθερές για το πρόγραμμα
#define NUM_PRODUCTS 20      // Πόσα προϊόντα έχει το κατάστημα
#define NUM_CLIENTS 5        // Πόσοι πελάτες θα τρέξουν
#define ORDERS_PER_CLIENT 10 // Πόσες παραγγελίες κάνει κάθε πελάτης

// Δομή προϊόντος
typedef struct {
    char name[50];        // Όνομα προϊόντος
    float cost;           // Κόστος προϊόντος
    int stock;            // Απόθεμα προϊόντος
    int order_count;      // Αριθμός αιτημάτων για το προϊόν
    int sold_count;       // Αριθμός πωληθέντων τεμαχίων
} Item;

// Συνάρτηση για την αρχικοποίηση των προϊόντων
void setup_items(Item items[]) {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        snprintf(items[i].name, sizeof(items[i].name), "Προϊόν %d", i + 1);
        items[i].cost = (i + 1) * 5;
        items[i].stock = 10;
        items[i].order_count = 0;
        items[i].sold_count = 0;
    }
}

// Συνάρτηση για τη διαδικασία του πελάτη
void customer_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]) {
    for (int i = 0; i < ORDERS_PER_CLIENT; i++) {
        int product_id = rand() % NUM_PRODUCTS; // Επιλογή τυχαίου προϊόντος
        write(pipe_to_shop[1], &product_id, sizeof(product_id)); // Στέλνουμε το προϊόν στο κατάστημα

        int response;
        read(pipe_from_shop[0], &response, sizeof(response)); // Λαμβάνουμε την απάντηση από το κατάστημα

        // Εκτύπωση του αποτελέσματος της παραγγελίας
        if (response == 1) {
            printf("Πελάτης %d: Παραγγελία %d για το Προϊόν %d ολοκληρώθηκε\n", client_id + 1, i + 1, product_id + 1);
        } else {
            printf("Πελάτης %d: Παραγγελία %d για το Προϊόν %d απέτυχε\n", client_id + 1, i + 1, product_id + 1);
        }
    }
}

// Συνάρτηση για τη διαδικασία του καταστήματος
void shop_process(Item items[], int client_id, int pipe_to_shop[2], int pipe_from_shop[2]) {
    for (int i = 0; i < ORDERS_PER_CLIENT; i++) {
        int product_id;
        read(pipe_to_shop[0], &product_id, sizeof(product_id)); // Λαμβάνουμε το προϊόν από τον πελάτη
        items[product_id].order_count++; // Ενημερώνουμε τον αριθμό των αιτημάτων για το προϊόν

        int response;
        if (items[product_id].stock > 0) { // Αν υπάρχει απόθεμα, ολοκληρώνουμε την παραγγελία
            items[product_id].stock--;
            items[product_id].sold_count++;
            response = 1; // Επιτυχία
        } else {
            response = 0; // Αποτυχία (δεν υπάρχει απόθεμα)
        }

        write(pipe_from_shop[1], &response, sizeof(response)); // Στέλνουμε την απάντηση στον πελάτη
    }
}

// Συνάρτηση για την εκτύπωση της αναφοράς του καταστήματος
void print_report(Item items[]) {
    int total_orders = 0, successful_orders = 0, failed_orders = 0;
    float total_revenue = 0;

    // Εκτύπωση των στατιστικών για κάθε προϊόν
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Προϊόν %d (%s):\n", i + 1, items[i].name);
        printf("  Αιτήματα: %d\n", items[i].order_count);
        printf("  Πωλήσεις: %d\n", items[i].sold_count);
        printf("  Απόθεμα: %d\n\n", items[i].stock);

        // Ενημερώνουμε τα συνολικά στατιστικά
        total_orders += items[i].order_count;
        successful_orders += items[i].sold_count;
        total_revenue += items[i].sold_count * items[i].cost;
    }

    // Υπολογισμός αποτυχημένων παραγγελιών
    failed_orders = total_orders - successful_orders;

    // Εκτύπωση τελικών στατιστικών
    printf("Σύνολο Παραγγελιών: %d\n", total_orders);
    printf("Επιτυχημένες Παραγγελίες: %d\n", successful_orders);
    printf("Αποτυχημένες Παραγγελίες: %d\n", failed_orders);
    printf("Συνολικά Έσοδα: %.2f€\n", total_revenue);
}

// Η κύρια συνάρτηση του προγράμματος
int main() {
    // Δημιουργούμε έναν πίνακα από προϊόντα
    Item items[NUM_PRODUCTS];
    setup_items(items); // Φορτώνουμε τα προϊόντα με δεδομένα (όνομα, κόστος, απόθεμα)

    // Pipes για επικοινωνία
    int pipe_to_shop[NUM_CLIENTS][2];   // Από τους πελάτες προς το κατάστημα
    int pipe_from_shop[NUM_CLIENTS][2]; // Από το κατάστημα προς τους πελάτες

    // Δημιουργία των pipes
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pipe(pipe_to_shop[i]);   // Φτιάχνουμε pipe για τον πελάτη i -> κατάστημα
        pipe(pipe_from_shop[i]); // Φτιάχνουμε pipe για το κατάστημα -> πελάτη i
    }

    // Δημιουργία παιδιών διεργασιών (πελάτες)
    pid_t pid;
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pid = fork(); // Δημιουργούμε νέο process για κάθε πελάτη
        if (pid == 0) {
            // Είμαστε στο process του πελάτη
            customer_process(items, i, pipe_to_shop[i], pipe_from_shop[i]);
            exit(0); // Μόλις τελειώσει, ο πελάτης σταματάει
        }
    }

    // Είμαστε ακόμα στο parent process (το κατάστημα)
    // Το κατάστημα τώρα επικοινωνεί με όλους τους πελάτες
    for (int i = 0; i < NUM_CLIENTS; i++) {
        shop_process(items, i, pipe_to_shop[i], pipe_from_shop[i]);
    }

    // Περιμένουμε όλοι οι πελάτες να τελειώσουν
    for (int i = 0; i < NUM_CLIENTS; i++) {
        wait(NULL); // Περιμένουμε κάθε παιδί να τελειώσει
    }

    // Εκτύπωση αναφοράς στο τέλος
    printf("\nΑναφορά:\n");
    print_report(items); // Εκτύπωση αναφοράς για τα προϊόντα και τα συνολικά στατιστικά

    return 0; // Τέλος προγράμματος
}
