#include <stdio.h>   // Για εκτύπωση και εισαγωγή/έξοδο
#include <stdlib.h>  // Για δυναμική μνήμη, όπως malloc και exit
#include <unistd.h>  // Για pipes, fork και άλλες συναρτήσεις συστήματος
#include <string.h>  // Για χειρισμό strings
#include <sys/types.h> // Για τύπους όπως pid_t
#include <sys/wait.h>  // Για την αναμονή (wait) παιδιών διεργασιών
#include "functions.h" // Το αρχείο με τις δηλώσεις συναρτήσεων

// Ορίζουμε σταθερές για το πρόγραμμα
#define NUM_PRODUCTS 20      // Πόσα προϊόντα έχει το κατάστημα
#define NUM_CLIENTS 5        // Πόσοι πελάτες θα τρέξουν
#define ORDERS_PER_CLIENT 10 // Πόσες παραγγελίες κάνει κάθε πελάτης

// Η κύρια συνάρτηση του προγράμματος
int main() {
    // Δημιουργούμε έναν πίνακα από προϊόντα
    Item items[NUM_PRODUCTS]; // Έχουμε 20 προϊόντα στον πίνακα
    setup_items(items);       // Φορτώνουμε τα προϊόντα με δεδομένα (όνομα, κόστος, απόθεμα)

    // Pipes για επικοινωνία
    int pipe_to_shop[NUM_CLIENTS][2];   // Από τους πελάτες προς το κατάστημα
    int pipe_from_shop[NUM_CLIENTS][2]; // Από το κατάστημα προς τους πελάτες

    // Δημιουργία των pipes
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pipe(pipe_to_shop[i]);   // Φτιάχνουμε pipe για τον πελάτη i -> κατάστημα
        pipe(pipe_from_shop[i]); // Φτιάχνουμε pipe για το κατάστημα -> πελάτη i
    }

    // Δημιουργία παιδιών διεργασιών (πελάτες)
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pid_t pid = fork(); // Δημιουργούμε νέο process για κάθε πελάτη
        if (pid == 0) {
            // Είμαστε στο process του πελάτη
            // Ο κάθε πελάτης θα χρησιμοποιήσει το customer_process
            customer_process(items, i, pipe_to_shop[i], pipe_from_shop[i]);
            exit(0); // Μόλις τελειώσει, ο πελάτης σταματάει
        }
    }

    // Είμαστε ακόμα στο parent process (το κατάστημα)
    // Το κατάστημα τώρα επικοινωνεί με όλους τους πελάτες
    for (int i = 0; i < NUM_CLIENTS; i++) {
        // Το κατάστημα επεξεργάζεται τα δεδομένα που στέλνουν οι πελάτες
        shop_process(items, i, pipe_to_shop[i], pipe_from_shop[i]);
    }

    // Περιμένουμε όλοι οι πελάτες να τελειώσουν
    for (int i = 0; i < NUM_CLIENTS; i++) {
        wait(NULL); // Περιμένουμε κάθε παιδί να τελειώσει
    }

    // Εκτύπωση αναφοράς στο τέλος
    printf("\nΑναφορά:\n");

    // Ορίζουμε μεταβλητές για τα συνολικά στατιστικά
    int total_orders = 0;          // Σύνολο παραγγελιών
    int successful_orders = 0;    // Παραγγελίες που ολοκληρώθηκαν
    int failed_orders = 0;        // Αποτυχημένες παραγγελίες
    float total_revenue = 0;      // Συνολικά έσοδα

    // Περνάμε από όλα τα προϊόντα για να βγάλουμε αναφορά
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Προϊόν %d (%s):\n", i + 1, items[i].name); // Εκτύπωση ονόματος προϊόντος
        printf("  Αιτήματα: %d\n", items[i].order_count);  // Πόσες φορές ζητήθηκε το προϊόν
        printf("  Πωλήσεις: %d\n", items[i].sold_count);  // Πόσες φορές πουλήθηκε
        printf("  Απόθεμα: %d\n\n", items[i].stock);      // Πόσα κομμάτια έμειναν στο ράφι

        // Ενημερώνουμε τα συνολικά στατιστικά
        total_orders += items[i].order_count;            // Προσθέτουμε τα αιτήματα
        successful_orders += items[i].sold_count;        // Προσθέτουμε τις πωλήσεις
        total_revenue += items[i].sold_count * items[i].cost; // Προσθέτουμε τα έσοδα
    }

    // Υπολογισμός αποτυχημένων παραγγελιών
    failed_orders = total_orders - successful_orders; // Όσες ζητήθηκαν αλλά δεν πουλήθηκαν

    // Εκτύπωση τελικών στατιστικών
    printf("Σύνολο Παραγγελιών: %d\n", total_orders);       // Όλες οι παραγγελίες
    printf("Επιτυχημένες Παραγγελίες: %d\n", successful_orders); // Πόσες ολοκληρώθηκαν
    printf("Αποτυχημένες Παραγγελίες: %d\n", failed_orders);     // Πόσες δεν ολοκληρώθηκαν
    printf("Συνολικά Έσοδα: %.2f€\n", total_revenue);       // Πόσα λεφτά βγάλαμε

    return 0; // Τέλος προγράμματος
}

