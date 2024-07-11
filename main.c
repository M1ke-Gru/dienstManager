#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int dbCreator(sqlite3 *db, int rc);
int whoIsOnDuty(sqlite3 *db, int rc) { return 0; }

int main() {
  sqlite3 *db;
  char *zErrMsg = NULL;
  int rc;

  rc = sqlite3_open("database.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return (1);
  }
  int db_create = dbCreator(db, rc);
  if (db_create) {
    fprintf(stderr, "Error when creating the database: %s\n",
            sqlite3_errmsg(db));
    return 1;
  }
  sqlite3_close(db);
  return 0;
}

int dutyDBCreator(sqlite3 *db, int rc) {
  char *createTableQuery =
      "CREATE TABLE IF NOT EXISTS duty_week (week_start DATE PRIMARY KEY, "
      "room_1 Integer, room_2 INTEGER)";
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(db, createTableQuery, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Prepare error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return 0;
}

typedef struct room {
  int priority;
  int room_number;
} Room;

int compare(const void *a, const void *b) {
  int first = *(int *)a;
  int second = *(int *)b;
  return (first - second);
}

int roomComparer(const void *r1, const void *r2) {
  const Room *pr1 = r1;
  const Room *pr2 = r2;
  return (pr1->priority - pr2->priority);
}

Room *roomRandomizer(sqlite3 *db, Room rooms[], size_t rooms_size,
                     int firstRoom, int lastRoom, int *not_occupied,
                     int not_occupied_quantity, int *weeks_not_on_duty) {

  // Generating who was not on the duty for how long from the existing sql data.
  char *sql = "SELECT week_start FROM duty_week WHERE condition";

  sqlite3_stmt *stmt;
  int prep = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  char *week_beginning_array[firstRoom - lastRoom - not_occupied_quantity];
  while ((prep = sqlite3_step(stmt)) == SQLITE_ROW) {
    char *week_begining = sqlite3_column_text(stmt, 0);
  }

  // Starting to
  qsort(not_occupied, not_occupied_quantity, sizeof(int), compare);
  int numberOfRooms = lastRoom - firstRoom - sizeof(not_occupied);
  int r = firstRoom;
  int not_occupied_iterator = 0;
  while (r < rooms_size) {
    int numberOfPeople =
        lastRoom - firstRoom - not_occupied_quantity / sizeof(not_occupied[0]);
    int currentRoom = firstRoom + r;
    if (not_occupied[not_occupied_iterator] != currentRoom) {
      int rooms_iter = 0;
      int week_assign_priority = weeks_not_on_duty[r];
      srand(time(NULL));
      float priority = (numberOfPeople / 2) - weeks_not_on_duty[r] +
                       ((float)(rand() % 400) / 100 - 2);
      Room roomStruct;
      roomStruct.priority = priority;
      roomStruct.room_number = currentRoom;
      rooms[r] = roomStruct;
    } else {
      not_occupied_iterator++;
    }
    r++;
  }
  qsort(rooms, rooms_size / sizeof(Room), sizeof(Room), roomComparer);
  return rooms;
}

int populateDBWithRooms(sqlite3 *db, Room rooms[], size_t rsize, int day,
                        int month, int year) {
  for (int i = 0; i < 10; i++) {
    time_t rawtime;
    char formatted_time[80];
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    strftime(formatted_time, sizeof(formatted_time), "%d-%m-Y", timeinfo);
    const char *insertionQuery =
        "INSERT INTO duty_week (week_start, room_1, room_2) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, insertionQuery, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 1;
    }
    sqlite3_bind_text(stmt, 1, formatted_time, -1, SQLITE_TRANSIENT);

    sqlite3_bind_int(stmt, 2, rooms[0].room_number);
    rooms[0].priority =
        (rsize / sizeof(rooms[0]) / 2) + ((float)(rand() % 400) / 100 - 2);
    qsort(rooms, rsize / sizeof(Room), sizeof(Room), roomComparer);

    sqlite3_bind_int(stmt, 3, rooms[0].room_number);
    rooms[0].priority =
        (rsize / sizeof(rooms[0]) / 2) + ((float)(rand() % 400) / 100 - 2);
    qsort(rooms, rsize / sizeof(Room), sizeof(Room), roomComparer);

    int exe = sqlite3_step(stmt);
    if (exe != SQLITE_DONE) {
      fprintf(stderr, "Error inserting data: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 1;
    }
    sqlite3_finalize(stmt);
  }

  fprintf(stdout, "Data inserted successfully.\n");
  return 0;
}
