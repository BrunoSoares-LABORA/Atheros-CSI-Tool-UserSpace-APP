#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libpq-fe.h>
#include <inttypes.h>

#include "csi_fun.h"
#include "csi_persistence.h"

void pg_close(PGconn** conn) {
    if(*conn != NULL) {
        PQfinish(*conn);
        *conn = NULL;
    }
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
        pg_close(conn);
        return 0;
    }

    return 1;
}

int csi_init_table(PGconn** conn) {
    if(*conn == NULL) {
        return 0;
    }

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
                 ");" \
                 "CREATE TABLE IF NOT EXISTS csi_data(" \
                          "id INTEGER PRIMARY KEY" \
                          "timestamp_nano INTEGER" \
                          "status_id INTEGER" \
                          "antenna INTEGER" \
                          "subcarrier INTEGER" \
                          "rc_1_real INTEGER" \
                          "rc_1_imag INTEGER" \
                          "rc_1_rssi INTEGER" \
                          "rc_2_real INTEGER" \
                          "rc_2_imag INTEGER" \
                          "rc_2_rssi INTEGER" \
                          "rc_3_real INTEGER" \
                          "rc_3_imag INTEGER" \
                          "rc_3_rssi INTEGER" \
                 ");");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not create initial CSI database table: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(conn);
        return 0;
    }

    PQclear(res);
    return 1;
}

int save_csi_status(PGconn** conn, csi_struct* csi_status) {
    if(*conn == NULL) {
        return 0;
    }

    PGresult *res;

    char* insert_query = NULL;
    size_t len;

    len = 1 + (size_t)create_csi_status_insert_query(NULL, 0, csi_status);
    insert_query = malloc(len);
    create_csi_status_insert_query(insert_query, len, csi_status);

    res = PQexec(*conn, insert_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not insert CSI status: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(conn);
        return 0;
    }

    free(insert_query);
    PQclear(res);
    return 1;
}

int create_csi_status_insert_query(char *insert_query, size_t max_len, csi_struct* csi_status) {
    return snprintf(insert_query, max_len, "INSERT INTO csi_status VALUES(" \
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
}

int save_csi_matrix(PGconn** conn, int csi_status_id, csi_struct* csi_status, COMPLEX(* csi_matrix)[CSI_NC][CSI_MAX_SUBCARRIERS]) {
    int subcarrier, antenna, nc;

    int fixed_insert_len = 90 + ((int) (floor(log10(abs(csi_status_id))) + 1)) +
            ((int) (floor(log10(abs(csi_status->tstamp))) + 1));
    int subcarrier_len = 0;
    int antenna_len = 0;
    char* insert_query = NULL;

    "INSERT INTO csi_data VALUES(DEFAULT,'%" PRIu64"','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d')"

    for(subcarrier = 0; subcarrier < csi_status->num_tones; subcarrier++) {
        subcarrier_len = (int) (floor(log10(abs(subcarrier))) + 1);
        for(antenna = 0; antenna < csi_status->nr; antenna++) {
            antenna_len = (int) (floor(log10(abs(antenna))) + 1);
            int rc_len = 0;
            for(nc = 0; nc < csi_status->nc; nc++) {
                // +2 represents rssi NOT IMPLEMENTED
                rc_len += (((int) (floor(log10(abs(csi_matrix[antenna][nc][subcarrier].real))) + 1)) +
                        ((int) (floor(log10(abs(csi_matrix[antenna][nc][subcarrier].imag))) + 1)) + 2);
            }

            size_t len = (size_t) (fixed_insert_len + subcarrier_len + antenna_len + rc_len);
            insert_query = malloc(len);
            sprintf(insert_query, "INSERT INTO csi_data VALUES(DEFAULT,'%" PRIu64"','%d','%d','%d'",
                    csi_status->tstamp,
                    csi_status_id,
                    antenna,
                    subcarrier
            );

            char aux[250];
            for(nc = 0; nc < csi_status->nc; nc++) {
                sprintf(aux, ",'%d','%d','%d'", csi_matrix[antenna][nc][subcarrier].real,
                        csi_matrix[antenna][nc][subcarrier].imag, 0);
                strcat(insert_query, aux);
            }

            printf(insert_query);

            free(insert_query);
        }
    }

    return 1;
}