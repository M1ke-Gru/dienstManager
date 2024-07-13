import sqlite3
import pandas as pd
import glob
import os
import random
from datetime import datetime, timedelta


class Room:
    def __init__(self, number, cursor):
        # After intialization run findPriority() with all the necessary arguments
        self.last_week_on_duty = None
        self.number = number
        self.priority = 0
        self.cursor = cursor

    def findPriority(self, numOfRooms, alreadyInThisDB=False):
        if not alreadyInThisDB:
            self.cursor.execute(
                f"select week_number from rooms_on_duty where room_1 = {self.number} or room_2 = {self.number}"
            )
            last_week_on_duty = self.cursor.fetchall()
            self.cursor.execute("select COUNT(*) from rooms_on_duty")
            number_of_weeks_last_db = self.cursor.fetchall()[0][0]
            weeks_not_on_duty = 0
            if len(last_week_on_duty) > 0:
                weeks_not_on_duty = number_of_weeks_last_db - max(last_week_on_duty)[0]
            else:
                weeks_not_on_duty = random.randint(0, number_of_weeks_last_db)
            self.priority = numOfRooms / 2 - weeks_not_on_duty + random.randrange(-2, 2)
        else:
            self.priority = numOfRooms / 2 + random.randrange(-2, 2)

        return self.priority


class DutySchedule:
    def __init__(
        self,
        froom,
        lroom,
        empty_rooms,
        num_of_weeks,
        db_name="database",
    ):
        for file in glob.glob("*.db"):
            os.remove(file)
        self.db_name = f"{db_name}-{datetime.now().date().strftime("%d.%m.%Y")}.db"
        conn = sqlite3.connect(self.db_name)
        cursor = conn.cursor()

        cursor.execute("""CREATE TABLE IF NOT EXISTS rooms_on_duty (
            week_number INTEGER PRIMARY KEY,
            week_start TEXT,
            room_1 INTEGER,
            room_2 INTEGER
        )""")

        self.rooms = []
        for i in range(froom, lroom + 1):
            if i not in empty_rooms:
                self.rooms.append(Room(i, cursor))
        dt = datetime.now().date()
        self.first_days_of_weeks = []
        for _ in range(num_of_weeks):
            dt += timedelta(days=7 - dt.weekday())
            self.first_days_of_weeks.append(dt.strftime("%d.%m.%Y"))
        self.empty_rooms = empty_rooms
        for r in self.rooms:
            r.findPriority(lroom - froom - len(self.empty_rooms))
        self.insertTuplesIntoDB(cursor)
        conn.commit()
        conn.close()

    def lowerPriorityTwoRooms(self, sorted_rooms):
        rooms_to_lower = sorted_rooms[:2]
        for room in rooms_to_lower:
            room.findPriority(len(self.empty_rooms))

        sorted_rooms.sort(key=lambda x: x.priority, reversed=True)

        return sorted_rooms

    def lowerRoomPriority(self, room, sorted_rooms):
        room.findPriority(len(self.empty_rooms))
        for srpos in range(len(sorted_rooms)):
            if sorted_rooms[srpos].priority > room.priority:
                sorted_rooms.insert(srpos, room)
        return sorted_rooms

    def createWeekTuples(self):
        week_tuples_list = []
        sorted_rooms = [r for r in self.rooms]
        for idx, date in enumerate(self.first_days_of_weeks):
            sorted_rooms.sort(key=lambda x: x.priority)
            tuple_of_the_week = (
                idx + 1,
                date,
                sorted_rooms[0].number,
                sorted_rooms[1].number,
            )
            week_tuples_list.append(tuple_of_the_week)
            for r in sorted_rooms:
                r.priority -= 1
            sorted_rooms[0].findPriority(len(sorted_rooms), alreadyInThisDB=True)
            sorted_rooms[1].findPriority(len(sorted_rooms), alreadyInThisDB=True)
            sorted_rooms = sorted_rooms[2:] + sorted_rooms[:2]
        return week_tuples_list

    def insertTuplesIntoDB(self, cursor):
        insertQuery = """
            insert into rooms_on_duty (week_number, week_start, room_1, room_2) values (?, ?, ?, ?)
        """
        week_tuples_list = self.createWeekTuples()
        for week in week_tuples_list:
            cursor.execute(insertQuery, week)

    def create_duty_xls(self):
        conn = sqlite3.connect(self.db_name)
        cursor = conn.cursor()
        query = "SELECT * FROM rooms_on_duty"
        df = pd.read_sql(query, conn)
        df.to_excel("output.xlsx")

        cursor.close()


def runTheScript():
    print(
        "This is a script for generating an excel spreadsheet of which rooms should be on duty on which week."
    )
    froom = int(input("Enter the first room number:"))
    lroom = int(input("Enter the last room number:"))
    empty_rooms = []

    while True:
        user_input = input(
            "Now enter the numbers of empty rooms. Enter 'exit' if you have entered all the empty rooms: "
        )
        if user_input.lower() == "exit":
            break
        try:
            room_number = int(user_input)
            empty_rooms.append(room_number)
        except ValueError:
            print("Please enter a valid integer or 'exit' to finish.")
    number_of_weeks = int(
        input("Enter the number of weeks to generate the spreadsheet for: ")
    )
    empty_rooms = empty_rooms[:-1]
    print("That's it! Generating schedule...")
    ds = DutySchedule(
        empty_rooms=empty_rooms, froom=froom, lroom=lroom, num_of_weeks=number_of_weeks
    )
    print("Generating excel spreadsheet")
    ds.create_duty_xls()


runTheScript()
