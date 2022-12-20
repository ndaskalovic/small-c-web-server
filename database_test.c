#include <sqlite3.h>
#include <stdio.h>

int main(void)
{

    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    char *sql = "DROP TABLE IF EXISTS Bookings;"
                "CREATE TABLE Bookings(Date TEXT, Time TEXT, Duration Text);"
                "INSERT INTO Bookings VALUES('2022-12-25', '16:30', '60');"
                "INSERT INTO Bookings VALUES('2022-12-25', '09:00', '60');";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    

    int last_id = sqlite3_last_insert_rowid(db);
    printf("The last Id of the inserted row is %d\n", last_id);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }

    sqlite3_close(db);

    return 0;
}