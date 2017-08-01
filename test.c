#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lmdb.h"

#define BUF_SIZE 1024
#define CACHE_SIZE 1UL * 1024UL * 1024UL * 1024UL //1gb

#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define CHECK(test, msg) ((test) ? (void)0 : ((void)fprintf(stderr, \
    "%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))

/* Must create somedb folder first */
#define DBDIR "./somedb"


/* Put the key to db */
int putdb(char *, char *);

/* Get the key from db */
char* getdb(char *);

/* Check if key exists: 1=yes 0=no*/
int checkdb(char *);

int main(int argc, char * argv[])
{
	//test simple write/read
    putdb("testkey", "value");
    putdb("testkey", "value1");
	
	char* val = getdb("testkey");
    printf("testkey %s\n", val);
	free(val);
	
	int status = checkdb("testkey");
	printf("status %i\n", status);
	status = checkdb("falsekey");
	printf("status %i\n", status);
}


/* Put to db */
int putdb(char *keyvalue, char *value) {

    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val key, data, rdata;
    MDB_cursor *cursor;
    char sval[32];

    //printf("put %s %s\n",keyvalue, value);

    E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, 0, 0664));

    /* Put some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

    key.mv_data = keyvalue;
    key.mv_size = strlen(keyvalue);
    data.mv_data = value;
    data.mv_size = strlen(value);

    E(mdb_put(txn, dbi, &key, &data, 0));

    E(mdb_txn_commit(txn));

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);

    return 0;
}

int checkdb(char *thekey) {
	int rc;
	int status = 0;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val key, data, rdata;
    MDB_cursor *cursor;

    //printf("get %s\n", thekey);

    E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, 0, 0664));

    /* Get some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

    key.mv_data = thekey;
    key.mv_size = strlen(thekey);

    rc = mdb_get(txn, dbi, &key, &data);
    if (!rc) {
        status = 1;
    } else {
        status = 0;
    }

    //printf("%s %d, %s %d\n", (char *) key.mv_data, key.mv_size, (char *)data.mv_data, data.mv_size);

    E(mdb_txn_commit(txn));


    mdb_dbi_close(env, dbi);
    mdb_env_close(env);

    return status;
}

/* Get from lmdb */
char* getdb(char *thekey) {

    int rc;
    MDB_env *env;
    MDB_dbi dbi;
    MDB_txn *txn;
    MDB_val key, data, rdata;
    MDB_cursor *cursor;
	char *valuen = NULL;

    //printf("get %s\n", thekey);

    E(mdb_env_create(&env));

    /* Set the cache size */
    E(mdb_env_set_mapsize(env, CACHE_SIZE));
    E(mdb_env_open(env, DBDIR, 0, 0664));

    /* Get some data */
    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));

    key.mv_data = thekey;
    key.mv_size = strlen(thekey);

    rc = mdb_get(txn, dbi, &key, &data);
    if (!rc) {
        valuen = (char *) malloc(data.mv_size + 1);
        memcpy(valuen, data.mv_data, data.mv_size);
        valuen[data.mv_size] = 0;

        //printf("%s %s\n", (char *) key.mv_data, valuen);

        //free(valuen);
    } else {
        printf("No such key\n");
    }

    //printf("%s %d, %s %d\n", (char *) key.mv_data, key.mv_size, (char *)data.mv_data, data.mv_size);

    E(mdb_txn_commit(txn));


    mdb_dbi_close(env, dbi);
    mdb_env_close(env);

    return valuen;
}

