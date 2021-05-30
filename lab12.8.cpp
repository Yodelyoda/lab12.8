#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <string>
#include <fstream>
#include <Windows.h>

using namespace std;

class TRoute
{
public:
	enum ERoute
	{
		RT_NONE,
		RT_FORWARD,
		RT_BACKWARD
	};

	TRoute(unsigned allTime);
	~TRoute();

	TRoute& PushAfter(const string& name);
	TRoute& PushBefore(const string& name);
	TRoute& Pop();

	string GetName() const;
	unsigned GetAllTime() const;

	TRoute& GoTo(const std::string& name);

	bool IsEmpty() const;

	void Clear();

	ERoute GetMinimal(const string& name1, const string& name2) const;

	unsigned GetTimeForward(const string& name1, const string& name2) const;
	unsigned GetTimeBackward(const string& name1, const string& name2) const;

	string GetWayForward(const string& name1, const string& name2);
	string GetWayBackward(const string& name1, const string& name2);

	friend std::ostream& operator << (std::ostream& ost, const TRoute& route);

protected:
	struct TStation
	{
		string name;
		unsigned tmPrev;
		unsigned tmNext;
		TStation* prev;
		TStation* next;
	}*station;

	unsigned allTime;

protected:
	TRoute(const TRoute&);

	TStation* Find(const string& name) const;
};


TRoute::TRoute(unsigned allTime) : station(NULL), allTime(allTime)
{
	;
}

TRoute::~TRoute()
{
	Clear();
}

TRoute::TStation* TRoute::Find(const string& name) const
{
	TStation* node = station;

	if (node)
	{
		for (; (node->next != station) && (node->name != name); node = node->next) { ; }

		if (node->name != name)
		{
			node = NULL;
		}
	}

	return node;
}

TRoute& TRoute::PushAfter(const string& name)
{
	if (station)
	{
		unsigned tmNew = (station->tmNext / 2);

		TStation * a = station;
		TStation * b = station->next;

		TStation * node = new TStation;
		node->name = name;
		node->tmPrev = node->tmNext = tmNew;
		node->next = b;
		node->prev = a;

		a->next = b->prev = node;
		a->tmNext = b->tmPrev = tmNew;
	}
	else
	{
		station = new TStation;
		station->name = name;
		station->tmPrev = station->tmNext = allTime;
		station->next = station->prev = station;
	}

	return *this;
}

TRoute& TRoute::PushBefore(const string & name)
{
	if (station)
	{
		station = station->prev;
	}
	PushAfter(name);

	station = station->next->next;

	return *this;
}

TRoute& TRoute::Pop()
{
	if (station)
	{
		if (station->next != station)
		{
			TStation* a = station->prev;
			TStation* b = station->next;

			unsigned tmNew = a->tmNext + b->tmPrev;
			a->tmNext = b->tmPrev = tmNew;
			a->next = b;
			b->prev = a;

			delete station;
			station = b;
		}
		else
		{
			delete station;
			station = NULL;
		}
	}

	return *this;
}

string TRoute::GetName() const
{
	return station ? station->name : string();
}

unsigned TRoute::GetAllTime() const
{
	return allTime;
}

TRoute& TRoute::GoTo(const string & name)
{
	if (station)
	{
		TStation* current = station;
		for (; (current->next != station) && (name != current->name)
			; current = current->next) {
			;
		}

		if (current->name == name)
		{
			station = current;
		}
	}

	return *this;
}

bool TRoute::IsEmpty() const
{
	return (station == NULL);
}

void TRoute::Clear()
{
	for (; !IsEmpty(); Pop()) { ; }
}

unsigned TRoute::GetTimeForward(const string & name1, const string & name2) const
{
	unsigned tm = 0;

	if (station)
	{
		TStation* start = Find(name1);

		if (start)
		{
			TStation* node = start;
			for (; (node->next != start) && (node->name != name2); node = node->next)
			{
				tm += node->tmNext;
			}

			if (node->name != name2)
			{
				tm = 0;
			}
		}
	}

	return tm;
}

unsigned TRoute::GetTimeBackward(const string & name1, const string & name2) const
{
	return GetTimeForward(name2, name1);
}

TRoute::ERoute TRoute::GetMinimal(const string & name1, const string & name2) const
{
	TRoute::ERoute route = RT_NONE;

	unsigned forward = GetTimeForward(name1, name2);
	unsigned backward = GetTimeBackward(name1, name2);

	if (forward && backward)
	{
		route = (forward < backward) ? RT_FORWARD : RT_BACKWARD;
	}

	return route;
}

string TRoute::GetWayForward(const string & name1, const string & name2)
{
	TStation* a = Find(name1);
	TStation* b = Find(name2);

	stringstream ssout;

	if (a && b)
	{
		for (; (a != b); a = a->next)
		{
			ssout << a->name << " <" << a->tmNext << "> ";
		}
		ssout << a->name;
	}

	return ssout.str();
}

string TRoute::GetWayBackward(const string & name1, const string & name2)
{
	TStation* a = Find(name1);
	TStation* b = Find(name2);

	stringstream ssout;

	if (a && b)
	{
		for (; (a != b); a = a->prev)
		{
			ssout << a->name << " <" << a->tmPrev << "> ";
		}
		ssout << a->name;
	}

	return ssout.str();
}

ostream& operator << (ostream & ost, const TRoute & route)
{
	if (route.station)
	{
		TRoute::TStation* node = route.station;

		ost << node->name;

		for (node = node->next; node->next != route.station->next; node = node->next)
		{
			ost << " <" << node->tmPrev << "> " << node->name;
		}

		ost << " <" << node->tmPrev << "> " << node->name;
	}

	return ost;
}


int main()
{
	TRoute route(180);

	route.PushAfter("A");
	route.PushAfter("B");
	route.GoTo("B").PushAfter("C");
	route.GoTo("A").PushBefore("D");


	cout << route << std::endl;

	cout << "minimal from A to C: ";

	switch (route.GetMinimal("A", "C"))
	{
	case (TRoute::RT_FORWARD):
		std::cout << route.GetWayForward("A", "C") << " = "
			<< route.GetTimeForward("A", "C") << std::endl;
		break;

	case (TRoute::RT_BACKWARD):
		std::cout << route.GetWayBackward("A", "C") << " = "
			<< route.GetTimeBackward("A", "C") << std::endl;
		break;

	default:
		std::cerr << "way not found ..." << std::endl;
	}

	system("pause");

	return(0);
}