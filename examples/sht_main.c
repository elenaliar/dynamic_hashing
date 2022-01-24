#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "sht_file.h"
//#include "hash_file.h"

#define RECORDS_NUM 256// you can change it if you want
#define GLOBAL_DEPT 1 // you can change it if you want
#define FILE_NAME "data.db"
#define FILE_NAME1 "data1.db"
#define FILE_NAME2 "data2.db"
#define FILE_NAME3 "data_.db"
#define FILE_NAME4 "data_1.db"
#define FILE_NAME5 "data_2.db"

#define CALL_OR_DIE(call)     \
  {                           \
    HT_ErrorCode code = call; \
    if (code != HT_OK) {      \
      printf("Error\n");      \
      exit(code);             \
    }                         \
  }

const char* names[] = {
  "Yannis",
  "Christofos",
  "Sofia",
  "Marianna",
  "Vagelis",
  "Maria",
  "Iosif",
  "Dionisis",
  "Konstantina",
  "Theofilos",
  "Giorgos",
  "Dimitris"
};

const char* surnames[] = {
  "Ioannidis",
  "Svingos",
  "Karvounari",
  "Rezkalla",
  "Nikolopoulos",
  "Berreta",
  "Koronis",
  "Gaitani",
  "Oikonomou",
  "Mailis",
  "Michas",
  "Halatsis",
  "Black", 
  "Brown", 
  "Papadopoulos", 
  "Green", 
  "Jones",
  "Red",
  "Yellow",
  "West", 
  "North", 
  "South", 
  "East",
  "Potter",
  "Weasley"
  ,"Granger"
  ,"Chang"
  ,"Jackson"
};
const char* cities[] = {
  "Athens",
  "San Francisco",
  "Los Angeles",
  "Amsterdam",
  "London",
  "New York",
  "Tokyo",
  "Hong Kong",
  "Munich",
  "Miami",
  "Seoul", 
  "Daegu",
  "Rome", 
  "Washington",
  "Paris", 
  "Madrid",
  "Oslo",
  "Budapest",
  "Busan",
  "Beijing",
  "San Jose",
  "Berlin",
  "New Delhi",
  "Dublin",
  "Warsaw",
  "Stockholm",
  "Sarajevo",
  "Austin"
};

int main() {
      BF_Init(LRU);
  
      CALL_OR_DIE(HT_Init());
      CALL_OR_DIE(SHT_Init());

      int indexDesc, indexDesc1, indexDesc2, count=0, count2=0, iDesc, iDesc1, iDesc2, cnt=0, cnt2=0;
    
      CALL_OR_DIE(HT_CreateIndex(FILE_NAME, GLOBAL_DEPT));
      CALL_OR_DIE(HT_OpenIndex(FILE_NAME, &indexDesc)); 
      CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME1, "surname", 20, GLOBAL_DEPT, FILE_NAME));
      CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME1, &indexDesc1)); 
      CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME2, "city", 20, GLOBAL_DEPT, FILE_NAME));
      CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME2, &indexDesc2)); 

      Record record;
      SecondaryRecord record1, record2;
      srand(12569874);
      int r, l=0, m=0;
      //printf("Insert Entries\n");
      printf("Creating first primary directory\n\n\n");
      for (int id = 0; (id < RECORDS_NUM); ++id) {
    // create a record
        UpdateRecordArray updateArray[max_records];
        for(int i=0; i<max_records; i++){
          updateArray[i].oldTupleId = -1;
        }
        int tupleId=0;
        record.id = id;
        r = rand() % 12;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 26;
        r = l;
        l++;
        if(l>25){
          l=0;
          count++;
        }
        //sinartisi check an available
        //r = check(r, used1s[r], used1s);
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        memcpy(record1.index_key, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 26;
        r = m;
        m++;
        if(m>25){
          m=0;
          count2++;
        }
        //r = check(r, used1c[r], used1c);
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        memcpy(record2.index_key, cities[r], strlen(cities[r]) + 1);
        CALL_OR_DIE(HT_InsertEntry(indexDesc, record, &tupleId, updateArray));
        record1.tupleId = tupleId;
        record2.tupleId = tupleId;
        if(tupleId!=0)
          CALL_OR_DIE(SHT_SecondaryInsertEntry(indexDesc1, record1));
        if(updateArray[0].oldTupleId != -1)
          SHT_SecondaryUpdateEntry(indexDesc1, updateArray); 
        if(tupleId!=0)
          CALL_OR_DIE(SHT_SecondaryInsertEntry(indexDesc2, record2));
        if(updateArray[0].oldTupleId != -1){
          SHT_SecondaryUpdateEntry(indexDesc2, updateArray); 
        }
      }


      CALL_OR_DIE(HT_CreateIndex(FILE_NAME3, GLOBAL_DEPT));
      CALL_OR_DIE(HT_OpenIndex(FILE_NAME3, &iDesc)); 
      CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME4, "surname", 20, GLOBAL_DEPT, FILE_NAME3));
      CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME4, &iDesc1)); 
      CALL_OR_DIE(SHT_CreateSecondaryIndex(FILE_NAME5, "city", 20, GLOBAL_DEPT, FILE_NAME3));
      CALL_OR_DIE(SHT_OpenSecondaryIndex(FILE_NAME5, &iDesc2)); 

      l=0, m=0;
      //printf("Insert Entries\n");
      printf("Creating second primary directory\n\n\n");
      for (int id = 0; (id < RECORDS_NUM); ++id) {
    // create a record
        UpdateRecordArray updateArray[max_records];
        for(int i=0; i<max_records; i++){
          updateArray[i].oldTupleId = -1;
        }
        int tupleId=0;
        record.id = id;
        r = rand() % 12;
        memcpy(record.name, names[r], strlen(names[r]) + 1);
        r = rand() % 26;
        r = l;
        l++;
        if(l>25){
          l=0;
          count++;
        }
        //r = check(r, used2s[r], used2s);
        memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
        memcpy(record1.index_key, surnames[r], strlen(surnames[r]) + 1);
        r = rand() % 26;
        r = m;
        m++;
        if(m>25){
          m=0;
          count2++;
        }
        //r = check(r, used2c[r], used2c);
        memcpy(record.city, cities[r], strlen(cities[r]) + 1);
        memcpy(record2.index_key, cities[r], strlen(cities[r]) + 1);
        CALL_OR_DIE(HT_InsertEntry(iDesc, record, &tupleId, updateArray));
        record1.tupleId = tupleId;
        record2.tupleId = tupleId;
        if(tupleId!=0)
          CALL_OR_DIE(SHT_SecondaryInsertEntry(iDesc1, record1));
        if(updateArray[0].oldTupleId != -1)
          SHT_SecondaryUpdateEntry(iDesc1, updateArray); 
        if(tupleId!=0)
          CALL_OR_DIE(SHT_SecondaryInsertEntry(iDesc2, record2));
        if(updateArray[0].oldTupleId != -1){
          SHT_SecondaryUpdateEntry(iDesc2, updateArray); 
        }
      }


      printf("Print and Hashstatistics on first primary directory\n\n");
      int id = rand() % RECORDS_NUM;
      printf("Showing statistics for primary directory\n");
      CALL_OR_DIE(HT_HashStatistics(FILE_NAME));
      printf("\n");
      printf("Showing statistics for secondary directory on surname\n");
      CALL_OR_DIE(SHT_HashStatistics(FILE_NAME1));
      printf("\n");
      printf("Showing statistics for secondary directory on city\n");
      CALL_OR_DIE(SHT_HashStatistics(FILE_NAME2));
      printf("\n");
      printf("Print functions from primary directory\n");
      CALL_OR_DIE(HT_PrintAllEntries(indexDesc, &id));
      CALL_OR_DIE(HT_PrintAllEntries(indexDesc, NULL));
      printf("\n");
      int tmp = rand()%26;
      printf("Print functions from secondary directory on surname\n");
      CALL_OR_DIE(SHT_PrintAllEntries(indexDesc1, surnames[tmp]));
      printf("\n");
      tmp = rand()%26;
      printf("Print functions from secondary directory on city\n");
      CALL_OR_DIE(SHT_PrintAllEntries(indexDesc2, cities[tmp]));
      printf("\n");


      printf("Print and Hashstatistics on second primary directory\n\n");
      id = rand() % RECORDS_NUM;
      printf("Showing statistics for primary directory\n");
      CALL_OR_DIE(HT_HashStatistics(FILE_NAME3));
      printf("\n");
      printf("Showing statistics for secondary directory on surname\n");
      CALL_OR_DIE(SHT_HashStatistics(FILE_NAME4));
      printf("\n");
      printf("Showing statistics for secondary directory on city\n");
      CALL_OR_DIE(SHT_HashStatistics(FILE_NAME5));
      printf("\n");
      printf("Print functions from primary directory\n");
      CALL_OR_DIE(HT_PrintAllEntries(iDesc, &id));
      CALL_OR_DIE(HT_PrintAllEntries(iDesc, NULL));
      printf("\n");
      tmp = rand()%26;
      printf("Print functions from secondary directory on surname\n");
      CALL_OR_DIE(SHT_PrintAllEntries(iDesc1, surnames[tmp]));
      printf("\n");
      tmp = rand()%26;
      printf("Print functions from secondary directory on city\n");
      CALL_OR_DIE(SHT_PrintAllEntries(iDesc2, cities[tmp]));
      printf("\n");

      printf("Join on surname\n\n");
      tmp = rand()%26;
      CALL_OR_DIE(SHT_InnerJoin(indexDesc1, iDesc1, surnames[tmp]));
      CALL_OR_DIE(SHT_InnerJoin(indexDesc1, iDesc1, NULL));
      printf("\n");

      printf("Join on city\n\n");
      tmp = rand()%26;
      CALL_OR_DIE(SHT_InnerJoin(indexDesc2, iDesc2, cities[tmp]));
      CALL_OR_DIE(SHT_InnerJoin(indexDesc2, iDesc2, NULL));
      printf("\n");
      



      CALL_OR_DIE(HT_CloseFile(indexDesc));
      CALL_OR_DIE(SHT_CloseSecondaryIndex(indexDesc1)); 
      CALL_OR_DIE(SHT_CloseSecondaryIndex(indexDesc2)); 
      CALL_OR_DIE(HT_CloseFile(iDesc));
      CALL_OR_DIE(SHT_CloseSecondaryIndex(iDesc1)); 
      CALL_OR_DIE(SHT_CloseSecondaryIndex(iDesc2)); 
      emptyfilearray();
      BF_Close();
}
