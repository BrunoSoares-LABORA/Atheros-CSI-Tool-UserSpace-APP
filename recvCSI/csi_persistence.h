#define PG_USER     "postgres"
#define PG_PASS     "postgres"
#define PG_DBNAME   "postgres"
#define PG_HOST     "127.0.0.1"
#define PG_PORT     5432

int pg_connect(PGconn** conn);
void pg_close(PGconn** conn);
int csi_init_table(PGconn** conn);
int save_csi_status(PGconn** conn, csi_struct* csi_status);