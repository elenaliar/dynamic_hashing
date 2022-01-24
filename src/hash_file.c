#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bf.h"
#include "sht_file.h"

Infofile* arrayfile;


//epistrefei posa records iparxoun se ena bucket
int get_number_of_records(char* data){
    int* a=(int*)data;
    a+=2;
    return *a;
}

//tin kaloume otan prosthetoume plus kainourgia block sto arxeio gia na ginoun update ta stoixeia tou pinaka
void increasecountblocks(int indexdesc, int plus){
    arrayfile[indexdesc].blocks += plus;
}

//simartisi pou allazei ta velakia, thelei orisma: data, posa velakia dld k kai tous arithmous ton dio block new kai old

void changearrows(char* data, int k, int oldblock, int newblock, int gd){
    int* a = (int*) data;
    a++;
    char* b = (char*)a;
    int countold=0;//poses fores sinantisame to oldblock, molis countold = k/2 opou sinantame oldblock tha vazoume newblock
    int count=1;
    for(int u=gd; u>0; u--){//ipologizei 2^gd tosa einai ta kelia tou hashtable=count
        count = count * 2;
    }
    for(int f=0; f<count; f++){//count = kelia
        for(int g=0; g<gd; g++){
            b++;
            if(g == gd-1)
                a = (int*) b;
        }
        if(*a == oldblock){
            countold++;
            if(countold>k/2){
                *a = newblock;
            }
        }
        a++;
        b = (char*) a;
    }
}

//ipologizei to k(opou k o arithmos ton kelion tou hashtable pou deixnoun sto bucket me blocknum)
int calculatek(char* data, int blocknum, int gd){
      int count=1, k=0, ld;//count = 2^depth = posa kelia tha exei to hashtable gia sigkekrimeno depth
      int* a = (int*) data;
      a++;
      char* b = (char*) a;
      for(int u=gd; u>0; u--){//count kelia sto hashtable
          count = count * 2;
      }
      for(int g=0; g<count; g++){
          for(int f=0; f<gd; f++){
              b++;
          }
          a = (int*) b;
          if(*a == blocknum){
            k++;
          }
          a++;
          b = (char*)a;
      }
      return k;
}

//epistrefei to localdepth tou bucket pou vrisketai sto block me arithmo blocknum
int calculate_localdepth(char* data, int blocknum, int gd){
      int count=1, k=0, ld;//count = 2^depth = posa kelia tha exei to hashtable gia sigkekrimeno depth
      int* a = (int*) data;
      a++;
      char* b = (char*) a;
      for(int u=gd; u>0; u--){//count kelia sto hashtable
          count = count * 2;
      }
      for(int g=0; g<count; g++){
          for(int f=0; f<gd; f++){
              b++;
              if(f == gd-1)
                  a = (int*) b;
          }
          if(*a == blocknum)
            k++;
          a++;
          b = (char*)a;
      }
      count=1;
      for(int u=k; u>0; u--){//ipologizo to 2^k
          count = count * 2;
      }
      ld = gd - count;//apo theoreia ld = gd -2^k
      return ld;
}
//epistrefei local depth tou block
int get_localdepth(char* data){
    int* a = (int*) data;
    a++;
    return *a;
}

//auksanei to local depth kata 1
void increase_localdepth(char* data){
    int* a = (int*) data;
    a++;
    *a = *a+1;
}

//thetei tin timi tou localdepth isi me ld
void assignlocaldepth(char* data, int ld){
    int* a = (int*) data;
    a++;
    *a = ld;
}

//epistrefei to globaldepth
int get_globaldepth(char* data){
    int* a = (int*) data;
    a++;
    int gd = *a;
    return gd;
}

//auksanei to global depth kata 1
void increase_globaldepth(char* data, int gd){
    int* a = (int*) data;
    a++;
    *a = gd;
}

//epistrefei enan arithmo o opoios isoutai to plithos ton block pou iparxei to hashtable
int get_blocksforhashtable(char* data){
    int* a = (int*) data;
    a+=2;
    return *a;
}

//epistrefei ton srithmo tou block
int get_number_of_block(char* data){
    int* a = (int*) data;
    return *a;
}

//grapsimo enos record se bucket pou vrisketai sto block me arithmo numblock sto arxeio me filedesc
void writerecord(char* data, Record r){
    int* a = (int*) data;
    a +=2;//stin arxi exoume 3 int ara tous prospername gia na ftasoume sto simeio pou xoraei eggrafi
    *a +=  1;//auksise tin timi se autin tin thesi(tin metavliti number_records) kata 1
    int total_records = *a;
    a++;
    Record* b = (Record* )a;
    for(int i=0; i<total_records-1;i++){//prosperase ta idi grammena records
        b++;
    }
    memcpy(b, &r, sizeof(Record)+1);
}

//ektiponei tis plirofories mias eggrafis
void printrecord(Record* a){
    printf("id=%d, name=%s, surname=%s, city=%s\n", a->id, a->name, a->surname, a->city);
}

//ektiponei tis plirofories mias eggrafis xoris omos allagi grammis
void print_record(Record* a){
    printf(" id=%d, name=%s, surname=%s, city=%s ", a->id, a->name, a->surname, a->city);
}

//ektiponei ta stoixeia enos bucket(1 bucket = 1 block)
//1.se poio block tou arxeiou eimaste, 2.to local depth tou bucket, 3.posa records exei, 4.ektiposi tou record
void printbucket(char* data){
      int* a = (int*) data;
      printf("number of block is %d\n", *a);
      a++;
      printf("local depth of bucket is %d\n", *a);
      a++;
      printf("number of records in bucket equals to %d\n", *a);
      int total_records = *a;
      a++;
      Record* b = (Record* )a;
      for(int i=0; i<total_records; i++){
          printf("printing info for record %d\n", i);
          printrecord(b);
          b++;
      }
}

void assign_number_to_block(char* data, int number){
    int* a = (int *)data;
    *a = number;
}

//
void write_gd(char* data, int gd){
    int* a = (int *)data;
    a++;
    *a = gd;
}

//to hash table xreiazetai total blocks
void writeblocksforhash(char* data, int total){
    int* a = (int *)data;
    a+=2;
    *a=total;
}
//epistrefei ton arithmo tou block sto opoio einai to buckets tou sigekrimenou index
//an den vrethikai kapoio koino hash index me to index epistrefoume 0
int get_bucket_of_index(char* index, char* data, int gd){
    char* index2;
    int count=1;
    int* b;
    int* a = (int*)data;
    a++;
    index2 = (char*)a;
    for(int u=gd; u>0; u--){//ipologizei 2^gd tosa einai ta kelia tou hashtable
        count = count * 2;
    }
    for(int f=0; f<count; f++){
        char* tmp;
        tmp = malloc(sizeof(char)*(gd+1));
        for(int g=0; g<gd; g++){
            tmp[g]=*index2;
            index2++;
        }
        tmp[gd] = '\0';
        int result  = strcmp(tmp, index);
        if(result == 0){
            b = (int*) index2;
            free(tmp);
            return *b;
        }
        b = (int*)index2;
        b++;
        index2 = (char*)b;
        free(tmp);
    }
    return 0;//den vrethikai kapoio index
}

//tha ftiaxnei to neo diplasiasmeno hashtable
void create_new_hashtable(char* data, int oldgd, int newgd){
    int *a = (int*) data;
    int blocknum = *a;
    a++;
    char* b = (char*)a;
    int count=1;//count = 2^depth = posa kelia tha exei to hashtable gia sigkekrimeno depth
    for(int u=oldgd; u>0; u--){
        count = count * 2;
    }
    info_hash temp[count];
    int i, j;
    for(i=0; i<count; i++){
        temp[i].index = (char*)malloc(sizeof(char)*(oldgd+1));
        for(j=0; j<oldgd; j++){
            temp[i].index[j] = *b;
            b++;
        }
        temp[i].index[oldgd] = '\0';
        a = (int*)b;
        temp[i].block = *a;
        a++;
        b = (char*)a;
    }
    memset(data, 0, sizeof(BF_BLOCK_SIZE));
    //////dimiourgia hashtable me newgd
    int count2=1;//count2 = 2^newgd
    for(int u=newgd; u>0; u--){
        count2 = count2 * 2;
    }
    a = (int*)data;
    *a = blocknum; //arithmos block pou vrisketai to hashtable
    a++;
    b = (char*) a;
    char* index = (char*)malloc(sizeof(char)*newgd+1);
    int k;
    for(int i=0; i<count2; i++){
        for(k=0; k<newgd; k++){
            index[k] = 0 +'0';
        }
        index[k] = '\0';
        int n=i;
        for(k=newgd-1; k>=0; k--){
            index[k] = n%2 +'0';
            n = n/2;
        }
        for(int h=0; h<newgd; h++){
            *b = index[h];
            b++;
        }
        a = (int*)b;
        *a = 1;//to 1 simainei oti den deixnei se kapoio bucket(den exei dimiourgithei to bucket)
        a++;
        b = (char*) a;
    }
    a = (int*)data;
    a++;
    b = (char*)a;
    for(int d=0; d<count; d++){
        a = (int*)data;
        a++;
        b = (char*)a;
        for(int e=0; e<count2; e++){
            char* tmp, *tmp2;
            tmp = (char*)malloc(sizeof(char)*(oldgd+1));
            for(int c=0; c<newgd; c++){
                if(c<oldgd)
                  tmp[c]=*b;
                b++;
            }
            tmp[oldgd] = '\0';
            a = (int*)b;
            if(strcmp(tmp, temp[d].index) == 0){
                *a = temp[d].block;
            }
            a++;
            b = (char*) a;
            free(tmp);
        }
    }
    for(int i=0; i<count; i++){
        free(temp[i].index); 
    }
    free(index);
}

//tin xrisimopoioumai mono otan ftiaxnoume to hashtable apo tin arxi
void create_hashtable(char* data, int depth){
    int count=1, k;//count = 2^depth = posa kelia tha exei to hashtable gia sigkekrimeno depth
    for(int u=depth; u>0; u--){
        count = count * 2;
    }
    int* a = (int*)data;
    int number = *a; //arithmos block pou vrisketai to hashtable
    a++;
    char *b = (char*) a;
    char index[depth];
    for(int i=0; i<count; i++){
        for(k=0; k<depth; k++){
            index[k] = 0 +'0';
        }
        index[k] = '\0';
        int n=i;
        for(k=depth-1; k>=0; k--){
            index[k] = n%2 +'0';
            n = n/2;
        }
        //index[k] = '\0';
        for(int h=0; h<depth; h++){
            *b = index[h];
            b++;
        }
        a = (int*)b;
        *a = ++number;//to 1 simainei oti den deixnei se kapoio bucket(den exei dimiourgithei to bucket)
        a++;
        b = (char*) a;
    }
}

//thetei localdepth se 1 kai records=0, xrisimopoieitai mono otan ftiaxoume to bucket stin arxi tis createindex
void initialize_bucket(char* data){
    int* a = (int*) data;
    a++;
    *a = 1;
    a++;
    *a=0;
}

//apodesmeusi tou xorou tou global array
void emptyfilearray(){
    free(arrayfile);
}

//psaxnei sta data enos bucket na vrei eggrafes me record.id=id kai tis tiponei
void findcertainrecordid(char* data, int id){
      int* a = (int*) data;
      a+=2;
      int total_records = *a;
      a++;
      Record* b = (Record* )a;
      for(int i=0; i<total_records; i++){
          if(b->id == id){
              printrecord(b);
          }
          b++;
      }
}


void inittotalrectozero(char* data){
    int* a = (int*) data;
    a+=2;
    *a=0;

}

//data=oldblock, data2=newblock, data3=hashtable, key=2->surname, key=3->city
//parallila me to rehash ton eggrafon tou oldblock apothikevoume katallila ta aparaitita stoixeia ston updatearray
void rehashrecords(char* data, char* data2, char* data3, int gd, UpdateRecordArray* updatearray, int desc){
    int oldblock = get_number_of_block(data);
    int newblock = get_number_of_block(data2);
    int* a = (int*) data;
    a+=2;
    int totalrecords = *a;
    a++;
    Record* r = (Record*)a;
    Record tmprec[max_records];
    for(int i=0; i<max_records; i++){
        tmprec[i] = *r;
        int tid;
        tid = calculate_tupleid(&tmprec[i], data);
        strcpy(updatearray[i].surname, r->surname);
        strcpy(updatearray[i].city, r->city);
        updatearray[i].oldTupleId = tid;
        r++;
    }
    a = (int*) data;
    a+=2;
    *a = 0;
    a++;
    Record* r2 = (Record*) a;
    memset(r2, 0, max_records*sizeof(Record));
    for(int j=0; j<max_records; j++){
        int id  = tmprec[j].id;
        char* hash_id = hash_function(gd, id, 1);
        int dest = get_bucket_of_index(hash_id, data3, gd);
        int tid;
        if(dest == oldblock){
            writerecord(data, tmprec[j]);
            tid = calculate_tupleid(&tmprec[j],data);
        }
        else if(dest == newblock){
            writerecord(data2, tmprec[j]);
            tid = calculate_tupleid(&tmprec[j], data2);
        }
        updatearray[j].newTupleId = tid;
        free(hash_id);
    }
    return HT_OK;
}

//hash function pou metatrepei to id se binary kai to antistrefei, apothikevei ta psifia se pinak
char* hash_function(int depth,int id, int flag){//otan to flag einai ena simainei oti xasaroume ena id record, otan einai 0 gia na ftiaksoume to hash index(hash table)
      int b[depth], b_2[depth];
      char* hash_index;
      for(int j = 0; j < depth; j++){ 
          b[j] = 0;
      }
      int i = depth-1;
      while(i !=-1){
          if(id % 2 == 0)
              b[i] = 0;
          else
              b[i] = 1;
          i--;
          id = id / 2;
      };
      if(flag == 1){//xasaroume id mias eggrafis
          for(int j=0; j<depth; j++){
              b_2[j] = b[(depth-1)-j];
          }
      }
      else if(flag == 0){//ftiaxnoume hashindex
          for(int j=0; j<depth; j++){
              b_2[j] = b[j];
          }
      }
      hash_index = (char*)malloc(sizeof(char)*(depth+1));
      for (int o=0; o<depth; o++){
          hash_index[o] = b_2[o]+'0';
      }
      hash_index[depth] = '\0';
      return hash_index;
}

//prosthetei ta stoixeia tou anoixtou arxeiou ston global array
void addopenfile(const char* file, int index, int bl, int filedesc){
      strcpy((arrayfile[index]).filename, file);
      strcpy((arrayfile[index]).ffilename, "no");
      arrayfile[index].indexdesc = index;
      arrayfile[index].filedesc = filedesc;
      arrayfile[index].blocks = bl;
      arrayfile[index].flag = 1;
      arrayfile[index].type = 1;

}

//afairei ta stoixeia tou arxeiou apo ton global array
void removefile(int index){
      arrayfile[index].indexdesc = -1;
      arrayfile[index].filedesc = -1;
      arrayfile[index].blocks = -1;
      arrayfile[index].flag = 0;
      arrayfile[index].type = 0;
      strcpy(arrayfile[index].filename, "");
      strcpy((arrayfile[index]).ffilename, "no");
}


//ipologizei to tupleid simfona me ton tipo pou eixe dothei stin ekfonisi
int calculate_tupleid(Record* r, char* data){
    int blocknum = get_number_of_block(data);
    int totalblocks = blocknum-2;
    int index_of_rec_in_block;
    int tmp =0;
    for(int k=0; k<totalblocks; k++)
        tmp += max_records;
    int* a= (int*)data;
    a+=2;
    int totalrec = *a;
    a++;
    Record* b = (Record*) a;
    for(int j=0; j<totalrec; j++){ 
        if(r->id==b->id){
            index_of_rec_in_block = j+1;
            tmp += index_of_rec_in_block;
            return tmp;
        }
        b++;
    }

}

    

HT_ErrorCode HT_Init() {
  //insert code here
      arrayfile = malloc(sizeof(Infofile) * MAX_OPEN_FILES);//arxikopoiisi tou arrayfile
      if( arrayfile==NULL ){
        printf("Unable to allocate memory\n");
        return HT_ERROR;
      }
      for(int i=0; i<MAX_OPEN_FILES; i++){
          arrayfile[i].flag = 0;
          arrayfile[i].type = 0;
      }
      return HT_OK;
}

//dimiourgo ena keno arxeio katakermatismou, dimiourgo to block 0 opou apothikevo to to blocknum=0, globaldepth=depth, kai to sinoliko arithmo me blocks pou exei to arxeio
HT_ErrorCode HT_CreateIndex(const char *filename, int depth) {
  //insert code here
      BF_Block *block;
      int desc, number;
      char* data;
      int num_block, total_buckets=1;

      for(int u=depth; u>0; u--){
        total_buckets = total_buckets * 2;
      }
      total_buckets += 2;
      CALL_BF(BF_CreateFile(filename));
      CALL_BF(BF_OpenFile(filename, &desc));
      BF_Block_Init(&block);
      for(int i=0; i<total_buckets; i++){//auto to for ftiaxnei sosta to hashtable mono tin proti fora pou to ftiaxnoume apo to miden
          CALL_BF(BF_AllocateBlock(desc, block));
          CALL_BF(BF_GetBlock(desc, i, block));
          data = BF_Block_GetData(block);
          CALL_BF(BF_GetBlockCounter(desc, &number));
          assign_number_to_block(data, number-1);
          if(i==0){//block 0 me plirofories
              write_gd(data, depth);
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
}


HT_ErrorCode HT_OpenIndex(const char *fileName, int *indexDesc){
  //insert code here
      int filedescr, i, number;
      for(i=0; i<MAX_OPEN_FILES; i++){//vriskei proti keni thesi tou global array me ta anoixta arxeia gia na grapsoume ekei ta stoixeia tou neou arxeiou
        if (arrayfile[i].flag == 0){
            *indexDesc = i;
            break;
        }
      }
      CALL_BF(BF_OpenFile(fileName, &filedescr));
      CALL_BF(BF_GetBlockCounter(filedescr, &number));
      addopenfile(fileName, *indexDesc, number, filedescr);
      printf("%s, %d, %d, %d, %d\n", arrayfile[i].filename, arrayfile[i].indexdesc, arrayfile[i].filedesc, arrayfile[i].blocks, arrayfile[i].flag);
      return HT_OK;
}

HT_ErrorCode HT_CloseFile(int indexDesc) {
  //insert code here
      if(arrayfile[indexDesc].flag == 0)
          return HT_OK;
      printf("%s, %d, %d, %d, %d\n", arrayfile[indexDesc].filename, arrayfile[indexDesc].indexdesc, arrayfile[indexDesc].filedesc, arrayfile[indexDesc].blocks, arrayfile[indexDesc].flag);
      CALL_BF(BF_CloseFile(arrayfile[indexDesc].filedesc));
      removefile(indexDesc);
      return HT_OK;
}

HT_ErrorCode HT_InsertEntry(int indexDesc, Record record, int* tupleid, UpdateRecordArray *updateArray) {
  //insert code here
      int id = record.id;
      int ld, gd, records, number;
      BF_Block* block;
      BF_Block_Init(&block);
      int desc = arrayfile[indexDesc].filedesc;
      CALL_BF(BF_GetBlockCounter(desc, &number));
      CALL_BF(BF_GetBlock(desc, 0, block));
      char* data = BF_Block_GetData(block);
      gd = get_globaldepth(data);//gd = globaldepth
      int blockshash = get_blocksforhashtable(data);//posa block gia to hashtable
      CALL_BF(BF_UnpinBlock(block));
      char* hashed_id  = hash_function(gd, id, 1);//hasharoume to record.id
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
          if(max_records == records){
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
                  rehashrecords(data, data2, data3, newgd, updateArray, desc);
                  //afou ginoun rehashed ta records tou oldblock kanoume insert to neo record
                  free(hashed_id);
                  hashed_id  = hash_function(newgd, id, 1);//ksanaipologizoume to hash_id afou auksithikai to global depth
                  //o neos kouvas proorismou tou neou record
                  newdest = get_bucket_of_index(hashed_id, data3, newgd);
                  if(newdest == oldblock){
                    writerecord(data, record);
                    *tupleid = calculate_tupleid(&record, data);
                  }
                  else if(newdest == newblock){
                    writerecord(data2, record);
                    *tupleid = calculate_tupleid(&record, data2);
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
                  rehashrecords(data, data2, data3, gd, updateArray, desc);
                  //afou ginoun rehashed ta records tou oldblock kanoume insert to neo record
                  int newdest = get_bucket_of_index(hashed_id, data3, gd);//o neos kouvas proorismou tou neou record
                  if(newdest == oldblock){
                    writerecord(data, record);
                    *tupleid = calculate_tupleid(&record, data);
                  }
                  else if(newdest == newblock){
                    writerecord(data2, record);
                    *tupleid = calculate_tupleid(&record, data2);
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
          }
          else{//apli periptosi insert opou iparxei xoros
              writerecord(data, record);  
              *tupleid = calculate_tupleid(&record, data);
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
              if(max_records == records){
                  if(ld == gd){
              //split to hash table kai meta ton kado
                  }   
                  else{
              //split ton kadon
                  }
              }
              else{//apli periptosi insert opou iparxei xoros
                  writerecord(data, record);  
                  BF_Block_SetDirty(block);
                  CALL_BF(BF_UnpinBlock(block)); 
                  BF_Block_Destroy(&block); 
                  *tupleid = calculate_tupleid(&record, data);
                  break;
              }
              CALL_BF(BF_UnpinBlock(block));
              BF_Block_Destroy(&block);
          }
      }
      free(hashed_id);
      return HT_OK;
}

HT_ErrorCode HT_PrintAllEntries(int indexDesc, int *id) {
  //insert code here
      int desc = arrayfile[indexDesc].filedesc;
      char* hashed_id;
      if(id == NULL){//ektiponoume oles tis eggrafes
          printf("printing all records in the file\n");
          int totalblocks;
          BF_GetBlockCounter(desc, &totalblocks);
          char* data;
          BF_Block* block;
          BF_Block_Init(&block);
          CALL_BF(BF_GetBlock(desc, 0, block));
          data = BF_Block_GetData(block);
          int hashblocks = get_blocksforhashtable(data);
          int buckets = totalblocks - hashblocks;
          buckets = buckets-1;
          buckets = totalblocks - buckets;
          for(int y=buckets; y<totalblocks; y++){
              BF_Block_Init(&block);
              CALL_BF(BF_GetBlock(desc, y, block));
              data = BF_Block_GetData(block);
              printbucket(data);
              CALL_BF(BF_UnpinBlock(block));
              BF_Block_Destroy(&block);
          }
      }
      else{
          printf("printing only records with id=%d in the file\n", *id);
          int block;
          BF_Block* bl;
          BF_Block_Init(&bl);
          CALL_BF(BF_GetBlock(desc, 0, bl));
          char* data = BF_Block_GetData(bl);
          int gd = get_globaldepth(data);
          int hashblocks = get_blocksforhashtable(data);
          CALL_BF(BF_UnpinBlock(bl));
          BF_Block_Destroy(&bl);
          hashed_id = hash_function(gd, *id, 1);
          for(int r=1; r<=hashblocks; r++){
              BF_Block_Init(&bl);
              CALL_BF(BF_GetBlock(desc, r, bl));
              data = BF_Block_GetData(bl);
              block = get_bucket_of_index(hashed_id, data, gd);
              CALL_BF(BF_UnpinBlock(bl));
              BF_Block_Destroy(&bl);
              if(block != 0)
                  break;
          }
          BF_Block_Init(&bl);
          CALL_BF(BF_GetBlock(desc, block, bl));
          data = BF_Block_GetData(bl);
          findcertainrecordid(data, *id);
          CALL_BF(BF_UnpinBlock(bl));
          BF_Block_Destroy(&bl);
          free(hashed_id);
      }
      return HT_OK;
}

HT_ErrorCode HT_HashStatistics(char *filename){
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