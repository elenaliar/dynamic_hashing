#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "bf.h"
#include "sht_file.h"
#define MAX_OPEN_FILES 20


#pragma
//#ifndef HASH_FILE_H
#define HASH_FILE_H

int check(int id, int counter, int* used){
    if(counter>=max_srecords){
        int newid = rand()%13;
        return check(newid, used[newid], used);
    }
    else{
        used[id]++;
        return id;
    }
}

int get_typeofdir(char* data){
    int* a = (int*) data;
    a+=3;
    int type = *a;
    return type;
}


//epistrefei to filedesc tou arxeiou sto opoio einai deuterevon
int get_ffiledesc(char* data){
    int* a = (int*) data;
    a+=4;
    int desc = *a;
    return desc;
}

void swriterecord(char* data, SecondaryRecord r){
    int* a = (int*) data;
    a +=2;//stin arxi exoume 3 int ara tous prospername gia na ftasoume sto simeio pou xoraei eggrafi
    *a +=  1;//auksise tin timi se autin tin thesi(tin metavliti number_records) kata 1
    int total_records = *a;
    a++;
    SecondaryRecord* b = (SecondaryRecord* )a;
    for(int i=0; i<total_records-1;i++){//prosperase ta idi grammena records
        b++;
    }
    memcpy(b, &r, sizeof(SecondaryRecord)+1);
}

//epistrefei to num-osto record enos bucket , to indexing arxizei apo 0 ara an theloume to proto record enos bucket dinoume num=0
SecondaryRecord* sfindrec(char* data, int num){
    int* a = (int*) data;
    a +=2;//stin arxi exoume 3 int ara tous prospername gia na ftasoume sto simeio pou xoraei eggrafi
    int total_records = *a;
    a++;
    SecondaryRecord* b = (SecondaryRecord* )a;
    for(int i=0; i<total_records; i++){//prosperase ta idi grammena records
        if(i == num){
            return b;
        }
        b++;
    }  
}

//pairnei to index_key=key, kai to oldtupleid ta vriskei mesa sto bucket kai antikathista to oldtupleid me to newtupleid
void update_secrec(char* data, int oldtupleid, int newtupleid, char key[20]){
    int* a = (int*) data;
    a+=2;
    int totalrec = *a;
    a++;
    SecondaryRecord* s = (SecondaryRecord*) a;
    SecondaryRecord new;
    memcpy(new.index_key, key, strlen(key) + 1);
    new.tupleId = newtupleid;
    for(int j=0; j<totalrec; j++){
        if(s->tupleId==oldtupleid && (strcmp(s->index_key, key)==0)){
            *s = new;
           return;
        }
        s++;
    }
    return;
}

//os hash function theorisame to athroisma ton ascii kodikon ton grammaton apo ta opoia apoteleitai to string
//meta ton arithmo poy prokiptei ton pairname stin hash0function tou protevontos
char* shash_function(char *str, int gd){
    long int res =0;
    for(int i=0; i<strlen(str); i++){
        res += (str[i]);
    }
    res = res%10000;
    return hash_function(gd, res, 1);
 }

void srehashrecords(char* data, char* data2, char* data3, int gd){
    int oldblock = get_number_of_block(data);
    int newblock = get_number_of_block(data2);
    int* a = (int*) data;
    a+=2;
    int totalrecords = *a;
    *a = 0;
    a++;
    SecondaryRecord* r = (SecondaryRecord*)a;
    SecondaryRecord* r2 = (SecondaryRecord*) a;
    SecondaryRecord tmprec[max_srecords];
    for(int i=0; i<max_srecords; i++){
        tmprec[i] = *r;
        r++;
    }
    memset(r2, 0, max_srecords*sizeof(SecondaryRecord));
    int k=0;
    int l=0;
    for(int j=0; j<max_srecords; j++){
        char* hash_id = shash_function(tmprec[j].index_key, gd);
        int dest = get_bucket_of_index(hash_id, data3, gd);
        if(dest == oldblock){
            k++;
            swriterecord(data, tmprec[j]);
        }
        else if(dest == newblock){
            l++;
            swriterecord(data2, tmprec[j]);
        }
        else{
            printf("%s not\n", tmprec[j].index_key);
        }
        free(hash_id);
    }
}



//grafo sto block 0, to global depth, ton tipo tou secondary directory, to filedesc tou protevontos tou
void swrite_gd(char* data, int gd, int type, int filedesc){
    int* a = (int *)data;
    a++;
    *a = gd;
    a+=2;
    *a = type;
    a++;
    *a = filedesc;
}



//prosthetei ta stoixeia tou anoixtou arxeiou ston global array
void saddopenfile(const char* sfile, int index, int bl, int filedesc, int type){
      strcpy((arrayfile[index]).filename, sfile);
      strcpy((arrayfile[index]).ffilename, "yes");
      arrayfile[index].indexdesc = index;
      arrayfile[index].filedesc = filedesc;
      arrayfile[index].blocks = bl;
      arrayfile[index].flag = 1;
      arrayfile[index].type=type;
}

//afairei ta stoixeia tou arxeiou apo ton global array
void sremovefile(int index){
      arrayfile[index].indexdesc = -1;
      arrayfile[index].filedesc = -1;
      arrayfile[index].blocks = -1;
      arrayfile[index].flag = 0;
      arrayfile[index].type = 0;
      strcpy(arrayfile[index].filename, "");
      strcpy(arrayfile[index].ffilename, "");
}


//vriskei kai ektiponei tin eggrafi me to sigkekrimeno tupleid
//flag == 1 kaloume tin printrecord xoris allagi grammis
//flag == 0 kaloume tin printrecord me allagi grammis
//diatrexei ena ena ta buckets tou protevontos mexri na ftasei stin zitoumeni eggrafi
HT_ErrorCode findrec(int tupleid, int desc, int flag){
        BF_Block* block;
        char* data;
        BF_Block_Init(&block);
        int total;
        CALL_BF(BF_GetBlockCounter(desc, &total));
        CALL_BF(BF_GetBlock(desc, 0, block));
        data = BF_Block_GetData(block);
        int hashblocks = get_blocksforhashtable(data);
        CALL_BF(BF_UnpinBlock(block));
        BF_Block_Destroy(&block);
        BF_Block_Init(&block);
        int bucket = total - hashblocks;
        bucket = bucket-1;
        bucket = total-bucket;
        int tmp = tupleid;
        for(int i=bucket; i<total; i++){
            if(tmp<=0){
                return HT_OK;
            }
            if(tmp <= max_records){
                CALL_BF(BF_GetBlock(desc, i, block));
                data = BF_Block_GetData(block);
                int* a= (int*)data;
                a+=2;
                int totalrec = *a;
                a++;
                Record* b = (Record*) a;
                for(int j=0; j<totalrec; j++){ 
                    tmp--;
                    if(tmp==0){
                        if(flag==1){
                            printrecord(b);
                        }
                        else if(flag==0){
                            print_record(b);
                        }
                        break;
                    }
                    b++;
                }
            }
            else{
                tmp -= max_records;
                continue;
            }
        }
        CALL_BF(BF_UnpinBlock(block));
        BF_Block_Destroy(&block);
        return HT_OK;
}

HT_ErrorCode SHT_Init() {
  //insert code here
      return HT_OK;
}

HT_ErrorCode SHT_CreateSecondaryIndex(const char *sfileName, char *attrName, int attrLength, int depth, char *fileName){
  //insert code here
      BF_Block *block;
      int desc, number;
      char* data;
      int type=0;
      int num_block, total_buckets=1;
      for(int u=depth; u>0; u--){
        total_buckets = total_buckets * 2;
      }
      total_buckets += 2;
      if(strcmp(attrName, "surname") == 0)
        type=2;
      else if(strcmp(attrName, "city") == 0)
        type=3;
      CALL_BF(BF_CreateFile(sfileName));
      CALL_BF(BF_OpenFile(sfileName, &desc));
      int ffiledesc;
      //theoroume oti ta arxeia exoun diaforetika onomata 
      for(int i=0; i<MAX_OPEN_FILES; i++){//vriskei proti keni thesi tou global array me ta anoixta arxeia gia na grapsoume ekei ta stoixeia tou neou arxeiou
        if (strcmp(arrayfile[i].filename, fileName) == 0){
            ffiledesc = arrayfile[i].filedesc;
            break;
        }
      }
      BF_Block_Init(&block);
      for(int i=0; i<total_buckets; i++){//auto to for ftiaxnei sosta to hashtable mono tin proti fora pou to ftiaxnoume apo to miden
          CALL_BF(BF_AllocateBlock(desc, block));
          CALL_BF(BF_GetBlock(desc, i, block));
          data = BF_Block_GetData(block);
          CALL_BF(BF_GetBlockCounter(desc, &number));
          assign_number_to_block(data, number-1);
          if(i==0){//block 0 me plirofories
              swrite_gd(data, depth, type, ffiledesc);
              writeblocksforhash(data, 1);
          }
          else if(i==1){//block 1 me to hashtable
              create_hashtable(data, depth);
          }
          else{//block 2+ pou einai buckets kai periexoun eggrafes
              initialize_bucket(data);//thetei localdepth=1 kai num_records=0;
          }
          BF_Block_SetDirty(block);
          CALL_BF(BF_UnpinBlock(block));

      }
      BF_Block_Destroy(&block);
      CALL_BF(BF_CloseFile(desc));
      return HT_OK;
  return HT_OK;
}

HT_ErrorCode SHT_OpenSecondaryIndex(const char *sfileName, int *indexDesc) {
  //insert code here
      int filedescr, i, number;
      for(i=0; i<MAX_OPEN_FILES; i++){//vriskei proti keni thesi tou global array me ta anoixta arxeia gia na grapsoume ekei ta stoixeia tou neou arxeiou
        if (arrayfile[i].flag == 0){
            *indexDesc = i;
            break;
        }
      }
      CALL_BF(BF_OpenFile(sfileName, &filedescr));
      CALL_BF(BF_GetBlockCounter(filedescr, &number));
      BF_Block* block;
      char* data;
      BF_Block_Init(&block);
      CALL_BF(BF_GetBlock(filedescr, 0, block));
      data = BF_Block_GetData(block);
      int type = get_typeofdir(data);
      CALL_BF(BF_UnpinBlock(block));
      BF_Block_Destroy(&block);
      saddopenfile(sfileName, *indexDesc, number, filedescr, type);
      printf("%s, %s, %d, %d, %d, %d, %d\n", arrayfile[i].filename, arrayfile[i].ffilename, arrayfile[i].indexdesc, arrayfile[i].filedesc, arrayfile[i].blocks, arrayfile[i].flag, arrayfile[i].type);
      return HT_OK;
  return HT_OK;
}

HT_ErrorCode SHT_CloseSecondaryIndex(int indexDesc) {
  //insert code here
    if(arrayfile[indexDesc].flag == 0)
          return HT_OK;
    printf("%s, %d, %d, %d, %d, %d\n", arrayfile[indexDesc].filename, arrayfile[indexDesc].indexdesc, arrayfile[indexDesc].filedesc, arrayfile[indexDesc].blocks, arrayfile[indexDesc].flag, arrayfile[indexDesc].type);
    CALL_BF(BF_CloseFile(arrayfile[indexDesc].filedesc));
    sremovefile(indexDesc);
    return HT_OK;
}

HT_ErrorCode SHT_SecondaryInsertEntry (int indexDesc,SecondaryRecord record) {
  //insert code here
    char id[20];
    strcpy(id, record.index_key);
    int ld, gd, records, number, type, *tupleid;
    BF_Block* block;
    BF_Block_Init(&block);
    int desc = arrayfile[indexDesc].filedesc;
    CALL_BF(BF_GetBlockCounter(desc, &number));
    CALL_BF(BF_GetBlock(desc, 0, block));
    char* data = BF_Block_GetData(block);
    gd = get_globaldepth(data);//gd = globaldepth
    type = get_typeofdir(data);
    int blockshash = get_blocksforhashtable(data);//posa block gia to hashtable
    CALL_BF(BF_UnpinBlock(block));
    char* hashed_id = shash_function(record.index_key, gd);
    if(blockshash == 1){
          CALL_BF(BF_GetBlock(desc, 1, block));
          data = BF_Block_GetData(block);
          int block_2 = get_bucket_of_index(hashed_id, data, gd);//to bucket proorismou tou recordid vrisketai sto block me arithmo block_2
          int k = calculatek(data, block_2, gd);//k = posa kelia tou hashtable deixnoun sto bucket me arithmo block=block_2
          CALL_BF(BF_UnpinBlock(block));
          CALL_BF(BF_GetBlock(desc, block_2, block));
          data = BF_Block_GetData(block);
          ld = get_localdepth(data);//local depth tou bucket sto block2
          records = get_number_of_records(data);//posa records exei
          CALL_BF(BF_UnpinBlock(block));
          if(max_srecords <= records){
              if(ld == gd){
                  if(gd>=5)
                      return HT_OK;
                  //split to hash table kai meta ton kado
                  int newgd = gd+1;
                  BF_Block* block4;//deixnei sto block 0 me ta genika stoixeia
                  BF_Block_Init(&block4);
                  char* data4;
                  CALL_BF(BF_GetBlock(desc, 0, block4));
                  data4 = BF_Block_GetData(block4);
                  increase_globaldepth(data4, newgd);
                  BF_Block_SetDirty(block4);
                  CALL_BF(BF_UnpinBlock(block4));
                  BF_Block_Destroy(&block4);
                  BF_Block* block3;//sto block3 tha nai to hashtable
                  char* data3;
                  BF_Block_Init(&block3);//sto block 3 einai to block 1 pou exei to hashtable
                  CALL_BF(BF_GetBlock(desc, 1, block3));
                  data3 = BF_Block_GetData(block3);
                  create_new_hashtable(data3, gd, newgd);
                  BF_Block* block2;
                  char* data2;
                  int number;
                  BF_Block_Init(&block2);//dimiourgeitai neo block sto telos tou arxeiou
                  CALL_BF(BF_AllocateBlock(desc, block2));
                  CALL_BF(BF_GetBlockCounter(desc, &number));
                  CALL_BF(BF_GetBlock(desc, number-1, block2));
                  data2 = BF_Block_GetData(block2);
                  assign_number_to_block(data2, number-1);
                  inittotalrectozero(data2);
                  increasecountblocks(indexDesc, 1);
                  int oldblock = block_2;
                  int newblock = number-1;
                  int newld = ld+1;
                  assignlocaldepth(data2, newld);
                  assignlocaldepth(data, newld);
                  k = calculatek(data3, oldblock, newgd);
                  changearrows(data3, k, oldblock, newblock, newgd);
                  int newdest;
                  //sinartisi pou pairnei ena ena ta records tou oldblock kai ta kanei rehash
                  srehashrecords(data, data2, data3, newgd);
                  //afou ginoun rehashed ta records tou oldblock kanoume insert to neo record
                  free(hashed_id);
                  hashed_id = shash_function(record.index_key, newgd);//ksanaipologizoume to hash_id afou auksithikai to global depth
                  //o neos kouvas proorismou tou neou record
                  newdest = get_bucket_of_index(hashed_id, data3, newgd);
                  if(newdest == oldblock){
                    swriterecord(data, record);
                  }
                  else if(newdest == newblock){
                    swriterecord(data2, record);
                  }
                  BF_Block_SetDirty(block);
                  CALL_BF(BF_UnpinBlock(block));
                  BF_Block_Destroy(&block);
                  BF_Block_SetDirty(block2);
                  CALL_BF(BF_UnpinBlock(block2));
                  BF_Block_Destroy(&block2);
                  BF_Block_SetDirty(block3);
                  CALL_BF(BF_UnpinBlock(block3));
                  BF_Block_Destroy(&block3);
              }   
              else{
                  //split ton kadon, prosthetoume ena neo kado kai kanoume rehash tis eggrafes tou paliou
                  BF_Block* block2;
                  char* data2;
                  int number;
                  BF_Block_Init(&block2);//dimiourgeitai neo block sto telos tou arxeiou
                  CALL_BF(BF_AllocateBlock(desc, block2));
                  CALL_BF(BF_GetBlockCounter(desc, &number));
                  CALL_BF(BF_GetBlock(desc, number-1, block2));
                  data2 = BF_Block_GetData(block2);
                  assign_number_to_block(data2, number-1);
                  inittotalrectozero(data2);
                  increasecountblocks(indexDesc, 1);
                  int oldblock = block_2;
                  int newblock = number-1;
                  int newld = ld+1;
                  assignlocaldepth(data2, newld);
                  assignlocaldepth(data, newld);
                  //prepei na allaksoun oi arithmoi sto hashtable na deixnoun allou
                  BF_Block* block3;//sto block3 tha nai to hashtable
                  char* data3;
                  BF_Block_Init(&block3);
                  CALL_BF(BF_GetBlock(desc, 1, block3));
                  data3 = BF_Block_GetData(block3);
                  k = calculatek(data3, oldblock, gd);
                  changearrows(data3, k, oldblock, newblock, gd);
                  k = calculatek(data3, oldblock, gd);
                  //sinartisi pou pairnei mia mia ta records tou oldblock kai ta kanei rehash
                  srehashrecords(data, data2, data3, gd);
                  //afou ginoun rehashed ta records tou oldblock kanoume insert to neo record
                  int newdest = get_bucket_of_index(hashed_id, data3, gd);//o neos kouvas proorismou tou neou record
                  if(newdest == oldblock)
                    swriterecord(data, record);
                  else if(newdest == newblock)
                    swriterecord(data2, record);
                  BF_Block_SetDirty(block);
                  CALL_BF(BF_UnpinBlock(block));
                  BF_Block_Destroy(&block);
                  BF_Block_SetDirty(block2);
                  CALL_BF(BF_UnpinBlock(block2));
                  BF_Block_Destroy(&block2);
                  BF_Block_SetDirty(block3);
                  CALL_BF(BF_UnpinBlock(block3));
                  BF_Block_Destroy(&block3);
              }
          }
          else{//apli periptosi insert opou iparxei xoros
              if(records>=max_srecords){
                CALL_OR_DIE(SHT_SecondaryInsertEntry(indexDesc, record));
              }
              swriterecord(data, record); 
              BF_Block_SetDirty(block);
              CALL_BF(BF_UnpinBlock(block));
              BF_Block_Destroy(&block);
          }
        
      }
      else{//periptosi pou to hashtable pianei pano apo 2 blocks
          BF_Block *block;
          for(int t=0; t<blockshash; t++){
              BF_Block_Init(&block);
              CALL_BF(BF_GetBlock(desc, t+1, block));
              data = BF_Block_GetData(block);
              int block2 = get_bucket_of_index(hashed_id, data, gd);
              CALL_BF(BF_UnpinBlock(block));
              // BF_Block_Destroy(&block);
              BF_Block_Init(&block);
              CALL_BF(BF_GetBlock(desc, block2, block));
              data = BF_Block_GetData(block);
              ld = get_localdepth(data);
              records = get_number_of_records(data);
              if(max_srecords == records){
                  if(ld == gd){
              //split to hash table kai meta ton kado
                  }   
                  else{
              //split ton kadon
                  }
              }
              else{//apli periptosi insert opou iparxei xoros
                  swriterecord(data, record);  
                  BF_Block_SetDirty(block);
                  CALL_BF(BF_UnpinBlock(block)); 
                  BF_Block_Destroy(&block); 
                  break;
              }
              CALL_BF(BF_UnpinBlock(block));
              BF_Block_Destroy(&block);
          }
      }
    free(hashed_id);
    return HT_OK;
}

HT_ErrorCode SHT_SecondaryUpdateEntry (int indexDesc, UpdateRecordArray *updateArray ){
  //insert code here
    int desc = arrayfile[indexDesc].filedesc;
    BF_Block* block;
    BF_Block_Init(&block);
    CALL_BF(BF_GetBlock(desc, 0, block));
    char* data = BF_Block_GetData(block);
    int type = get_typeofdir(data);
    int gd = get_globaldepth(data);
    CALL_BF(BF_UnpinBlock(block));
    for(int i=0; i<max_srecords; i++){
        CALL_BF(BF_GetBlock(desc, 1, block));
        data = BF_Block_GetData(block);
        char* hash_key;//edo kanoume hash to index key 
        if(type==2)
            hash_key = shash_function(updateArray[i].surname, gd);
        else if(type==3)
            hash_key = shash_function(updateArray[i].city, gd);
        int bucket = get_bucket_of_index(hash_key, data, gd);
        CALL_BF(BF_UnpinBlock(block));
        CALL_BF(BF_GetBlock(desc, bucket, block));
        data = BF_Block_GetData(block);
        //sinartisi pou pairnei to data kai ta stoixeia tou pinaka kai ta kanei update
        if(type==2)
           update_secrec(data, updateArray[i].oldTupleId, updateArray[i].newTupleId, updateArray[i].surname); 
        else if(type==3)
            update_secrec(data, updateArray[i].oldTupleId, updateArray[i].newTupleId, updateArray[i].city);
        BF_Block_SetDirty(block);
        CALL_BF(BF_UnpinBlock(block));
        free(hash_key);
    }
    BF_Block_Destroy(&block);
    return HT_OK;
}

HT_ErrorCode SHT_PrintAllEntries(int sindexDesc, char *index_key ) {
    //insert code here
    char* hash_key; 
    char* filename;
    printf("printing all records with index_key = %s\n", index_key);
    int desc = arrayfile[sindexDesc].filedesc;
    int block;
    BF_Block* bl;
    BF_Block_Init(&bl);
    CALL_BF(BF_GetBlock(desc, 0, bl));
    char* data = BF_Block_GetData(bl);
    int gd = get_globaldepth(data);
    hash_key = shash_function(index_key, gd);
    int fdesc = get_ffiledesc(data);
    int hashblocks = get_blocksforhashtable(data);
    CALL_BF(BF_UnpinBlock(bl));
    BF_Block_Destroy(&bl);
    for(int r=1; r<=hashblocks; r++){
        BF_Block_Init(&bl);
        CALL_BF(BF_GetBlock(desc, r, bl));
        data = BF_Block_GetData(bl);
        block = get_bucket_of_index(hash_key, data, gd);
        CALL_BF(BF_UnpinBlock(bl));
        BF_Block_Destroy(&bl);
        if(block != 0)
            break;
    }
    BF_Block_Init(&bl);
    CALL_BF(BF_GetBlock(desc, block, bl));
    data = BF_Block_GetData(bl);
    int* a =(int*)data;
    a+=2;
    int totalsecrec = *a;
    a++;
    SecondaryRecord* r = (SecondaryRecord*)a;
    for(int k=0; k<totalsecrec; k++){
        if(strcmp(r->index_key, index_key) == 0){
            findrec(r->tupleId, fdesc, 1);//entopizei sto protevon arxeio tin eggrafi me to sigekrimeno tupleid kai tin ektiponei
        }
        r++;
    }
    CALL_BF(BF_UnpinBlock(bl));
    BF_Block_Destroy(&bl);
    free(hash_key);
  return HT_OK;
}

HT_ErrorCode SHT_HashStatistics(char *filename ) {
  //insert code here
      int i, desc;
      for(i=0; i<MAX_OPEN_FILES; i++){//vriskei proti keni thesi tou global array me ta anoixta arxeia gia na grapsoume ekei ta stoixeia tou neou arxeiou
        if(arrayfile[i].flag == 0)
            continue;
        if (strcmp(arrayfile[i].filename, filename) == 0){
            desc = arrayfile[i].filedesc;
            int hashblocks, totalblocks, bucket, countrecords, minrecord=0, maxrecord=0, record, totalrecord=0, avgrecord;
            BF_GetBlockCounter(desc, &totalblocks);
            char* data;
            BF_Block* block;
            BF_Block_Init(&block);
            CALL_BF(BF_GetBlock(desc, 0, block));
            data = BF_Block_GetData(block);
            hashblocks = get_blocksforhashtable(data);
            bucket = totalblocks - hashblocks;
            bucket = bucket-1;
            bucket = totalblocks - bucket;
            int j=0;
            CALL_BF(BF_UnpinBlock(block));
            BF_Block_Destroy(&block);
            for(int e=bucket; e<totalblocks; e++){
                BF_Block_Init(&block);
                CALL_BF(BF_GetBlock(desc, e, block));
                data = BF_Block_GetData(block);
                record = get_number_of_records(data);
                if(e == bucket){
                    minrecord = record;
                    maxrecord=record;
                }
                else{
                    if(record<minrecord)
                        minrecord = record;
                    if(record>maxrecord)
                        maxrecord = record;
                }
                CALL_BF(BF_UnpinBlock(block));
                BF_Block_Destroy(&block);
                totalrecord += record;
                j++;
            }
            avgrecord = totalrecord/j;
            printf("showing statistics for file with name: %s and filedesc=%d\n", filename, desc);
            printf("totalrecords = %d\n", totalrecord);
            printf("totalblocks = %d\n", totalblocks);
            printf("minimum number of records in bucket = %d\n", minrecord);
            printf("maximum number of records in bucket = %d\n", maxrecord);
            printf("average number of records in the buckets = %d\n", avgrecord);
        }
      }
  return HT_OK;
}

HT_ErrorCode SHT_InnerJoin(int sindexDesc1, int sindexDesc2,  char *index_key ) {
  //insert code here
    int desc1, desc2, fdesc1, fdesc2, totalblocks1, totalblocks2;
    desc1 = arrayfile[sindexDesc1].filedesc;
    desc2 = arrayfile[sindexDesc2].filedesc;
    char* hash_key1, *hash_key2; 
    char* data1, *data2;
    int block1, block2;
    BF_Block* bl;
    BF_GetBlockCounter(desc1, &totalblocks1);
    BF_GetBlockCounter(desc2, &totalblocks2);

    BF_Block_Init(&bl);

    CALL_BF(BF_GetBlock(desc1, 0, bl));
    data1 = BF_Block_GetData(bl);
    fdesc1 = get_ffiledesc(data1);
    int gd1 = get_globaldepth(data1);
    int hashblocks1 = get_blocksforhashtable(data1);
    CALL_BF(BF_UnpinBlock(bl));
    BF_Block_Destroy(&bl);

    BF_Block_Init(&bl);
    CALL_BF(BF_GetBlock(desc2, 0, bl));
    data2 = BF_Block_GetData(bl);
    fdesc2 = get_ffiledesc(data2);
    int gd2 = get_globaldepth(data2);
    int hashblocks2 = get_blocksforhashtable(data2);
    CALL_BF(BF_UnpinBlock(bl));
    BF_Block_Destroy(&bl);

    if(index_key!=NULL){

        hash_key1 = shash_function(index_key, gd1);
        hash_key2 = shash_function(index_key, gd2);

        for(int r=1; r<=hashblocks1; r++){
            BF_Block_Init(&bl);
            CALL_BF(BF_GetBlock(desc1, r, bl));
            data1 = BF_Block_GetData(bl);
            block1 = get_bucket_of_index(hash_key1, data1, gd1);
            CALL_BF(BF_UnpinBlock(bl));
            BF_Block_Destroy(&bl);
            if(block1 != 0)
                break;
        }

        for(int r=1; r<=hashblocks2; r++){
            BF_Block_Init(&bl);
            CALL_BF(BF_GetBlock(desc2, r, bl));
            data2 = BF_Block_GetData(bl);
            block2 = get_bucket_of_index(hash_key2, data2, gd2);
            CALL_BF(BF_UnpinBlock(bl));
            BF_Block_Destroy(&bl);
            if(block2 != 0)
                break;
        }
        printf("Showing results of join function\nPrinting records from both primary directories with index_key = %s\n\n", index_key);
        BF_Block_Init(&bl);
        CALL_BF(BF_GetBlock(desc1, block1, bl));
        data1 = BF_Block_GetData(bl);
        CALL_BF(BF_UnpinBlock(bl));
        BF_Block_Destroy(&bl);
        int* a =(int*)data1;
        a+=2;
        int totalsecrec1 = *a;
        a++;
        SecondaryRecord* r = (SecondaryRecord*)a;
        for(int k=0; k<totalsecrec1; k++){
            if(strcmp(r->index_key, index_key) == 0){
                BF_Block_Init(&bl);
                CALL_BF(BF_GetBlock(desc2, block2, bl));
                data2 = BF_Block_GetData(bl);
                int* a2 =(int*)data2;
                a2+=2;
                int totalsecrec2 = *a2;
                a2++;
                SecondaryRecord* r2 = (SecondaryRecord*)a2;
                for(int k=0; k<totalsecrec2; k++){
                    if(strcmp(r2->index_key, index_key) == 0){
                        findrec(r->tupleId, fdesc1, 0);
                        printf(" ---- ");
                        findrec(r2->tupleId, fdesc2, 0);//entopizei sto protevon arxeio tin eggrafi me to sigekrimeno tupleid kai tin ektiponei
                        printf("\n");
                    }
                    r2++;
                }
                CALL_BF(BF_UnpinBlock(bl));
                BF_Block_Destroy(&bl);
            }
            printf("\n");
            r++;
        }
        free(hash_key1);
        free(hash_key2);
    }
    else{
        printf("Printing all possible pairs\n\n");
        int buck1 = totalblocks1 - hashblocks1;
        buck1 = buck1-1;
        buck1 = totalblocks1 - buck1;
        int buck2 = totalblocks2 - hashblocks2;
        buck2 = buck2-1;
        buck2 = totalblocks2 - buck2;
        char* data3;
        for(int j=buck1; j<totalblocks1; j++){
            BF_Block_Init(&bl);
            CALL_BF(BF_GetBlock(desc1, j, bl));
            data3 = BF_Block_GetData(bl);
            int cnt = get_number_of_records(data3);
            CALL_BF(BF_UnpinBlock(bl));
            BF_Block_Destroy(&bl);
            for(int h=0; h<cnt; h++){
                SecondaryRecord* rec = sfindrec(data3, h);
                char* hashed;
                for(int r=1; r<=hashblocks2; r++){
                    BF_Block_Init(&bl);
                    CALL_BF(BF_GetBlock(desc2, r, bl));
                    data2 = BF_Block_GetData(bl);
                    hashed = shash_function(rec->index_key, gd2);
                    block2 = get_bucket_of_index(hashed, data2, gd2);
                    CALL_BF(BF_UnpinBlock(bl));
                    BF_Block_Destroy(&bl);
                    if(block2 != 0)
                        break;
                }
                BF_Block_Init(&bl);
                CALL_BF(BF_GetBlock(desc2, block2, bl));
                data2 = BF_Block_GetData(bl);
                int* a2 =(int*)data2;
                a2+=2;
                int totalsecrec2 = *a2;
                a2++;
                SecondaryRecord* r2 = (SecondaryRecord*)a2;
                for(int k=0; k<totalsecrec2; k++){
                    if(strcmp(r2->index_key, rec->index_key) == 0){
                        findrec(rec->tupleId, fdesc1, 0);
                        printf(" ---- ");
                        findrec(r2->tupleId, fdesc2, 0);//entopizei sto protevon arxeio tin eggrafi me to sigekrimeno tupleid kai tin ektiponei
                        printf("\n");
                    }
                    r2++;
                }
                CALL_BF(BF_UnpinBlock(bl));
                BF_Block_Destroy(&bl);
                free(hashed);
            }
        }

    }
    return HT_OK;
}




//#endif // HASH_FILE_H