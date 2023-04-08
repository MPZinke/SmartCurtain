

from datetime import datetime
import os
from sqlalchemy import create_engine, func, select, update, MetaData
from sqlalchemy.orm import Session, relationship
from sqlalchemy.ext.automap import automap_base
from typing import Optional


DB_USER: str = os.getenv("SMARTCURTAIN_DB_USER");
DB_HOST: str = os.getenv("SMARTCURTAIN_DB_HOST");
DB_PASSWORD: str = os.getenv("SMARTCURTAIN_DB_PASSWORD");


# ————————————————————————————————————————————————— OBJECT  CREATION ————————————————————————————————————————————————— #

# FROM: https://docs.sqlalchemy.org/en/20/orm/extensions/automap.html
ENGINE = create_engine(f"postgresql+psycopg2://{DB_USER}:{DB_PASSWORD}@{DB_HOST}/SmartCurtain")
Base = automap_base()
Base.prepare(autoload_with=ENGINE)


Options = Base.classes.Options
Homes = Base.classes.Homes
HomesOptions = Base.classes.HomesOptions
Rooms = Base.classes.Rooms
RoomsOptions = Base.classes.RoomsOptions
Curtains = Base.classes.Curtains
CurtainsEvents = Base.classes.CurtainsEvents
CurtainsOptions = Base.classes.CurtainsOptions


def Homes__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"is_deleted": self.is_deleted,
		"name": self.name,
		"HomesOptions": list(map(dict, self.homesoptions_collection)),
		"Rooms": list(map(dict, self.rooms_collection))
	}.items()


def Rooms__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"is_deleted": self.is_deleted,
		"name": self.name,
		"RoomsOptions": list(map(dict, self.roomsoptions_collection)),
		"Curtains": list(map(dict, self.curtains_collection))
	}.items()


def Curtains__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"buffer_time": self.buffer_time,
		"direction": self.direction,
		"is_deleted": self.is_deleted,
		"length": self.length,
		"name": self.name,
		"CurtainsEvents": list(map(dict, self.curtainsevents_collection)),
		"CurtainsOptions": list(map(dict, self.curtainsoptions_collection))
	}.items()


def Options__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"description": self.description,
		"is_deleted": self.is_deleted,
		"name": self.name
	}.items()


def AreaOptions__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"Option": dict(self.options),
		"data": self.data,
		"is_on": self.is_on,
		"notes": self.notes
	}.items()



def CurtainsEvents__iter__(self) -> dict:
	yield from {
		"id": self.id,
		"is_activated": self.is_activated,
		"is_deleted": self.is_deleted,
		"percentage": self.percentage,
		"time": self.time,
		"Option": dict(self.options)
	}.items()


Options.__iter__ = Options__iter__
Homes.__iter__ = Homes__iter__
HomesOptions.__iter__ = AreaOptions__iter__
Rooms.__iter__ = Rooms__iter__
RoomsOptions.__iter__ = AreaOptions__iter__
Curtains.__iter__ = Curtains__iter__
CurtainsEvents.__iter__ = CurtainsEvents__iter__
CurtainsOptions.__iter__ = AreaOptions__iter__


def SELECT_Homes() -> list:
	with Session(ENGINE) as session:
		statement = select(Homes)
		return list(map(dict, session.scalars(statement)))


def SELECT_Rooms() -> list:
	with Session(ENGINE) as session:
		statement = select(Rooms)
		return list(map(dict, session.scalars(statement)))


def SELECT_Curtains() -> list:
	with Session(ENGINE) as session:
		statement = select(Curtains)
		return list(map(dict, session.scalars(statement)))


def SELECT_Options() -> list:
	with Session(ENGINE) as session:
		statement = select(Options)
		return list(map(dict, session.scalars(statement)))


def UPDATE_CurtainsEvents(id: int, *, is_activated: Optional[bool]=None, is_deleted: Optional[bool]=None,
  percentage: Optional[int]=None, time: Optional[datetime]=None, **kwargs: dict
):
	if(any(key != "Option" for key in kwargs)):
		raise Exception(f"""{", ".join([key for key in kwargs if(key != "Options")])} are not allowed""")

	with Session(ENGINE) as session:
		statement = update(CurtainsEvents).where(CurtainsEvents.id == id)
		# if((curtain_event := session.scalar(select(CurtainsEvents))) is None):
		# 	raise Exception(f"Could not find CurtainsEvents by id {id}")

		if(is_activated is not None):
			statement = statement.values(is_activated=is_activated)
		if(is_deleted is not None):
			statement = statement.values(is_deleted=is_deleted)
		if(percentage is not None):
			statement = statement.values(percentage=percentage)
		if(time is not None):
			statement = statement.values(time=time)
		if("Option" in kwargs):
			statement = statement.values(**{"Options.id": kwargs["Option"].id()})

		session.execute(statement)
		session.commit()


def test():
	import json
	# print(json.dumps(SELECT_Homes(), default=str, indent=4))
	UPDATE_CurtainsEvents(1, is_activated=True, is_deleted=True)


if(__name__ == "__main__"):
	test()
