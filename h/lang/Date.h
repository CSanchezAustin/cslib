#ifndef DATECLS_H
#define DATECLS_H

/*
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
 *| Header.......: DATECL.H
 *| Date.........: Sat  12-01-1994
 *| Author.......: James M. Curran,  et al
 *| Version......: 5.0   Compile w/MSC++ 7.0 or Borland C++ 3.1 (or later versions)
 *| Usage........: General purpose date conversion, arithmetic,
 *|              :    comparison, and formatting class
 *| Compile line.: cl /AM /W3 /Zp /D_DOS /Osel /Gs /c datecl4.cpp
 *|              : cl /AM /W3 /Zp /D_DOS /Osel /Gs /c datedemo.cpp
 *| Link line....:
 *|    link /NOD /ONERROR:NOEXE datedemo date, datedemo, NUL, mafxcr mlibce;
 *|
 *| Acknowledgements:
 *|
 *|    Originally inspired by Steve Marcus (CIS 72007,1233)  6/16/91
 *|    Enhanced by Eric Simon (CIS 70540,1522)               6/29/91
 *|    Further Enhanced by Chris Hill (CIS 72030,2606)       7/11/91
 *|    Still Further Enhanced by Hill & Simon  v3.10         8/05/91
 *|
 *|    "It jist keeps on a 'git 'n bedder!"
 *|       by Charles D. Price (CIS 70541,3651) v4.0          6/27/92
 *|
 *|     Sealing the memory leaks...
 *|         some variable casts and string output.
 *|             by Kenneth A. Argo (CIS 71241,3635) v4.1        3/10/93
 *|
 *|     "Yet, more improvements..."
 *|             by Ly Minh Tr¡ (CIS 73062,512)  v4.2            3/13/93
 *|             ............................... v4.3            3/24/93
 *|             ............................... v4.4            6/03/93
 *|             ............................... v4.5            6/21/93
 *|             ............................... v4.6            8/04/93
 *|             ............................... v4.7            9/20/93
 *|             ............................... v4.8           11/18/93
 *|             ............................... v4.9            1/26/94
 *|
 *|      "All kinds of good stuff..."
 *|      by James M. Curran (CIS 72261,655)  v5.0    10/30/94
 *|
 *|
 *|    And the quest for the perfect date class continues....
 *|
 *+----------------------------------------------------------------------
 */

#include <time.h>

#include <cslib.h>

class EXPORT_DLL Date
{
public:
    //
    // TML - Put into class so we don't proliferate global names...in the
    //       tradition of the 'ios' class!
    //       Make use of the encapsulation feature of C++
    //
    enum Wday  {NON_DAY=0, SUNDAY=1,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY};                     
    enum Months   {NON_MONTH=0, JANUARY=1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST,
                   SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};
protected:
    unsigned long _julian; // see julDate();  days since 1/1/4713 B.C.
    short  _year;   // see NYear4()
    short _month;   // see NMonth()
    short  _day;   // see Day()
    short  _day_of_week; // see NDOW(); 1 = Sunday, ... 7 = Saturday

private:
    static unsigned int   _startDST;
    static unsigned int   _startSTD;

    void julian_to_mdy ();         // convert julian day to mdy
    void julian_to_wday ();        // convert julian day to day_of_week
    void mdy_to_julian ();         // convert mdy to julian day

public:
    Date ();
    Date (short m, short d, short y);
    Date (long  j);
    Date (const Date  &dt);
    Date (const tm   &TM);
    Date (int weeknum, int dow, short m, short y);


    virtual ~Date() {}              // Do nothing!

    inline Date operator + (long i) const {return Date(_julian + i);};
    inline Date operator + (int i) const {return Date(_julian + (long)i);};
    inline Date operator - (long i) const  {return Date(_julian - i);};
    inline Date operator - (int i) const {return Date(_julian - (long)i);};
    inline long operator - (const Date &dt) const {return ( _julian - dt._julian );};

    Date &operator += (long i);
    Date &operator -= (long i);
    Date &operator=(const Date& rhs);

    Date  operator ++ ();               // Prefix increment
    Date  operator -- ();               // Prefix decrement

    Date  operator ++ (int);            // Postfix increment
    Date  operator -- (int);            // Postfix decrement

    inline int operator <  (const Date &dt) const {return(_julian <  dt._julian);};
    inline int operator <= (const Date &dt) const {return(_julian <= dt._julian);};
    inline int operator >  (const Date &dt) const {return(_julian >  dt._julian);};
    inline int operator >= (const Date &dt) const {return(_julian >= dt._julian);};
    inline int operator == (const Date &dt) const {return(_julian == dt._julian);};
    inline int operator != (const Date &dt) const {return(_julian != dt._julian);};


    inline long julDate()  const {return(_julian);}; // returns julian date

    int  isLeapYear() const;  // returns true if leap year, false if not
    bool  isDST()   const;  // returns true if date is within Daylight
                                      // Savings Time (DST), false if not

    // Sets the month and day which DST and STD date begins!  This will
    // enable isDST() to return the correct result for regions other than
    // North America.  Returns true if month and day values are valid, false
    // otherwise - TML

    static bool setDST(unsigned nMonth, unsigned nDay);
    static bool setSTD(unsigned nMonth, unsigned nDay);


    //-------------------------------------------------
    // Version 4.0 Extension to Public Interface - CDP
    //-------------------------------------------------

    // These 'Set's modify the date object and actually SET it.
    // They all return a reference to self (*this)

    Date &set(void);       // Sets to current system date
    Date &set(long lJulian);
    Date &set(unsigned int nMonth, unsigned int nDay, unsigned int nYear);
    Date &set(int weeknum, int dow, short m, short y);
    Date &addMonths(int nCount = 1); // May also pass neg# to decrement

    Date &addWeeks(int nCount = 1); //
    Date &addYears(int nCount = 1); //

    unsigned int daysInMonth(void) const ; // Number of days in month (1..31)

    int weekOfMonth(void)   const; // Numeric Week Of Month  (1..6)
    int weekOfYear(void)   const; // Numeric Week Of Year  (1..52)

    // Numeric Day of date object
    inline int day(void)  const {return _day;}

    // eg. 1992
    inline int year() const {return _year;}

    // Month Number (1..12)
    inline int month() const {return _month;}

    // First Day Of Month (1..7)
    inline int firstDayOfMonth(void) const {return Date(_month, 1, _year)._day_of_week;}

    // (1..7)
    inline int dayOfWeek(void) const {return _day_of_week;}

    int dayOfYear()   const;      // returns relative date since Jan. 1

    // First Date Of Month
    inline Date monthStart()  const  {return(Date(_month, 1, _year));}

    // Last Date Of Month
    inline Date monthEnd()  const  {return((Date(_month, 1, _year).addMonths(1))-1);}

    // First Date Of Year
    inline Date yearStart()  const {return(Date(1, 1, _year));}

    // Last Date Of Year
    inline Date yearEnd()  const {return(Date(1, 1, _year+1)-1);}


#ifndef NO_HOLIDAYS
    inline  static Date NewYearsDay(short inyear) {return(Date(JANUARY, 1, inyear));}
    inline  static Date ValentinesDay(short inyear) {return(Date(FEBRUARY, 14, inyear));}
    inline  static Date PresidentsDay(short inyear) {return(Date(3, MONDAY, FEBRUARY, inyear));}
    inline  static Date StPatricksDay(short inyear) {return(Date(MARCH, 17, inyear));}
    inline  static Date MothersDay(short inyear) {return(Date(2, SUNDAY, MAY, inyear));}
    inline  static Date MemorialDay(short inyear) {return(Date(0, MONDAY, MAY, inyear));}
    inline  static Date FlagDay(short inyear)  {return(Date(JUNE, 14, inyear));}
    inline  static Date FathersDay(short inyear) {return(Date(3, SUNDAY, JUNE, inyear));}
    inline  static Date CanadaDay(short inyear)  {return(Date(JULY, 1, inyear));}
    inline  static Date IndependenceDay(short inyear) {return(Date(JULY, 4, inyear));}
    inline  static Date BastilleDay(short inyear) {return(Date(JULY, 14, inyear));}
    inline  static Date LaborDay(short inyear)  {return(Date(1, MONDAY, SEPTEMBER, inyear));}
    inline  static Date VeteransDay(short inyear) {return(Date(NOVEMBER, 11, inyear));}
    inline  static Date ThanksgivingDay(short inyear) {return(Date(4, THURSDAY, NOVEMBER, inyear));}
    inline  static Date ChristmasDay(short inyear) {return(Date(DECEMBER, 25, inyear));}
#endif // NO_HOLIDAYS

};

#endif
