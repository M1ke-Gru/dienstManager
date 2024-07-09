At the start I need to get how many weeks before each room was on duty. For that I first choose a room and return all of the dates on which this room was last on duty. After that: weeks_not_on_duty=(current_day-duty_week_first_day)/7
week_assign_priority=(number_of_people)/2-weeks_not_on_duty+x, where -2<=x<=2
room=(week_assign_priority, room_number);
rooms().insert(room)
rooms.sort(week_assign_priority)
for r in rooms:
  insert_into_db(r)
