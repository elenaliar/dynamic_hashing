//#ifndef HASH_FILE_H
#pragma
#define HASH_FILE_H
#define MAX_OPEN_FILES 20


typedef enum HT_ErrorCode {
  HT_OK,
  HT_ERROR
} HT_ErrorCode;

#define CALL_OR_DIE(call)     \
  {                           \
    HT_ErrorCode code = call; \
    if (code != HT_OK) {      \
      printf("Error\n");      \
      exit(code);             \
    }                         \
  }

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {      \
    BF_PrintError(code);    \
    return HT_ERROR;        \
  }                         \
}


typedef struct Record {
	int id;
	char name[15];
	char surname[20];
	char city[20];
} Record;

typedef struct {  //μπορειτε να αλλαξετε τη δομη συμφωνα  με τις ανάγκες σας
	char surname[20];
	char city[20];
	int oldTupleId; // η παλια θέση της εγγραφής πριν την εισαγωγή της νέας
	int newTupleId; // η νέα θέση της εγγραφής που μετακινήθηκε μετα την εισαγωγή της νέας εγγραφής 
} UpdateRecordArray;

typedef struct{
	char index_key[20];
	int tupleId;  /*Ακέραιος που προσδιορίζει το block και τη θέση μέσα στο block στην οποία έγινε η εισαγωγή της εγγραφής στο πρωτεύον ευρετήριο.*/ 
}SecondaryRecord;


#define max_records BF_BLOCK_SIZE/sizeof(Record)
#define max_srecords 500/sizeof(SecondaryRecord)

typedef struct Infofile{
	char filename[50];//onoma arxeiou katakermatismou
	int indexdesc;//thesi tou anoixtou arxeiou ston pinaka arrayfile
	int blocks;//posa blocks exei
	int filedesc;//anagnoristiko arxeiou block
	int flag;//otan to flag einai 0 simainei den iparxei arxeio stin thesi tou pinaka allios isoutai me 1
	int type;//=1 otan einai protevon, =2 otan deuterevon sto surname, =3 deuterevon sto city
	char ffilename[50];//an type==1 einai iso me "no", allios einai iso me "yes"
}Infofile;


//sto bucket ta stoixeia einai apothikeumena me ayti tin seira
//den to xrisimopoioume pouthena, apla iparxei gia na einai pio eukoli eikonika i anaparastasi tou bucket
typedef struct Bucket{
	int blocknumber;//arithmos block sto opoio vrisketai to bucket
	int localdepth; //to localdepth tou bucket
	int number_records;//posa records exoume apothikeusei sto bucket/block
	Record records[max_records];
}Bucket;


typedef struct info_hash{
	char*  index;//to index tou hashtable
	int block;//arithmos block pou vrisketai to bucket
}info_hash;



extern Infofile* arrayfile;//pinakas gia na kratame ta stoixeia ton arxeion katakermatismou pou exoun anoixtei
void rehashrecords(char*, char*, char*, int, UpdateRecordArray*, int);//allaksa tin rehashrecords oste na gemizei ton updaterecordarray
int calculate_tupleid(Record*, char*);
char* hash_function(int,int, int);
void printrecord(Record*);
void print_record(Record*);
void printbucket(char*);
void writerecord(char*, Record);
int get_number_of_records(char*);
void assign_number_to_block(char*, int);
void write_gd(char*, int);
void initialize_bucket(char*);
void create_hashtable(char*, int);
int get_localdepth(char*);
int get_globaldepth(char*);
int get_number_of_block(char*);
int get_bucket_of_index(char*, char*, int);
int get_blocksforhashtable(char*);
void increase_localdepth(char*);
void increase_globaldepth(char*, int);
int calculate_localdepth(char*, int, int);
void emptyfilearray();
void findcertainrecordid(char*, int);
void addopenfile(const char*, int, int, int);
void removefile(int);
void writeblocksforhash(char*, int);
void increasecountblocks(int, int);
int calculatek(char*, int, int);
void assignlocaldepth(char*, int);
void changearrows(char*, int, int, int, int);
void inittotalrectozero(char*);
void create_new_hashtable(char*, int, int);

/*
 * Η συνάρτηση HT_Init χρησιμοποιείται για την αρχικοποίηση κάποιον δομών που μπορεί να χρειαστείτε. 
 * Σε περίπτωση που εκτελεστεί επιτυχώς, επιστρέφεται HT_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
 */
HT_ErrorCode HT_Init();

/*
 * Η συνάρτηση HT_CreateIndex χρησιμοποιείται για τη δημιουργία και κατάλληλη αρχικοποίηση ενός άδειου αρχείου κατακερματισμού με όνομα fileName. 
 * Στην περίπτωση που το αρχείο υπάρχει ήδη, τότε επιστρέφεται ένας κωδικός λάθους. 
 * Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HΤ_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
 */
HT_ErrorCode HT_CreateIndex(
	const char *fileName,		/* όνομααρχείου */
	int depth
	);


/*
 * Η ρουτίνα αυτή ανοίγει το αρχείο με όνομα fileName. 
 * Εάν το αρχείο ανοιχτεί κανονικά, η ρουτίνα επιστρέφει HT_OK, ενώ σε διαφορετική περίπτωση κωδικός λάθους.
 */
HT_ErrorCode HT_OpenIndex(
	const char *fileName, 		/* όνομα αρχείου */
  int *indexDesc            /* θέση στον πίνακα με τα ανοιχτά αρχεία  που επιστρέφεται */
	);

/*
 * Η ρουτίνα αυτή κλείνει το αρχείο του οποίου οι πληροφορίες βρίσκονται στην θέση indexDesc του πίνακα ανοιχτών αρχείων.
 * Επίσης σβήνει την καταχώρηση που αντιστοιχεί στο αρχείο αυτό στον πίνακα ανοιχτών αρχείων. 
 * Η συνάρτηση επιστρέφει ΗΤ_OK εάν το αρχείο κλείσει επιτυχώς, ενώ σε διαφορετική σε περίπτωση κωδικός λάθους.
 */
HT_ErrorCode HT_CloseFile(
	int indexDesc 		/* θέση στον πίνακα με τα ανοιχτά αρχεία */
	);

/*
 * Η συνάρτηση HT_InsertEntry χρησιμοποιείται για την εισαγωγή μίας εγγραφής στο αρχείο κατακερματισμού. 
 * Οι πληροφορίες που αφορούν το αρχείο βρίσκονται στον πίνακα ανοιχτών αρχείων, ενώ η εγγραφή προς εισαγωγή προσδιορίζεται από τη δομή record. 
 * Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HT_OK, ενώ σε διαφορετική περίπτωση κάποιος κωδικός λάθους.
 */
HT_ErrorCode HT_InsertEntry(
	int indexDesc,	/* θέση στον πίνακα με τα ανοιχτά αρχεία */
	Record record,
	int* tupleId,
	UpdateRecordArray* updateArray		/* δομή που προσδιορίζει την εγγραφή */
	);

/*
 * Η συνάρτηση HΤ_PrintAllEntries χρησιμοποιείται για την εκτύπωση όλων των εγγραφών που το record.id έχει τιμή id. 
 * Αν το id είναι NULL τότε θα εκτυπώνει όλες τις εγγραφές του αρχείου κατακερματισμού. 
 * Σε περίπτωση που εκτελεστεί επιτυχώς επιστρέφεται HP_OK, ενώ σε διαφορετική περίπτωση κάποιος κωδικός λάθους.
 */
HT_ErrorCode HT_PrintAllEntries(
	int indexDesc,	/* θέση στον πίνακα με τα ανοιχτά αρχεία */
	int *id 				/* τιμή του πεδίου κλειδιού προς αναζήτηση */
	);


HT_ErrorCode HT_HashStatistics(
	char *filename
	);

//#endif // HASH_FILE_H

