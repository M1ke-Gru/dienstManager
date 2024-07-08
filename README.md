This database is a db for management of who is on duty. It stores the table of dates and rooms of people who are on duty. 

It must be generated every 18 weeks, with the input data being the data about which rooms are currently occupied with people, so that every week there are exactly two people on duty. Also it must be regenerated, when it becomes known that an occupied room became unoccupied and vice-versa. 

A single person must be on duty one out of every every n weeks, where n=(number_of_occupied_rooms/2)+x with -2 <= x <= 1. 

Also there must be a possibility for a person to say that they won't be able to carry out duty during a certain time period and if one of the weeks in that time period he was supposed to be on duty, he will be on duty on the week right after his arrival.
