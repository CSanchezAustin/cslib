/* 
*+----------------------------------------------------------------------
*| File.........: DATECLS.CPP
*| Date.........: Sat  09-25-1993
*| Author.......: James M. Curran, et al
*| Version......: 5.0   Compile w/MSVC++ 1.0  / or Borland C++ 3.1
*| Usage........: General purpose date conversion, arithmetic,
*|              :    comparison, and formatting class
*|
*| See DATE.H for acknowledgements and compile/link notes.
// Copyright 2011 Chris Sanchez
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// ====================================================================
//
*+----------------------------------------------------------------------
*/
#include <cslib.h>

#include <lang/Date.h>

#include <ctype.h>

//--------------------- Class Varaibles ------------------------------------------------

unsigned int   	Date::_startDST=((Date::APRIL<<8) + 1);
unsigned int   	Date::_startSTD=((Date::OCTOBER<<8) + 31);


	

//#if !defined(_INC_WINDOWS)	// if this is NOT a windows program

                  
// See last of these blocks for English names & comments
                   
#if defined(_DEUTSCHEN)	//--------------------------------------------------------------------

#if 0
		static const char *dayname[] = {"Sonntag","Montag","Dienstag",
					"Mittwoch","Donnerstag","Freitag","Samstag"} ;

		static const char *mname[] = {"Januar","Februar","Marz","April","Mai",
				   	"Juni","Juli","August","September","Oktober","November",
					"Dezember"};
		static const char	szInvalidDay[] 		= "Invalide Tag";
		static const char	szInvalidDate[] 	= "Invalide Datum";
		static const char	szInvalidMonth[]	= "Invalide Monat";
		static const char	szBCE[]				= " B.C.E.";
		static const char	szYears[]			= "Jahre";
		static const char	szMonths[]			= "Monate";
		static const char	szDays[]			= "Daten";
#endif

#elif defined(_ESPANOL)  //------------------------------------------------------------------


#if 0
		static const *char dayname[]= {"Domingo","Lunes","Martes",
					"Miercoles","Jueves","Viernes","Sabado"} ;

		static const char *mname[] = {"Enero","Febrero","Marzo","Abril",
					"Mayo","Junio","Julio","Agosto","Septiembre","Octubre",
					"Noviembre","Diciembre"};
		static const char	szInvalidDay[] 		= "Invalido dia";
		static const char	szInvalidDate[] 	= "Invalido data";
		static const char	szInvalidMonth[]	= "Invalido mes";
		static const char	szBCE[]				= " B.C.E.";
		static const char	szYears[]			= "anos";
		static const char	szMonths[]			= "mes";
		static const char	szDays[]			= "dia";
#endif

#elif defined (_FRANCAIS)

#if 0
		static const char *dayname[]= {"Dimanche","Lundi","Mardi",
					"Mercredi", "Jeudi","Vendredi","Samedi"} ;

		static const char *mname[] = {"Janvier","Fevrier","Mars","Avril",
					"Mai","Juin","Juillet","Aout","Septembre","Octobre",
					"Novembre","Decembre"};
		static const char	szInvalidDay[] 		= "invalide jour";
		static const char	szInvalidDate[] 	= "invalide date";
		static const char	szInvalidMonth[]	= "invalide mois";
		static const char	szBCE[]				= " B.C.E.";
		static const char	szYears[]			= "annee";
		static const char	szMonths[]			= "mois";
		static const char	szDays[]			= "journee";
#endif


#else   // ENGLISH

						// ---------------------------------------------------------------------

		
#if 0
		static const char *dayname[]= {"Sunday","Monday","Tuesday",
					"Wednesday", "Thursday","Friday","Saturday"} ;

		static const char *mname[] = {"January","February","March","April",
					"May","June","July","August","September","October",
					"November","December"};

		static const char	szInvalidDay[] 		= "invalid day";
		static const char	szInvalidDate[] 	= "invalid date";
		static const char	szInvalidMonth[]	= "invalid month";
		static const char	szBCE[]				= " B.C.E.";
		static const char	szYears[]			= "years";
		static const char	szMonths[]			= "months";
		static const char	szDays[]			= "days";
#endif

#endif
							// -----------------------------------------------------------------

	#define	GetString(x)			(x)
	#define GetStringArray(x,y)		(x##[y-1])

	#define	INVALID_DAY			szInvalidDay
	#define	INVALID_DATE		szInvalidDate
	#define	INVALID_MONTH		szInvalidMonth
	#define	MONTHS				mname
	#define	DAYNAME				dayname

/*
 * OK, the deal here is to use fake access functions to the strings
 * we've just defined above, but only if this is NOT a Windows program.
 * "Why?" you may be asking... Glad you asked.  If this were a Windows
 * EXE, or better yet, a DLL, we'd want to put those strings in a
 * STRINGTABLE, which means we'll need an access function to get them.
 * And unless we want to have a DOS version and a separate Windows 
 * version, we'll need access functions for the DOS version too.
 * But it's ineffecient to have REAL access funcs, when we can just
 * fake it using a couple of #defines.  Here a "call" to 
 * "GetString(INVALID_DATE)" would be exactly the same as using 
 * "szInvalidDate" in the code.  Similarly, "GetStringArray(MONTHS,month)"
 * is the same as "mname[month-1];".
 * BUT -- When  it comes time to write the Windows version, all we
 * need do is write simple functions for GetString & GetStringArray, 
 * and replace the #defines above with numeric constants. Viola!
 *
 * In fact here's a start for the Windows functions:
 *		#define	GetStringArray(x,y)	GetString(x+y)
 * -JMC
 */


//#endif   // (_INC_WINDOWS)


static char GauDays[]	= { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int	DaysSoFar[][13] =
			{
			{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
			{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
			};



////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Date::Date()
{
	_month 	= NON_MONTH;
	_day 	= 0;
	_year 	= 0;
	_julian 	= 0L;

	_day_of_week = NON_DAY;
}

//////////////////////////////////////////////////////////////

Date::Date (long j) :
             _julian(j), _day_of_week(NON_DAY)
{
	julian_to_mdy ();
}

//////////////////////////////////////////////////////////////

Date::Date (short m, short d, short y) : 
             _year(y), _month(m), _day(d), _day_of_week(NON_DAY)
{
	ASSERT_D(_month > 0);
	ASSERT_D(_month < 13);

	ASSERT_D(_day   > 0);
	ASSERT_D(_day   < 32);

	mdy_to_julian ();
}

//////////////////////////////////////////////////////////////

Date::Date (const tm	&TM) :
            _day_of_week(NON_DAY)
{
		_month	= (short) (TM.tm_mon + 1);
		_day		= (short)  TM.tm_mday;
		_year	= (short) (TM.tm_year + 1900);

		mdy_to_julian();
}


//////////////////////////////////////////////////////////////

Date::Date (const Date &dt)
{
    *this = dt;
}


//////////////////////////////////////////////////////////////

Date::Date (int weeknum, int dow, short m, short y) : 
            _year(y), _month(m), _day(1), _day_of_week(NON_DAY)
{
		int	d;

		ASSERT_D(weeknum > -1);		// weeknum = 0 mean "the last" 
		ASSERT_D(weeknum < 6);
		ASSERT_D(dow >  NON_DAY);
		ASSERT_D(dow <= SATURDAY);
		ASSERT_D(m   >  NON_MONTH);
		ASSERT_D(m   <= DECEMBER);
		
		mdy_to_julian ();

        if (weeknum ==0)
		{
            addMonths(1);
		}

		d = (dow - _day_of_week);
		_julian = _julian + d;

		if (d > -1) 
			weeknum--;
			
		addWeeks(weeknum);
		
}


//////////////////////////////////////////////////////////////
// Date Arithmetic
//////////////////////////////////////////////////////////////


Date &Date::operator+= (long i)
{
		_julian += i;

		julian_to_mdy();

		return *this;
}

//////////////////////////////////////////////////////////////

Date &Date::operator -= (long i)
{
		_julian -= i;
		julian_to_mdy();

		return *this;
}

//////////////////////////////////////////////////////////////

Date Date::operator ++()
{
		_julian++;

		julian_to_mdy();

		return *this;
}

//////////////////////////////////////////////////////////////

Date Date::operator --()
{
		_julian--;

		julian_to_mdy();

		return *this;
}

//////////////////////////////////////////////////////////////
Date Date::operator ++(int)
{
		Date temp=*this;				// TML - Necessary to save current
										// value of (*this) in order to
		_julian++;						// simulate postfix operation!
		julian_to_mdy();

		return temp;
}

//////////////////////////////////////////////////////////////

Date Date::operator --(int)
{
		Date temp=*this;				// TML - Necessary to save current
										// value of (*this) in order to
		_julian--;						// simulate postfix operation!

		julian_to_mdy();

		return temp;
}


Date& Date::operator=(const Date& rhs)
{
    if ( this != &rhs )
    {
        _julian = rhs._julian;
        _year = rhs._year;
        _month = rhs._month;
        _day = rhs._day;
        _day_of_week = rhs._day_of_week;
    }

    return *this;
}


//////////////////////////////////////////////////////////////
// Conversion routines
//////////////////////////////////////////////////////////////

void Date::julian_to_wday (void)
{
	_day_of_week = (enum Wday) ((_julian + 2) % 7 + 1);
}

//////////////////////////////////////////////////////////////


#define OCT5_1582		(2299160L)		// "really" 15-Oct-1582
#define OCT14_1582		(2299169L)		// "really"  4-Oct-1582
#define JAN1_1			(1721423L)

#define YEAR			(365)
#define FOUR_YEARS		(1461)
#define CENTURY 		(36524L)
#define FOUR_CENTURIES	(146097L)

void Date::julian_to_mdy ()
{
	long	z,y;
	short 	m,d;
	int 	lp;

	z = _julian+1;
	if (z >= OCT5_1582)
	{
		z -= JAN1_1;
		z  = z + (z/CENTURY)  - (z/FOUR_CENTURIES) -2;
		z += JAN1_1;

	}

	z = z - ((z-YEAR) / FOUR_YEARS);		// Remove leap years before current year
	y = z / YEAR;

	d = (short) (z - (y * YEAR));

	y = y - 4712;				// our base year in 4713BC
	if (y < 1)
		y--;

	lp = !(y & 3);				// lp = 1 if this is a leap year.

	if (d==0)
	{
			y--;
			d = (short) (YEAR + lp);
	}

	m  = (short) (d/30);		// guess at month

	while (DaysSoFar[lp][m] >=d)
		m--;					// Correct guess.

	d = (short) (d - DaysSoFar[lp][m]);

	_day = d;

	_month = (short) (m+1);

	_year = (short) y;

	julian_to_wday ();
}

//////////////////////////////////////////////////////////////


// The original here was far more complicated then it needed to be.
// What we need to keep in mind is the simple rule:
//	  Before 10/4/1585, a leap year occured every 4 years.
//	  After  10/15/1585, leap years were skipped on centuries
//		not divisible by 400. Plus 10 days were skipped to adjust
//		for the past error.

void Date::mdy_to_julian (void)
{
	int		a;
	int		work_year=_year;
	long	j;
	int 	lp;

	// correct for negative year  (-1 = 1BC = year 0)

	if (work_year < 0)
			work_year++;

	lp = !(work_year & 3);			// lp = 1 if this is a leap year.

	j =
		((work_year-1) / 4)		+		// Plus ALL leap years
		DaysSoFar[lp][_month-1]	+
		_day					+
		(work_year * 365L)	+		// Days in years
		 JAN1_1 			+
		 -366;						// adjustments

	// deal with Gregorian calendar
	if (j >= OCT14_1582)
	{

		a = (int)(work_year/100);
		j = j+ 2 - a + a/4;			// Skip days which didn't exist.
	}

	_julian = j;

	julian_to_wday ();
}



///////////////////////////////////////////////////////////////
//  Miscellaneous Routines
///////////////////////////////////////////////////////////////


int Date::dayOfYear( void ) const
{
	Date temp( 1, 1, _year );

	return (int) (_julian - temp._julian + 1);
}


//////////////////////////////////////////////////////////////

int Date::isLeapYear( void ) const
{
	return  ( (_year >= 1582) ?
			  (((_year % 4 == 0)  &&  (_year % 100 != 0))  ||  (_year % 400 == 0) ):
			  (_year % 4 == 0) );
}

//////////////////////////////////////////////////////////////
bool Date::isDST( void ) const
{
    // Initialize start of DST and STD
    Date tempDST( (_startDST >> 8), (_startDST & 255), _year ) ;
    Date tempSTD( (_startSTD >> 8), (_startSTD & 255), _year ) ;

    tempDST += -tempDST._day_of_week + 8 ;    // DST begins first Sunday in April
    tempSTD -= (tempSTD._day_of_week - 1) ;   // STD begins last Sunday in October

    return( (bool) (_julian >= tempDST._julian && _julian < tempSTD._julian) ) ;
}
//////////////////////////////////////////////////////////////

bool Date::setDST(unsigned nMonth, unsigned nDay)
{
    if (nMonth < 13 && nDay < 32)
    {
        _startDST = (nMonth << 8) + nDay;
        return true;
    }
    else
        return false;
}
//////////////////////////////////////////////////////////////

bool Date::setSTD(unsigned nMonth, unsigned nDay)
{
    if (nMonth < 13 && nDay < 32)
    {
        _startSTD = (nMonth << 8) + nDay;
        return true;
    }
    else
        return false;
}


//////////////////////////////////////////////////////////////

//-------------------------------------------------
// Version 4.0 Extension to Interface - CDP
//-------------------------------------------------

Date &Date::set(void)
{
	tm			*local_time;
	time_t		timer;

	timer 	  = ::time(NULL);
	local_time = ::localtime(&timer);

	_month = (short) (local_time->tm_mon + 1);
	_day   = (short)  local_time->tm_mday;
	_year  = (short) (local_time->tm_year + 1900);

	mdy_to_julian();

	return *this;
}

//////////////////////////////////////////////////////////////

Date &Date::set(unsigned int	nMonth,unsigned int	nDay,unsigned int	nYear)
{
	_month = (short) nMonth;
	_year  = (short) (nYear > 9999 ? 0 : nYear);

	_day   = (short) ((nDay < daysInMonth()) ? nDay : daysInMonth());

	mdy_to_julian();

	return *this;
}

//////////////////////////////////////////////////////////////

Date &Date::set(long	j)
{
	_julian = j;

	julian_to_mdy();

	return *this;
}

//////////////////////////////////////////////////////////////

Date &Date::set(int weeknum, int dow, short m, short y)
{
		int	d;

		ASSERT_D(weeknum > -1);		// weeknum = 0 mean "the last" 
		ASSERT_D(weeknum < 6);
		ASSERT_D(dow >  NON_DAY);
		ASSERT_D(dow <= SATURDAY);
		ASSERT_D(m   >  NON_MONTH);
		ASSERT_D(m   <= DECEMBER);

		set(m,1,y);

		mdy_to_julian ();

        if (weeknum ==0)	
		{
		    addMonths(1);
		}

		d = (dow - _day_of_week);
		_julian = _julian + d;

		if (d > 0)
			weeknum--;
			
		addWeeks(weeknum);			// Julian_to_mdy done in AddWeeks()

		return(*this);		
}

//////////////////////////////////////////////////////////////

unsigned int Date::daysInMonth() const
{
    return (unsigned int) (GauDays[_month] + (_month==2 && isLeapYear()));
}

//////////////////////////////////////////////////////////////

Date &Date::addMonths(int nCount)
{
    int nDays;

	_month += (short) nCount;

	while (_month < 1)
	{
        _month += 12;
        _year--;
	}

	while (_month > 12)
	{
        _month -= 12;
        _year++;
	}

    nDays = (unsigned int) daysInMonth();

    if (_day > nDays)                    // Bump up the month by one if day is
    {                                   // greater than # of days in new month
        _month++;                        // and assigned the difference as the
        _day -= nDays;                   // day value for the new month! - TML
    }

	mdy_to_julian();

	return *this;
}

//////////////////////////////////////////////////////////////

Date & Date::addWeeks(int nCount)
{
    set(_julian + (long)nCount*7);
    return *this;
}


//////////////////////////////////////////////////////////////

Date &Date::addYears(int nCount)
{
    _year += (short) nCount;
    mdy_to_julian();

    return *this;
}

//////////////////////////////////////////////////////////////

int Date::weekOfMonth(void) const 
{
	// Abs day includes the days from previous month that fills up
	// the begin. of the week.

    int nAbsDay = _day + firstDayOfMonth()-1;
    return (nAbsDay-_day_of_week)/7 + 1;
}
//////////////////////////////////////////////////////////////


int Date::weekOfYear(void) const 
{
    Date   doTemp(1, 1, _year);
    return (int)(((_julian - doTemp._julian+1)/7) + 1);
}


