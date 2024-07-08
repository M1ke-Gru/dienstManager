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

Room *roomRandomizer(int firstRoom, int lastRoom, int *not_occupied,
                     int *weeks_not_on_duty) {
  int s = sizeof(not_occupied) / sizeof(not_occupied[0]);
  qsort(not_occupied, s, sizeof(int), compare);
  int numberOfRooms = lastRoom - firstRoom - sizeof(not_occupied);
  Room rooms[numberOfRooms];
  int r = firstRoom;
  int not_occupied_iterator = 0;
  while (r < sizeof(rooms)) {
    int numberOfPeople =
        lastRoom - firstRoom - sizeof(not_occupied) / sizeof(not_occupied[0]);
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
  qsort(rooms, sizeof(rooms) / sizeof(Room), sizeof(Room), roomComparer);
  return rooms;
}

void populateDBWithRooms(Room *rooms, int day, int month, int year) {
  int r = 0;
  for (int i = 0; i < 10; i++) {
    char stroom1[4];
    sprintf(stroom1, "%d", rooms[r].room_number);
    r++;
    char stroom2[4];
    sprintf(stroom2, "%d", rooms[r].room_number);
    r++;
    char dt_day_str[2];
    char dt_month_str[2];
    char dt_year_str[4];
  }
}
