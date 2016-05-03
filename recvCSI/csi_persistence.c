#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <libpq-fe.h>

#include "csi_fun.h"
#include "csi_persistence.h"

int num_digits(int number) {
    return long_num_digits(number);
}

int long_num_digits(long number) {
    int digits = 0;
    if(number < 0) {
        number = number * -1;
        digits++;
    }

    if(number == 0) return 1;
    while (number > 0) {
        number /= 10;
        digits++;
    }

    return digits;
}

void pg_close(PGconn** conn) {
    if(*conn != NULL) {
        PQfinish(*conn);
        *conn = NULL;
    }
}

int pg_connect(PGconn** conn) {
    int port_len = num_digits(PG_PORT);
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

    char* init_tables_query = NULL;
    init_tables_query = malloc(1200*(sizeof(char)));
    sprintf(init_tables_query,
                 "CREATE SEQUENCE csi_status_id_seq " \
                         "START WITH 1 " \
                         "INCREMENT BY 1 " \
                         "NO MINVALUE " \
                         "NO MAXVALUE " \
                         "CACHE 1; " \
                 "ALTER TABLE public.csi_status_id_seq OWNER TO %s; " \
                 "CREATE SEQUENCE csi_data_id_seq " \
                         "START WITH 1 " \
                         "INCREMENT BY 1 " \
                         "NO MINVALUE " \
                         "NO MAXVALUE " \
                         "CACHE 1; " \
                 "ALTER TABLE public.csi_data_id_seq OWNER TO %s;",
            PG_USER,
            PG_USER
    );

    PGresult *res;
    res = PQexec(*conn, init_tables_query);
    PQclear(res);

    sprintf(init_tables_query,
                 "CREATE TABLE IF NOT EXISTS csi_status(" \
                         "id INTEGER PRIMARY KEY DEFAULT nextval('csi_status_id_seq'::regclass)," \
                         "timestamp_milli BIGINT," \
                         "channel INTEGER," \
                         "channel_bw REAL," \
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
                          "id INTEGER PRIMARY KEY DEFAULT nextval('csi_data_id_seq'::regclass)," \
                          "timestamp_milli BIGINT," \
                          "status_id INTEGER," \
                          "antenna INTEGER," \
                          "subcarrier INTEGER," \
                          "rc_1_real INTEGER," \
                          "rc_1_imag INTEGER," \
                          "rc_1_rssi INTEGER," \
                          "rc_2_real INTEGER," \
                          "rc_2_imag INTEGER," \
                          "rc_2_rssi INTEGER," \
                          "rc_3_real INTEGER," \
                          "rc_3_imag INTEGER," \
                          "rc_3_rssi INTEGER" \
                 ");"
    );

    res = PQexec(*conn, init_tables_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not create initial CSI database table: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(conn);
        return 0;
    }

    free(init_tables_query);
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

    res = PQexec(*conn, "SELECT id FROM csi_status ORDER BY id DESC LIMIT 1");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("ERROR: Could not retrieve latest CSI status registered: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(conn);
        return 0;
    }

    int csi_status_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    printf("INSERT ID: %d\n", csi_status_id);

    return csi_status_id;
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
    if(*conn == NULL) {
        return 0;
    }

    int subcarrier, antenna, nc;

    // max possible lenght
    int max_len = 27 + ((csi_status->num_tones * csi_status->nr) * 122);
    char* insert_query = NULL;
    insert_query = malloc((sizeof(char) * max_len));
    sprintf(insert_query, "INSERT INTO csi_data VALUES");

    for(subcarrier = 0; subcarrier < csi_status->num_tones; subcarrier++) {
        for(antenna = 0; antenna < csi_status->nr; antenna++) {
            sprintf(insert_query, "%s (DEFAULT,'%" PRIu64"','%d','%d','%d'",
                    insert_query,
                    csi_status->tstamp,
                    csi_status_id,
                    (antenna + 1),
                    (subcarrier + 1)
            );

            for(nc = 0; nc < csi_status->nc; nc++) {
                sprintf(insert_query, "%s,'%d','%d','%d'",
                        insert_query,
                        csi_matrix[antenna][nc][subcarrier].real,
                        csi_matrix[antenna][nc][subcarrier].imag,
                        0
                );
            }

            if((subcarrier+1) == csi_status->num_tones && (antenna+1) == csi_status->nr) {
                strcat(insert_query, ");");
            } else {
                strcat(insert_query, "),");
            }
        }
    }

    PGresult *res;
    res = PQexec(*conn, insert_query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("ERROR: Could not insert CSI matrix: %s", PQerrorMessage(*conn));
        PQclear(res);
        pg_close(conn);
        return 0;
    }

    free(insert_query);
    PQclear(res);
    return 1;
}

int save_csi(PGconn** conn, csi_struct* csi_status, COMPLEX(* csi_matrix)[CSI_NC][CSI_MAX_SUBCARRIERS]) {
    int csi_status_id = save_csi_status(conn, csi_status);
    if(!csi_status_id) {
        return 0;
    }

    if(!save_csi_matrix(conn, csi_status_id, csi_status, csi_matrix)) {
        return 0;
    }

    return 1;
}