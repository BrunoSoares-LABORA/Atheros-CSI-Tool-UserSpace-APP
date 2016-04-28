#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libpq-fe.h>
#include <inttypes.h>

#include "csi_fun.h"
#include "csi_persistence.h"

void pg_close(PGconn *conn) {
    PQfinish(conn);
}

int pg_connect(PGconn** conn) {
    int port_len = (int) (floor(log10(abs(PG_PORT))) + 1);
    int str_len = (int) (40 + port_len + strlen(PG_USER) + strlen(PG_PASS) + strlen(PG_DBNAME) + strlen(PG_HOST));
    char connection_string[str_len];

    sprintf(connection_string, "user=%s password=%s dbname=%s hostaddr=%s port=%d",
        PG_USER, PG_PASS, PG_DBNAME, PG_HOST, PG_PORT);

    *conn = PQconnectdb(connection_string);
    if (PQstatus(*conn) == CONNECTION_BAD) {
        printf("ERROR: Could not connect to database: %s", PQerrorMessage(*conn));
        pg_close(*conn);
        return 0;
    }

    return 1;
}

int csi_init_table(PGconn** conn) {
    PGresult *res;
    res = PQexec(*conn,
                 "CREATE TABLE IF NOT EXISTS csi_status(" \
                         "id INTEGER PRIMARY KEY," \
                         "timestamp_nano INTEGER," \
                         "channel INTEGER," \
                         "channel_bw DOUBLE," \
                         "rate INTEGER," \
                         "num_receives INTEGER," \
                         "num_transmitters INTEGER," \
                         "num_subcarriers INTEGER," \
                         "noise INTEGER," \
                         "phy_err INTEGER," \
                         "full_rssi INTEGER," \
                         "rssi_0 INTEGER," \
                         "rssi_1 INTEGER," \
                         "rssi_2 INTEGER," \
                         "payload_len INTEGER," \
                         "csi_len INTEGER," \
                         "buf_len INTEGER" \
                 ")");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not create initial CSI database table: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(*conn);
        return 0;
    }

    PQclear(res);
    return 1;
}

int save_csi_status(PGconn** conn, csi_struct* csi_status) {
    PGresult *res;

    char* insert_query = NULL;
    size_t len;

    len = 1 + (size_t)snprintf(NULL, 0, "INSERT INTO csi_status VALUES(" \
            "DEFAULT,'%" PRIu64"','%d','%f','%d','%d','%d','%d','%d','%d','%d'," \
            "'%d','%d','%d','%d','%d','%d'" \
          ")",
           csi_status->tstamp,
           csi_status->channel,
           (csi_status->chanBW ? 20.0 : 40.0),
           csi_status->rate,
           csi_status->nr,
           csi_status->nc,
           csi_status->num_tones,
           csi_status->noise,
           csi_status->phyerr,
           csi_status->rssi,
           csi_status->rssi_0,
           csi_status->rssi_1,
           csi_status->rssi_2,
           csi_status->payload_len,
           csi_status->csi_len,
           csi_status->buf_len
    );

    insert_query = malloc(len);
    snprintf(insert_query, len, "INSERT INTO csi_status VALUES(" \
            "DEFAULT,'%" PRIu64"','%d','%f','%d','%d','%d','%d','%d','%d','%d'," \
            "'%d','%d','%d','%d','%d','%d'" \
          ")",
             csi_status->tstamp,
             csi_status->channel,
             (!csi_status->chanBW ? 20.0 : 40.0),
             csi_status->rate,
             csi_status->nr,
             csi_status->nc,
             csi_status->num_tones,
             csi_status->noise,
             csi_status->phyerr,
             csi_status->rssi,
             csi_status->rssi_0,
             csi_status->rssi_1,
             csi_status->rssi_2,
             csi_status->payload_len,
             csi_status->csi_len,
             csi_status->buf_len
    );

    printf("Insert query: %s", insert_query);
    res = PQexec(*conn, insert_query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not create initial CSI database table: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(*conn);
        return 0;
    }

    free(insert_query);
    PQclear(res);
    return 1;
}