This is a very basic command line interpreter for Linux Operating Systems. It supports both interactive mode,
where the user can execute all the commands that can be executed via the execvp function, and batch mode, where
the user provides a batch file to be executed.

This tool was developed for the semester's project of Operating Systems class of Electrical Engineering and Computer Engineering
department at Aristotle University of Thessaloniki.

The code is in the shell.c file. Use the terminal to build, execute or delete the executable:

    $ make
    $ ./shell
    $ ./shell <batch file>
    $ ./shell_strict
    $ ./shell_strict <batch file>
    $ make clean
    
Georgios Kamtziridis, Winter Semester 2017-2018
    
The following is a detailed documentation of the code (in Greek).

## Επεξήγηση προγράμματος

Όταν ξεκινάει η εκτέλεση του προγράμματος, ελέγχονται τα ορίσματα εισόδου. Εάν έχει δοθεί ένα όρισμα, τότε το πρόγραμμα θα τρέξει σε _batch mode_, ενώ αν δεν έχει δοθεί κανένα θα τρέξει σε _interactive mode_. Αν δοθούν παραπάνω από ένα ορίσματα εμφανίζεται μήνυμα σφάλματος.

Έπειτα, ξεκινάει η εκτέλεση του κυρίως προγράμματος που βρίσκεται μέσα σε ένα _infinite loop_. Στην αρχή του _loop_ γίνεται έλεγχος του mode στο οποίο βρισκόμαστε, ώστε ή να διαβάσουμε μια γραμμή από το αρχείο που έχει δοθεί ή να εμφανίσουμε το _prompt_ και να περιμένουμε είσοδο από το χρήστη. Αφού πάρουμε είσοδο, καλούμε την συνάρτηση _parse_commands_ η οποία θα μας επιστρέψει  τον αριθμό των εντολών προς εκτέλεση καθώς και τις εντολές αυτές. Ο αριθμός επιστρέφεται από τη συνάρτηση με _return statement_, ενώ οι εντολές που θα εκτελεστούν χρησιμοποιούνται στη συνάρτηση σαν _output_ μεταβλητές. Οι εντολές αυτές είναι τύπου _Command_, και περιέχουν 2 πεδία:
*	**command**: η εντολή μαζί με τα ορίσματα (πχ _ls -la_).
*	**right**: ένδειξη του _delimiter_ που υπάρχει στα δεξιά. Η τιμή 1 υποδηλώνει ότι δεξιά της εντολής υπάρχει ο _delimiter_ ‘&&’, ενώ η τιμή 0 υποδηλώνει ότι είτε υπάρχει ο _delimiter_ ‘;’ είτε δεν υπάρχει τίποτα.
Χρησιμοποιούμε αυτό το _struct_ ώστε να είναι σε θέση το _prompt_ μας να εκτελέσει και σύνθετες παραστάσεις όπου χρησιμοποιούνται και οι δύο _delimiters_ ταυτόχρονα. Αυτό μπορεί να γίνει πιο κατανοητό στο _report_ του _project_, όπου δίνονται μερικά παραδείγματα εκτέλεσης του _prompt_.

Σε αυτή τη φάση ξεκινάει το δεύτερο _loop_, που είναι υπεύθυνο για την εκτέλεση της κάθε εντολής. Για την εκτέλεση της εντολής, κάνουμε _fork_. Το _child_ καλεί τη συνάρτηση _parse_arguments_, η οποία χωρίζει την εντολή που παίρνει σαν όρισμα. Τη χωρίζει στην εντολή που θα εκτελεστεί και στα ορίσματά της, ώστε τα δεδομένα αυτά να δοθούν στη συνάρτηση _execvp_. Εδώ, ελέγχεται αν η εντολή προς εκτέλεση είναι η εντολή εξόδου _quit_. Αν είναι τότε τερματίζεται το _child process_ με _status code 1_. Αν όχι, τότε πραγματοποιείται κλήση της συνάρτησης _execvp_. Αν εκτελεστεί η εντολή με επιτυχία τότε το _child process_ τερματίζει με _status code 0_. Αν, όμως, η εντολή δεν εκτελεστεί, παραδείγματος χάρη λόγω σφάλματος στη σύνταξη, τότε τερματίζεται το _child process_ με _status code 2_.

Το _parent process_ περιμένει τον τερματισμό του _child process_. Όταν αυτό γίνει, τότε αποθηκεύουμε το _status code_ ώστε να ληφθούν διαφορετικές ενέργειες, ανάλογα με το είδος του τερματισμού του _child process_:
*	**status code 0**: η εντολή εκτελέστηκε με επιτυχία, οπότε το _parent process_ αυξάνει τον δείκτη «i» ώστε να δείχνει στην επόμενη εντολή. Εάν δεν υπάρχει άλλη εντολή, αυτό θα εντοπιστεί στον έλεγχο του τρέχοντος _while loop_.
*	**status code 1**: στην περίπτωση αυτή ζητήθηκε από τον χρήστη ο τερματισμός του προγράμματος, άρα το _parent process_ τερματίζει το πρόγραμμα.
*	**status code 2**: η εκτέλεση της εντολής απέτυχε, οπότε ελέγχεται τι υπάρχει δεξιά της εντολής αυτής. Εάν υπάρχει ο _delimiter_ ‘&&’, τότε γνωρίζουμε ότι η επόμενη εντολή, έστω Α, δεν θα εκτελεστεί. Οπότε πηγαίνουμε στην Α και εξετάζουμε ξανά αν στα δεξιά της υπάρχει ο _delimter_ ‘&&’. Η διαδικασία αυτή συνεχίζεται μέχρι να βρεθεί ο _delimiter_ ‘;’ και να εκτελεστεί η εντολή μετά απ’ αυτόν ή να μην υπάρχει άλλη εντολή.

Να αναφέρουμε ότι η επιλογή του _status code 2_ για την περίπτωση της αποτυχημένης εκτέλεσης δεν είναι τυχαία. Γνωρίζουμε ότι όταν αποτυγχάνει η συνάρτηση _execvp_, επιστρέφει -1. Ωστόσο, υπάρχουν περιπτώσεις όπου η συνάρτηση αυτή αποτυγχάνει σε μετέπειτα στάδια (της εκτέλεσής της) και δεν επιστρέφει -1. Χαρακτηριστικό παράδειγμα αποτελεί η εντολή _ls_ όταν δοθεί με λανθασμένα ορίσματα. Τότε, η συνάρτηση τερματίζει με _status code 2_ και δεν επιστρέφεται ποτέ το -1. Άρα, χρησιμοποιώντας το 2 σαν κωδικό, όλες οι εντολές που δεν έχουν εκτελεστεί σωστά λόγω σφάλματος θα μοιράζονται το ίδιο _code_.

### Συναρτήσεις

Πιο αναλυτικά, οι συναρτήσεις που χρησιμοποιούνται είναι τρεις.

#### verify_command(char *command)

Η συνάρτηση αυτή παίρνει σαν όρισμα την είσοδο που δίνει ο χρήστης στο _interactive mode_ ή μια γραμμή από το αρχείο που διαβάζεται στο _batch mode_. Ελέγχει αν υπάρχει _delimiter_ ‘&&’ και όχι ‘&’, μιας και η _strtok_ δεν μπορεί να εντοπίσει διαφορές ανάμεσα στις δύο αυτές περιπτώσεις. Για ευκολία, στη περίπτωση όπου δίνεται σαν _delimiter_ ο ‘;;’ θεωρούμε ότι υπάρχει μια κενή εντολή μεταξύ τους η οποία παραλείπεται. 

#### parse_commands(Command commands[], char *command, char *delimiter)

Η συνάρτηση αυτή παίρνει σαν ορίσματα εισόδου την εντολή προς _parsing_ και τον _delimiter_. Ο πίνακας _commands_ αποτελεί μεταβλητή εξόδου όπως περιεγράφηκε προηγουμένως. Η συνάρτηση είναι αναδρομική. Στο πρώτο επίπεδο σπάει την εντολή με τον _delimiter_ ‘;’. Έπειτα, κάθε μια από τις εντολές αυτές χωρίζονται με τον _delimiter_ ‘&&’ στο δεύτερο επίπεδο. Στο επίπεδο αυτό όλες οι εντολές έχουν στα δεξιά τους ‘&&’ (δηλαδή _right_ = 1) εκτός από την τελευταία. Αν υπάρχει μόνο μια εντολή, τότε στα δεξιά της θα έχει είτε τον _delimiter_ ‘;’ είτε θα είναι η τελευταία εντολή της γραμμής (δηλαδή _right_ = 0). Λόγω της αναδρομής που χρησιμοποιείται, μόνο στη συνάρτηση αυτή χρησιμοποιείται η συνάρτηση _strtok_r_ αντί της _strtok_, ώστε να εξασφαλιστεί η ανεξαρτησία της από κλήση σε κλήση.

#### parse_arguments(char **args, char *command)

Η συνάρτηση αυτή παίρνει σαν όρισμα εισόδου την εντολή που πρόκειται να εκτελεστεί. Επιστρέφει, μέσω της μεταβλητής εξόδου _args_ την εντολή χωρισμένη στην ίδια (στη θέση 0) και στα ορίσματά της. Στην τελευταία θέση τοποθετείται η τιμή _NULL_, όπως απαιτεί η συνάρτηση _execvp_.
