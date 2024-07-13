# WohnheimDB

This is a generator of schedules for who is on duty on which week in a multi-room co-living/dorm. It automatically generates an excel spreadsheets, which needs a small bit of manual intervention afterwards (deleting the first autogenerated line and widening cells).

## Installation

To run this script you have to first install python. This is the [page](https://www.python.org/downloads/) to do that. After that might you need to install git (https://github.com/git-guides/install-git). After that the steps will depend on your operating system.

### Windows

Copy the following text, open Powershell and paste it there. Afterwards click Enter:

```
pip install sqlite3, pandas
cd /d "%USERPROFILE%\Desktop"
git clone git@github.com:M1ke-Gru/wohnheimDB.git
```

### Mac

Copy the following text, open Terminal and paste it there. Afterwards click Enter:

```
pip install sqlite3, pandas
cd ~/Desktop/
git clone git@github.com:M1ke-Gru/wohnheimDB.git
```

## Usage

### Windows

Paste this into Powershell and press Enter:

```
python %USERPROFILE%\Desktop\wohnheimDB\table_generator.py 
```

Now go to your Desktop, find wohnheimDB folder and there you find your .xls file.

### Mac

Paste this into Terminal and press Enter:

```
python ~\Desktop\wohnheimDB\table_generator.py 
```

Now go to your Desktop, find wohnheimDB folder and there you find your .xls file.

That is it! I hope you enjoy using my script!
